#ifndef ELEVATOR_CONTROLLER_HPP
#define ELEVATOR_CONTROLLER_HPP

#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>

// Common Values
#define BUFFSIZE 32

// Types for messages received from the GroupDispatcher
#define STATUS_REQUEST 1

// Types for messages sent from the ElevatorController to the GroupDispatcher
#define REGISTER_MESSAGE 0

#define STATUS_RESPONSE 1
#define ERROR_RESPONSE 2

// Common functions
void Die(std::string);

class ElevatorController {
	public:
		ElevatorController();
		~ElevatorController();

		void connectToGD(char* gdAddress, int port);
		void sendMessage(char* message);
		void receiveMessage(unsigned int echolen);

	private:
		int sock;
		struct sockaddr_in echoserver;
};

#endif
