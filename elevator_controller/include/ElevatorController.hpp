#ifndef ELEVATOR_CONTROLLER_HPP
#define ELEVATOR_CONTROLLER_HPP

#include <netinet/in.h>
#include <vector>
#include <sys/socket.h>

#include "ElevatorControllerView.hpp"
#include "Heap.hpp"
#include "Message.hpp"

class ElevatorController {
	public:
		ElevatorController();
		~ElevatorController();

		void connectToGD(char* gdAddress, int port);
		void addView(ElevatorControllerView* ecv);
    void run();

		void waitForGDRequest();
    void sendStatus();

		char getID() const { return this->id; }
		DownwardFloorRunHeap& getDownHeap() { return this->downHeap; }
		UpwardFloorRunHeap& getUpHeap() { return this->upHeap; }

		void pushFloorButton(char floor);
		void openDoor();
		void closeDoor();
		void emergencyStop();

	private:
		static char nextID;

		unsigned char id;
		int sock;
		struct sockaddr_in echoserver;
		std::vector<ElevatorControllerView*> views;
		DownwardFloorRunHeap downHeap;
		UpwardFloorRunHeap upHeap;

		static char getNextID() {
			return nextID++;
		}

    void receiveHallCall(HallCallAssignmentMessage& message);

		void sendRegistration();
		void receiveAck();

		void sendMessage(const Message& message);
		void sendMessage(const char* message, int len);
		char* receiveTCP(unsigned int length);
};

#endif
