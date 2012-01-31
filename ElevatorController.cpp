#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ElevatorController.h"

class ElevatorController {
	public:
		ElevatorController();
		~ElevatorController();

		int registerWithGD(char* gdAddress, int gdPort);

	private:
		int gdSock;
		struct sockaddr_in gdSockAddr;
		unsigned int gdSockAddrLen;
};

ElevatorController::ElevatorController() {
	this->gdSockAddrLen = sizeof(this->gdSockAddr);

	// Clear the struct
	memset(&(this->gdSockAddr), 0, this->gdSockAddrLen);
}

ElevatorController::~ElevatorController() {
	close(gdSock);
}

int ElevatorController::registerWithGD(char* gdAddress, int gdPort) {
	int tempSock;
	struct sockaddr_in tempSockAddr, gdSockAddr;

	if ((tempSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Failed to create socket");
	}

	/* Construct the a temporary sockaddr_in structure to initiate communication*/
	memset(&tempSockAddr, 0, sizeof(tempSockAddr)); /* Clear struct */
	tempSockAddr.sin_family = AF_INET; 						/* Internet/IP */
	tempSockAddr.sin_addr.s_addr = inet_addr(gdAddress); /* IP address */
	tempSockAddr.sin_port = htons(gdPort);				/* Server port */

	/* Establish connection */
	if (connect(tempSock,
				(struct sockaddr *) &tempSockAddr,
			sizeof(tempSockAddr)) < 0) {
		Die("Failed to connect with GroupDispatcher");
	}

	/* Send the word to the server */
	char request[1];
	request[0] = REGISTER_MESSAGE;

	if (send(tempSock, request, 1, 0) != 1) {
		Die("Mismatch in number of sent bytes");
	}

	/* Save the socket that the GroupDispatcher is using to communicate */
	if (this->gdSock =
			accept(tempSock, (struct sockaddr *) &(this->gdSockAddr),
				&(this->gdSockAddrLen)) < 0) {
		Die("Failed to accept GroupDispatcher connection");
	}

	close(tempSock);
}
