#ifndef ELEVATOR_CONTROLLER_HPP
#define ELEVATOR_CONTROLLER_HPP

#include <netinet/in.h>
#include <vector>
#include <sys/socket.h>

#include "ElevatorControllerView.hpp"
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
			return nextID++;
		}
		
    void receiveHallCall(HallCallAssignmentMessage message);
    
		void sendRegistration();
		void receiveAck();
		
		void sendMessage(const Message& message);
		void sendMessage(const char* message, int len);
		char* receiveTCP(unsigned int length);
};

class ElevatorControllerStatus {
	public:
		char speed;
    char destination;
    char position;
};

#endif
