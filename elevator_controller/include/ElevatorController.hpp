#ifndef ELEVATOR_CONTROLLER_HPP
#define ELEVATOR_CONTROLLER_HPP

#include <native/cond.h>
#include <native/mutex.h>
#include <native/task.h>

#include <netinet/in.h>
#include <vector>
#include <sys/mman.h>
#include <sys/socket.h>

#include "ElevatorControllerView.hpp"
#include "ElevatorSimulator.hpp"
#include "Heap.hpp"
#include "Message.hpp"

struct ECRTData {
	ECRTData();
	~ECRTData();

	RT_TASK ecThread, frThread, statusThread, supervisorThread;
	RT_MUTEX mutex;
	RT_MUTEX mutexBuffer;
	RT_COND freeCond;
};

struct ElevatorStatus {
	ElevatorStatus();

	unsigned char getCurrentFloor() 		const { return currentFloor; }
	unsigned char getDirection() 				const { return direction; }
	unsigned char getCurrentPosition() 	const { return currentPosition; }
	unsigned char getCurrentSpeed() 		const { return currentSpeed; }
	unsigned char getDestination() 			const { return destination; }
	unsigned char getServiceDirection()	const { return serviceDirection; }
	unsigned char getTaskActive() 			const { return taskActive; }
	bool getTaskAssigned() 							const { return taskAssigned; }
	bool getGDFailed() 									const { return GDFailed; }
	bool getGDFailedEmptyHeap() 				const { return GDFailedEmptyHeap; }

	void setCurrentFloor(		unsigned char currentFloor) 		{ this->currentFloor 			= currentFloor; }
	void setDirection(			unsigned char direction) 				{ this->direction 				= direction; }
	void setCurrentPosition(unsigned char currentPosition) 	{ this->currentPosition 	= currentPosition; }
	void setCurrentSpeed(		unsigned char currentSpeed) 		{ this->currentSpeed 			= currentSpeed; }
	void setDestination(		unsigned char destination) 			{ this->destination 			= destination; }
	void setServiceDirection(unsigned char serviceDirection){ this->serviceDirection 	= serviceDirection; }
	void setTaskActive(			unsigned char taskActive) 			{ this->taskActive 				= taskActive; }
	void setTaskAssigned(			bool taskAssigned) 			{ this->taskAssigned 			= taskAssigned; }
	void setGDFailed(					bool GDFailed) 					{ this->GDFailed 					= GDFailed; }
	void setGDFailedEmptyHeap(bool GDFailedEmptyHeap)	{ this->GDFailedEmptyHeap = GDFailedEmptyHeap; }

	unsigned char statusBuffer[2][BUFFSIZE];
	unsigned char bufferSelection;

	private:
		unsigned char currentFloor;
		unsigned char direction;
		unsigned char currentPosition;
		unsigned char currentSpeed;
		unsigned char destination;
		unsigned char serviceDirection;
		unsigned char taskActive;
		bool taskAssigned;
		bool GDFailed;
		bool GDFailedEmptyHeap;
};

class ElevatorController {
	public:
		/* Public Member Variables */
		ECRTData rtData;
		/* End Public Member Variables */

		ElevatorController();
		~ElevatorController();

		void connectToGD(char* gdAddress, int port);
		void addSimulator(ElevatorSimulator* es);
		void addView(ElevatorControllerView* ecv);

		/* Methods to be run in threads */
    void communicate();
		void floorRun();
		void supervise();
		void updateStatus();

		/* Communication */
		void waitForGDRequest();
    void sendStatus();

		/* Accessors */
		char getID() const { return this->id; }
		DownwardFloorRunHeap& getDownHeap() { return this->downHeap; }
		UpwardFloorRunHeap& getUpHeap() { return this->upHeap; }
		ElevatorSimulator* getSimulator() { return this->es; }

		/* Elevator Operation */
		void addHallCall(unsigned char floor, unsigned char direction);
		void addFloorSelection(unsigned char floor);
		void updateMissedFloor(unsigned char direction);
		void pushFloorButton(char floor);
		void openDoor();
		void closeDoor();
		void emergencyStop();

		/* Friends */
		friend void supervisorStartUp(void*);

	private:
		static char nextID;

		unsigned char id;
		int sock;
		struct sockaddr_in echoserver;

		std::vector<ElevatorControllerView*> views;

		ElevatorStatus eStat;

		DownwardFloorRunHeap downHeap;
		UpwardFloorRunHeap upHeap;
		std::vector<char> missedHallCalls;
		std::vector<char> missedFloorSelections;

		ElevatorSimulator* es;

		static char getNextID() {
			return nextID++;
		}

		void sendRegistration();
		void receiveAck();

		void sendMessage(const Message& message);
		void sendMessage(const char* message, int len);
		char* receiveTCP(unsigned int length);

		bool releaseFreeCond();
		void updateStatusBuffer();
};

#endif
