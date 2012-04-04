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
			
			if(!this->eStat.getServiceDirection())
			{
				if(downHeapSize > 0)
				{
					topItem = this->getDownHeap().peek();
					this->eStat.setServiceDirection(false);
				}else if(upHeapSize > 0)
				{
					this->updateMissedFloor(DIRECTION_DOWN);
					topItem = this->getUpHeap().peek();
					this->eStat.setServiceDirection(true);
				}
			}else
			{
				if(upHeapSize > 0)
				{
					topItem = this->getUpHeap().peek();
					this->eStat.setServiceDirection(true);
				}else if(downHeapSize > 0)
				{
					this->updateMissedFloor(DIRECTION_UP);
					topItem = this->getDownHeap().peek();
					this->eStat.setServiceDirection(false);
				}
			}
		
			if(topItem != this->eStat.getDestination())
			{
				rt_printf("FR%d next Dest is %d\n.", this->getID(), topItem);
				this->getSimulator()->setFinalDestination(topItem);
				this->eStat.setDestination(topItem);
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
		this->eStat.setCurrentFloor(this->getSimulator()->getCurrentFloor());
		this->eStat.setTaskAssigned(this->getSimulator()->getIsTaskActive());

		if(this->eStat.getTaskAssigned() && (this->getSimulator()->getDirection() == DIRECTION_UP))
		{
      this->eStat.setDirection(DIRECTION_UP);
		}
    else {
			this->eStat.setDirection(DIRECTION_DOWN);
    }

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
		this->releaseFreeCond();

		rt_mutex_release(&(this->rtData.mutex));
		this->updateStatusBuffer();
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
	//upheap and the downheap (hallcall and floor selections should be included.
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
			rt_printf("EC%d: Staus Request\n", (unsigned int)this->getID());
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
	FloorRunHeap& heap = (this->eStat.getDirection() == DIRECTION_UP) ? static_cast<FloorRunHeap&>(this->upHeap) : this->downHeap;
	std::vector<char>* hallCalls = heap.getHallCalls();
	std::vector<char>* floorRequests = heap.getFloorRequests();

	char len = 1 	/* EC ID */
						+1	/* Message Type */
						+1	/* Floor */
						+1	/* Direction */
						+1	/* Is moving? */
						+1	/* num hall calls */
						+hallCalls->size()*2	/* Hall calls */
						+1	/* Num Floor Requests */
						+floorRequests->size()	/* Floor requests */
						+1;	/* Terminator */

	char message[len];
	message[0] = STATUS_RESPONSE;
	message[1] = this->id;
	message[2] = this->eStat.getCurrentFloor();
	message[3] = this->eStat.getDirection();
	message[4] = (this->eStat.getCurrentSpeed() != 0) ? 1 : 0;

	/* Add variable-length data */
	char offset = 0;

	message[5] = hallCalls->size();
	for (std::vector<char>::iterator it = hallCalls->begin(); it != hallCalls->end(); ++it) {
		message[++offset + 5] = *it;
		message[++offset + 5] = this->eStat.getDirection();
	}

	message[offset + 6] = floorRequests->size();
	for (std::vector<char>::iterator it = floorRequests->begin(); it != floorRequests->end(); ++it) {
		message[++offset + 6] = *it;
	}


	message[7 + offset] = MESSAGE_TERMINATOR;

	this->sendMessage(message, len);

	delete hallCalls;
	delete floorRequests;
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
	if (this->eStat.getCurrentSpeed() == 0) {
		// If the elevator is stationary, always add the hall call to the heap
		if (callDirection == DIRECTION_UP) {
			this->upHeap.pushHallCall(floor);
			rt_printf("EC%d: Added %d to upHeap\n", (unsigned int)this->getID(), (unsigned int)floor);
		}
		else if (callDirection == DIRECTION_DOWN) {
			this->downHeap.pushHallCall(floor);
			rt_printf("EC%d: Added %d to downHeap\n", (unsigned int)this->getID(), (unsigned int)floor);
		}
		else {
			rt_printf("EC%d: Invalid direction for hall call: %d\n", (unsigned int)this->getID(), (unsigned int)floor);
		}
	}
	else {
		// If the elevator is moving, we need to check if it can stop at the given floor
		if (this->eStat.getDirection() == DIRECTION_UP) {
			rt_printf("EC%d: Checking if up stop at %d possible\n", (unsigned int)this->getID(), (unsigned int)floor);

			if (eStat.getCurrentFloor() >= (floor - 1)) {
				// Cannot stop in time
				this->missedHallCalls.push_back(floor);
			}
			else {
				// Can Stop in time
				this->upHeap.pushHallCall(floor);
			}
		}
		else if (this->eStat.getDirection() == DIRECTION_DOWN) {
			rt_printf("EC%d: Checking if down stop at %d possible\n", (unsigned int)this->getID(), (unsigned int)floor);

			if (this->eStat.getCurrentFloor() <= (floor + 1)) {
				// Cannot stop in time
				this->missedHallCalls.push_back(floor);
			}
			else {
				//Can stop in time
				this->downHeap.pushHallCall(floor);
			}
		}
		else {
			rt_printf("EC%d: Invalid direction for hall call: %d\n", (unsigned int)this->getID(), (unsigned int)floor);
		}
	}
}

void ElevatorController::addFloorSelection(unsigned char floor) {
	if(es->getDirection() == DIRECTION_UP)
	{
		if (es->getCurrentFloor() >= (floor - 1)) { // The elevator cannot stop at the floor
			this->missedFloorSelections.push_back(floor);
		}
		else {
			this->upHeap.pushFloorRequest(floor);
		}
	}

	if(es->getDirection() == DIRECTION_DOWN)
	{
		if (es->getCurrentFloor() <= (floor + 1)) {
			this->missedFloorSelections.push_back(floor);
		}
		else {
			this->downHeap.pushFloorRequest(floor);
		}
	}
}

void ElevatorController::updateMissedFloor(unsigned char direction)
{
	// Add missed hall calls
	if(direction == DIRECTION_UP) {
		rt_printf("EC%d: Appending missed up hall calls\n", (unsigned int)this->getID());
		this->upHeap.pushHallCallVector(this->missedHallCalls);
	}
	else if (direction == DIRECTION_DOWN){
		rt_printf("EC%d: Appending missed down hall calls\n", (unsigned int)this->getID());
		this->downHeap.pushHallCallVector(this->missedHallCalls);
	}
	else {
		rt_printf("EC%d: Unknown direction %d\n", (unsigned int)this->getID(), (unsigned int)direction);
	}
	this->missedHallCalls.clear();
	
	// Add missed floor requests
	if(direction == DIRECTION_UP) {
		rt_printf("EC%d: Appending missed down floor requests\n", (unsigned int)this->getID());
		this->downHeap.pushFloorRequestVector(this->missedFloorSelections);
	}
	else if (direction == DIRECTION_DOWN) {
		rt_printf("EC%d: Appending missed up floor requests\n", (unsigned int)this->getID());
		this->upHeap.pushFloorRequestVector(this->missedFloorSelections);
	}
	else {
		rt_printf("EC%d: Unknown direction %d\n", (unsigned int)this->getID(), (unsigned int)direction);
	}
	this->missedFloorSelections.clear();
}
