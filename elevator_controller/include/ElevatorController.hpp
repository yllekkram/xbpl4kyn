#ifndef ELEVATOR_CONTROLLER_HPP
#define ELEVATOR_CONTROLLER_HPP

#include <netinet/in.h>
#include <vector>
#include <sys/socket.h>

#include "ElevatorControllerView.hpp"

class ElevatorController {
	public:
		ElevatorController();
		~ElevatorController();

		void run();
		void addView(ElevatorControllerView* ecv);
		void waitForGDRequest();
		void connectToGD(char* gdAddress, int port);
		
		char getID() const { return this->id; }
		
		void pushFloorButton(char floor);
		void pushOpenDoor();
		void pushCloseDoorButton();
		void pushStopButton();

	private:
		static char nextID;
		
		char id;
		int sock;
		struct sockaddr_in echoserver;
		std::vector<ElevatorControllerView*> views;
				
		static char getNextID() {
			char temp = nextID;
			nextID++;
			return temp;
		}
		
		void sendRegistration();
		void receiveAck();
		
		void sendMessage(char * message, unsigned int len=0);
		char* receiveTCP(unsigned int length);
};

class ElevatorControllerStatus {
	public:
		char speed;
};

#endif
