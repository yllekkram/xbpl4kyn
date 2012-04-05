#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <native/cond.h>
#include <native/mutex.h>
#include <netinet/in.h>
#include <rtdk.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"
#include "ElevatorSimulator.hpp"
#include "Exception.hpp"

#define DEBUG_ADD_HALL_CALL

char ElevatorController::nextID = 1;

ECRTData::ECRTData() {
	mlockall(MCL_CURRENT|MCL_FUTURE);

	rt_mutex_create(&(this->mutex), NULL);
	rt_mutex_create(&(this->mutexBuffer), NULL);
	rt_cond_create(&(this->freeCond), NULL);

	rt_task_create(&(this->ecThread), NULL, 0, 99, T_JOINABLE);
	rt_task_create(&(this->frThread), NULL, 0, 99, T_JOINABLE);
	rt_task_create(&(this->statusThread), NULL, 0, 99, T_JOINABLE);
	rt_task_create(&(this->supervisorThread), NULL, 0, 99, T_JOINABLE);
}

ECRTData::~ECRTData() {
	rt_task_delete(&(this->supervisorThread));
	rt_task_delete(&(this->statusThread));
	rt_task_delete(&(this->frThread));
	rt_task_delete(&(this->ecThread));

	rt_cond_delete(&(this->freeCond));
	rt_mutex_delete(&(this->mutexBuffer));
	rt_mutex_delete(&(this->mutex));
}

ElevatorStatus::ElevatorStatus()
	: bufferSelection(0), currentFloor(0),	direction(DIRECTION_UP),
		currentPosition(0),	currentSpeed(0),	destination(0),
		serviceDirection(DIRECTION_UP),				taskActive(false),
		taskAssigned(0),		GDFailed(false),	GDFailedEmptyHeap(false)
{}


ElevatorController::ElevatorController()
	: rtData(), eStat(), downHeap(), upHeap(), missedHallCalls(), missedFloorSelections() {
	this->id = ElevatorController::getNextID();

	if ((this->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Failed to create socket");
	}
}

ElevatorController::~ElevatorController() {
	close(this->sock);
}

void ElevatorController::communicate() {
	while (!this->eStat.getGDFailed()) {
		try {
			this->waitForGDRequest();
		}
		catch (Exception e) {
			rt_printf("%s\n", e.what());
			exit(1);
		}
	}
	rt_printf("EC%d dropping comm thread\n", (unsigned int)this->getID());
}

void ElevatorController::floorRun() {
	unsigned char topItem;
	while(true)
	{
		if(!this->eStat.getGDFailedEmptyHeap())
		{
			rt_mutex_acquire(&(this->rtData.mutex), TM_INFINITE);
			rt_cond_wait(&(this->rtData.freeCond), &(this->rtData.mutex), TM_INFINITE);

			int upHeapSize = this->getUpHeap().getSize();
			int downHeapSize = this->getDownHeap().getSize();
			if(upHeapSize==0 && downHeapSize==0){this->eStat.setGDFailedEmptyHeap(true);}

			if(this->eStat.getServiceDirection() == DIRECTION_DOWN)
			{
				if(downHeapSize > 0)
				{
					topItem = this->getDownHeap().peek();
				}else if(upHeapSize > 0)
				{
					topItem = this->getUpHeap().peek();
				}this->downHeap.pushFloorRequestVector(this->missedFloorSelections);
			}else if(this->eStat.getServiceDirection() == DIRECTION_UP)
			{
				if(upHeapSize > 0)
				{
					topItem = this->getUpHeap().peek();
				}else if(downHeapSize > 0)
				{
					topItem = this->getDownHeap().peek();
				}
			}
		
			int tempDirection = this->eStat.getDirection();
			if(topItem != this->eStat.getDestination())
			{
				rt_printf("FR%d next Dest is %d\n.", this->getID(), topItem);
				this->getSimulator()->setFinalDestination(topItem);
				this->eStat.setDestination(topItem);
				rt_printf("FR%d tempDirection is %d afterDirection is %d\n.", this->getID(), tempDirection, this->getSimulator()->getDirection());
			}

			if(tempDirection != this->getSimulator()->getDirection())
			{
				this->updateMissedFloor(tempDirection);
				this->notifyDirectionChanged(this->getSimulator()->getDirection());
			}

			rt_mutex_release(&(this->rtData.mutex));
		}
	}
}

void ElevatorController::supervise() {
	while(true)
	{
		rt_mutex_acquire(&(this->rtData.mutex), TM_INFINITE);
		if(this->eStat.getGDFailed() && this->eStat.getGDFailedEmptyHeap())
		{
			if(!this->eStat.getTaskAssigned())
			{
				if(((this->eStat.getDirection() == DIRECTION_DOWN) && this->eStat.getDestination()!=0) || this->eStat.getDestination() == MAX_FLOORS)
				{
					this->eStat.setDestination(this->eStat.getDestination() - 1);
					this->getSimulator()->setFinalDestination(this->eStat.getDestination());
				}else if(this->eStat.getDestination() == 0 || this->eStat.getDestination() != MAX_FLOORS)
				{
					this->eStat.setDestination(this->eStat.getDestination() + 1);
					this->getSimulator()->setFinalDestination(this->eStat.getDestination());
				}
			}
		}
		rt_mutex_release(&(this->rtData.mutex));
		sleep(20);
	}
}

void ElevatorController::updateStatus() {
	rt_task_set_periodic(NULL, TM_NOW, 75000000);

	while(true)
	{
		rt_task_wait_period(NULL);

		rt_mutex_acquire(&(this->rtData.mutex), TM_INFINITE);
		int tempFloor = this->eStat.getCurrentFloor();
		this->eStat.setCurrentFloor(this->getSimulator()->getCurrentFloor());
		if(tempFloor != this->eStat.getCurrentFloor())
		{
			this->notifyFloorReached(this->eStat.getCurrentFloor());
		}
		this->eStat.setTaskAssigned(this->getSimulator()->getIsTaskActive());
		this->eStat.setCurrentSpeed(this->getSimulator()->getCurrentSpeed());
		this->eStat.setDirection(this->getSimulator()->getDirection());

		this->eStat.setCurrentPosition((unsigned char)ceil(this->getSimulator()->geCurrentPosition()));
		this->eStat.setTaskActive(this->eStat.getTaskAssigned());

		int upHeapSize = this->getUpHeap().getSize();
		int downHeapSize = this->getDownHeap().getSize();
		if(upHeapSize==0 && downHeapSize==0){this->eStat.setGDFailedEmptyHeap(true);}

		if(upHeapSize > 0)
		{
			int topItem = (int)(this->getUpHeap().peek());
			if(this->eStat.getCurrentFloor() == topItem && !this->eStat.getTaskAssigned())
			{
				rt_printf("ST%d Task Completed %d\n", this->getID(), this->eStat.getDestination());
				this->getUpHeap().pop();
			}
		}

		if(downHeapSize > 0)
		{
			int topItem = (int)(this->getDownHeap().peek());
			if(this->eStat.getCurrentFloor() == topItem && !this->eStat.getTaskAssigned())
			{
				rt_printf("ST%d Task Completed %d\n", this->getID(), this->eStat.getDestination());
				this->getDownHeap().pop();
			}
		}

		upHeapSize = this->getUpHeap().getSize();
		downHeapSize = this->getDownHeap().getSize();
		int total = upHeapSize + downHeapSize;
		if(this->eStat.getDirection()==DIRECTION_UP && upHeapSize == 0 && downHeapSize > 0){ this->eStat.setServiceDirection(DIRECTION_DOWN); }
		else if(this->eStat.getDirection()==DIRECTION_UP && upHeapSize > 0){ this->eStat.setServiceDirection(DIRECTION_UP); }
		else if(this->eStat.getDirection()==DIRECTION_DOWN && downHeapSize == 0 && upHeapSize > 0){ this->eStat.setServiceDirection(DIRECTION_UP); }
		else if(this->eStat.getDirection()==DIRECTION_DOWN && downHeapSize > 0){ this->eStat.setServiceDirection(DIRECTION_DOWN); }
		if(total==0){this->updateMissedFloor(this->eStat.getDirection());}
		this->releaseFreeCond();

		rt_mutex_release(&(this->rtData.mutex));
		this->updateStatusBuffer();

		/*rt_printf("UPDATED : currentSpeed %d currentFloor %d task %d\n",
			(unsigned int)this->eStat.getCurrentSpeed(),	(unsigned int)this->eStat.getCurrentFloor(),	this->eStat.getTaskActive());*/
	}
}

bool ElevatorController::releaseFreeCond()
{
	int heapSize = this->getUpHeap().getSize() + this->getDownHeap().getSize();
	if(heapSize > 0)
	{
		this->eStat.setGDFailedEmptyHeap(false);
		rt_cond_signal(&(this->rtData.freeCond));
		return true;
	}else
	{
		return false;
	}
}

void ElevatorController::updateStatusBuffer()
{
	//upheap and the downheap (hallcall and floor selections should benotifyDirectionChanged included.
	rt_mutex_acquire(&(this->rtData.mutexBuffer), TM_INFINITE);
	unsigned char selectedBuffer = this->eStat.bufferSelection;
	rt_mutex_release(&(this->rtData.mutexBuffer));
	
	rt_mutex_acquire(&(this->rtData.mutex), TM_INFINITE);
	this->eStat.statusBuffer[selectedBuffer][STATUS_CURRENT_FLOOR_INDEX] = this->eStat.getCurrentFloor();
	this->eStat.statusBuffer[selectedBuffer][STATUS_DIRECTION_INDEX] = this->eStat.getDirection();
	this->eStat.statusBuffer[selectedBuffer][STATUS_CURRENT_POSITION_INDEX] = this->eStat.getCurrentPosition();
	this->eStat.statusBuffer[selectedBuffer][STATUS_CURRENT_SPEED_INDEX] = this->eStat.getCurrentSpeed();
	//rt_printf("writting to buffer %d %s\n", selectedBuffer, statusBuffer[selectedBuffer]);
	rt_mutex_release(&(this->rtData.mutex));

	rt_mutex_acquire(&(this->rtData.mutexBuffer), TM_INFINITE);
	this->eStat.bufferSelection = ++(this->eStat.bufferSelection) % 2;
	rt_mutex_release(&(this->rtData.mutexBuffer));
}

void ElevatorController::addSimulator(ElevatorSimulator* es) {
	this->es = es;
}

void ElevatorController::addView(ElevatorControllerView* ecv) {
	this->views.push_back(ecv);
	ecv->setController(this);
}

void ElevatorController::waitForGDRequest() {
	char* request;
	try {
		request = receiveTCP(MAX_GD_REQUEST_SIZE);
	}
	catch (Exception e) {
		rt_printf("GD comm failed for EC%d, enabling supervisor thread\n", (unsigned int)this->getID());
		this->eStat.setGDFailed(true);
		return;
	}
	char requestType = request[0];

	switch (requestType) {
		case STATUS_REQUEST:
			// rt_printf("EC%d: Status Request\n", (unsigned int)this->getID());
      this->sendStatus();
			break;
		case HALL_CALL_ASSIGNMENT:
			rt_printf("EC%d: Received hall call for floor %d in %s direction\n",
					(unsigned int)this->getID(), (unsigned int)request[HCA_FLOOR_INDEX],
					((request[HCA_DIRECTION_INDEX] == DIRECTION_DOWN) ? "downward" : "upward"));
			this->addHallCall(request[HCA_FLOOR_INDEX], request[HCA_DIRECTION_INDEX]);
			break;
		default:
			rt_printf("EC%d: Unknown Message Type\n", (unsigned int)this->getID());
	}
}

void ElevatorController::sendStatus() {
	std::vector<char>* upHallCalls = upHeap.getHallCalls();
	std::vector<char>* downHallCalls = downHeap.getHallCalls();
	std::vector<char>* upFloorRequests = upHeap.getFloorRequests();
	std::vector<char>* downFloorRequests = downHeap.getFloorRequests();

	char numHallCalls = upHallCalls->size() + downHallCalls->size() + missedHallCalls.size();
	char numFloorRequests = upFloorRequests->size() + downFloorRequests->size() + missedFloorSelections.size();

	char len = 1 	/* EC ID */
						+1	/* Message Type */
						+1	/* Floor */
						+1	/* Direction */
						+1	/* Is moving? */
						+1	/* num hall calls */
						+numHallCalls * 2	/* Hall calls */
						+1	/* Num Floor Requests */
						+numFloorRequests	/* Floor requests */
						+1;	/* Terminator */

	char message[len];
	message[0] = STATUS_RESPONSE;
	message[1] = this->id;
	message[2] = this->eStat.getCurrentFloor();
	message[3] = this->eStat.getDirection();
	message[4] = (this->eStat.getCurrentSpeed() != 0) ? 1 : 0;

	/* Add variable-length data */
	char offset = 0;

	// Hall Calls in up direction
	message[5] = numHallCalls;
	for (std::vector<char>::iterator it = upHallCalls->begin(); it != upHallCalls->end(); ++it) {
		message[++offset + 5] = *it;
		message[++offset + 5] = DIRECTION_UP;
	}
	// Hall Calls in down direction
	for (std::vector<char>::iterator it = downHallCalls->begin(); it != downHallCalls->end(); ++it) {
		message[++offset + 5] = *it;
		message[++offset + 5] = DIRECTION_DOWN;
	}
	// Floors that couldn't be stopped at
	for (std::vector<char>::iterator it = missedHallCalls.begin(); it != missedHallCalls.end(); ++it) {
		message[++offset + 5] = *it;
		message[++offset + 5] = this->eStat.getDirection();
	}

	// Floor requests in the up direction
	message[offset + 6] = numFloorRequests;
	for (std::vector<char>::iterator it = upFloorRequests->begin(); it != upFloorRequests->end(); ++it) {
		message[++offset + 6] = *it;
	}

	// Floor requests in the down direction
	message[offset + 6] = downFloorRequests->size();
	for (std::vector<char>::iterator it = downFloorRequests->begin(); it != downFloorRequests->end(); ++it) {
		message[++offset + 6] = *it;
	}

	// Missed Floor requests
	message[offset + 6] = missedFloorSelections.size();
	for (std::vector<char>::iterator it = missedFloorSelections.begin(); it != missedFloorSelections.end(); ++it) {
		message[++offset + 6] = *it;
	}

	message[7 + offset] = MESSAGE_TERMINATOR;

	this->sendMessage(message, len);

	delete upHallCalls;
	delete downHallCalls;
	delete upFloorRequests;
	delete downFloorRequests;
}

void ElevatorController::connectToGD(char* gdAddress, int port) {
	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));
	echoserver.sin_family = AF_INET;
	echoserver.sin_addr.s_addr = inet_addr(gdAddress);
	echoserver.sin_port = htons(port);

	rt_printf("EC%d: Connecting to GroupDispatcher...", this->getID());
	/* Establish connection */
	if (connect(this->sock,
				(struct sockaddr *) &(echoserver),
			sizeof(echoserver)) < 0) {
		//Die("Failed to connect with server");
	}
  rt_printf("done.\n");
	this->sendRegistration();
}

void ElevatorController::sendRegistration() {
	rt_printf("EC%d: Sending EC->GD registration...", (unsigned int)this->getID());
	sendMessage(RegisterWithGDMessage(this->getID()));

	rt_printf("done.\n");

	receiveAck();
}

void ElevatorController::receiveAck() {
	rt_printf("EC%d: Waiting for EC->GD ack...", (unsigned int)this->getID());
	char* message = this->receiveTCP(2);
	rt_printf("done.\n");
	if (message[0] != REGISTRATION_ACK)
		Die("EC->GD Registration not acknowledged");
}

void ElevatorController::sendMessage(const Message& message) {
	this->sendMessage(message.getBuffer(), message.getLen());
}

void ElevatorController::sendMessage(const char* message, int len) {
	if (len == 0) {
		len = strlen(message);
	}
	/* Send the word to the server */
	if (send(this->sock, message, len, 0) != len) {
		Die("Mismatch in number of sent bytes");
	}
}

char* ElevatorController::receiveTCP(unsigned int length) {
	char* buffer = new char[BUFFSIZE];

	/* Receive the word back from the server */
	int bytes = 0;
	//std::cout << "EC" << (unsigned int)this->getID() << ": Waiting for TCP...";
	bytes = recv(this->sock, buffer, BUFFSIZE-1, 0);
	if (bytes <= 0) {
		Die("bytes received error");
	}
	//std::cout << "got " << bytes << " bytes" << std::endl;

	//std::cout << "EC" << (unsigned int)this->getID() << ": Received: ";
	//printBuffer(buffer, bytes);
	//std::cout << std::endl;

	return buffer;
}

void ElevatorController::openDoor() {
	rt_printf("EC%d: opening door\n", (unsigned int)this->getID());
}

void ElevatorController::closeDoor() {
	rt_printf("EC%d: closing door\n", (unsigned int)this->getID());
}

void ElevatorController::emergencyStop() {
	rt_printf("EC%d: emergency stop\n", (unsigned int)this->getID());
}

void ElevatorController::addHallCall(unsigned char floor, unsigned char callDirection) {
	char localServiceDir = this->eStat.getServiceDirection();
#ifdef DEBUG_ADD_HALL_CALL
  rt_printf("EC%d: service direction = %d\n", this->getID(), localServiceDir);
#endif
  
  switch (callDirection) {

		case DIRECTION_UP:

			switch (localServiceDir) {

				case DIRECTION_UP:

					if (this->eStat.getCurrentSpeed() == 0) {
						if (this->eStat.getCurrentFloor() <= floor) {
							/* Call Up, Direction Up, Stationary, Below Destination */
#ifdef DEBUG_ADD_HALL_CALL
							rt_printf("EC%d: up, up, stat, below - upHeap\n", (unsigned int)this->getID());
#endif
							this->upHeap.pushHallCall(floor);
						}
						else {
							/* Call Up, Direction Up, Staionary, Above Destination */
#ifdef DEBUG_ADD_HALL_CALL
							rt_printf("EC%d: up, up, stat, above - missed\n", (unsigned int)this->getID());
#endif
							this->missedHallCalls.push_back(floor);
						}
					}
					else {
						if (this->eStat.getCurrentFloor() <= (floor - 1)) {
							/* Call Up, Direction Up, Moving, Below Destination */
#ifdef DEBUG_ADD_HALL_CALL
							rt_printf("EC%d: up, up, moving, below - upHeap\n", (unsigned int)this->getID());
#endif
							this->upHeap.pushHallCall(floor);
						}
						else {
							/* Call Up, Direction Up, Moving, Above Destination */
#ifdef DEBUG_ADD_HALL_CALL
							rt_printf("EC%d: up, up, moving, above - missed\n", (unsigned int)this->getID());
#endif
							this->missedHallCalls.push_back(floor);
						}
					}

					break;

				case DIRECTION_DOWN:
					/* Call Up, Direction Down, N/A, N/A */
#ifdef DEBUG_ADD_HALL_CALL
					rt_printf("EC%d: up, down, N/A, N/A - upHeap\n", (unsigned int)this->getID());
#endif
					this->upHeap.pushHallCall(floor);
					break;

				default:
					rt_printf("EC%d: Unknown elevator direction (%d)\n", (unsigned int)this->getID(), localServiceDir);
			}

			break;

		case DIRECTION_DOWN:

			switch (localServiceDir) {

				case DIRECTION_UP:
					/* Call Down, Direction Up, N/A, N/A */
#ifdef DEBUG_ADD_HALL_CALL
					rt_printf("EC%d: down, up, N/A, N/A - downHeap\n", (unsigned int)this->getID());
#endif
					this->downHeap.pushHallCall(floor);
					break;

				case DIRECTION_DOWN:
					if (this->eStat.getCurrentSpeed() == 0) {
						if (this->eStat.getCurrentFloor() >= floor) {
							/* Call Down, Direction Down, Stationary, Above Destination */
#ifdef DEBUG_ADD_HALL_CALL
							rt_printf("EC%d: down, down, stat, above - downHeap\n", (unsigned int)this->getID());
#endif
							this->downHeap.pushHallCall(floor);
						}
						else {
							/* Call Down, Direction Down, Stationary, Below Destination */
#ifdef DEBUG_ADD_HALL_CALL
							rt_printf("EC%d: down, down, stat, below - missed\n", (unsigned int)this->getID());
#endif
							this->missedHallCalls.push_back(floor);
						}
					}
					else {
						if (this->eStat.getCurrentFloor() >= (floor + 1)) {
							/* Call Down, Direction Down, Moving, Above Destination */
#ifdef DEBUG_ADD_HALL_CALL
							rt_printf("EC%d: down, down, moving, above - downHeap\n", (unsigned int)this->getID());
#endif
							this->downHeap.pushHallCall(floor);
						}
						else {
							/* Call Down, Direction Down, Moving, Below Destination */
#ifdef DEBUG_ADD_HALL_CALL
							rt_printf("EC%d: down, down, moving, below - missed\n", (unsigned int)this->getID());
#endif
							this->missedHallCalls.push_back(floor);
						}
					}
					break;

				default:
					rt_printf("EC%d: Unknown elevator direction (%d)\n", (unsigned int)this->getID(), localServiceDir);
			}

			break;

		default:
			rt_printf("EC%d: Unkown hall call direction (%d)\n", (unsigned int)this->getID(), (unsigned int)callDirection);
	}
}

void ElevatorController::addFloorSelection(unsigned char floor) {
	if (this->eStat.getCurrentSpeed() == 0) {
		if ((this->eStat.getDirection() == DIRECTION_UP) && (this->eStat.getCurrentFloor() < floor)) {
#ifdef DEBUG_ADD_FLOOR_REQUEST
			rt_printf("EC%d: Added floor request to upHeap\n", this->getID());
#endif
			this->upHeap.pushFloorRequest(floor);
		}
		else if ((this->eStat.getDirection() == DIRECTION_DOWN) && (this->eStat.getCurrentFloor() > floor)) {
#ifdef DEBUG_ADD_FLOOR_REQUEST
			rt_printf("EC%d: Added floor request to downHeap\n", this->getID());
#endif
			this->downHeap.pushFloorRequest(floor);
		}
		else {
#ifdef DEBUG_ADD_FLOOR_REQUEST
			rt_printf("EC%d: Added floor request to missed list\n", this->getID());
#endif
			missedFloorSelections.push_back(floor);
		}
	}
	else {
		if ((this->eStat.getDirection() == DIRECTION_UP) && (this->eStat.getCurrentFloor() < (floor - 1))) {
			this->upHeap.pushFloorRequest(floor);
		}
		else if ((this->eStat.getDirection() == DIRECTION_DOWN) && (this->eStat.getCurrentFloor() > (floor + 1))) {
			this->downHeap.pushFloorRequest(floor);
		}
		else {
			missedFloorSelections.push_back(floor);
		}
	}
}

void ElevatorController::updateMissedFloor(unsigned char direction)
{
	// Add missed hall calls
	if(direction == DIRECTION_UP) {
		rt_printf("EC%d: Appending missed up hall calls\n", (unsigned int)this->getID());
		this->upHeap.pushHallCallVector(this->missedHallCalls);
		this->downHeap.pushFloorRequestVector(this->missedFloorSelections);
	}
	else if (direction == DIRECTION_DOWN){
		rt_printf("EC%d: Appending missed down hall calls\n", (unsigned int)this->getID());
		this->downHeap.pushHallCallVector(this->missedHallCalls);
		this->upHeap.pushFloorRequestVector(this->missedFloorSelections);
	}
	else {
		rt_printf("EC%d: Unknown direction %d\n", (unsigned int)this->getID(), (unsigned int)direction);
	}
	this->missedHallCalls.clear();
	this->missedFloorSelections.clear();
}

void ElevatorController::notifyFloorReached(unsigned char floor)
{
	for(std::vector<ElevatorControllerView*>::iterator it = views.begin(); it != views.end(); ++it)
	{
		(*it) -> notifyFloorReached(floor);
	}
}

void ElevatorController::notifyDirectionChanged(unsigned char direction)
{
	for(std::vector<ElevatorControllerView*>::iterator it = views.begin(); it != views.end(); ++it)
	{
		(*it) -> notifyDirectionChanged(direction);
	}
}
