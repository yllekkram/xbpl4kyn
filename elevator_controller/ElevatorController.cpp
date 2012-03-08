#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"

char ElevatorController::nextID = 1;

ElevatorController::ElevatorController() {
	this->id = ElevatorController::getNextID();
	
	/* Create the TCP socket */
	if ((this->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Failed to create socket");
	}
}

ElevatorController::~ElevatorController() {
	close(this->sock);
}

void ElevatorController::run() {
	while (true) {
		this->waitForGDRequest();
	}
}

void ElevatorController::addView(ElevatorControllerView* ecv) {
	this->views.push_back(ecv);
	ecv->setController(this);
	
}

void ElevatorController::waitForGDRequest() {
	char* request = receiveTCP(MAX_GD_REQUEST_SIZE);
	char requestType = request[0];
  Message* message = NULL;
	
	switch (requestType) {
		case STATUS_REQUEST:
      this->sendStatus();
			std::cout << "Status Request" << std::endl;
			break;
		case HALL_CALL_ASSIGNMENT:
      message = new HallCallAssignmentMessage(request);
			std::cout << "Hall Call Assigned: Floor" << (int) request[1];
			std::cout << std::endl;
			break;
		default:
			std::cout << "Unknown Message Type" << std::endl;
	}
}

void ElevatorController::sendStatus() {
	std::vector<char>* hallCalls = new std::vector<char>(); // = FloorRequestHeap.getHallCalls();

	this->sendMessage(StatusResponseMessage(this->id, 
                                          5, 6, 7, hallCalls->size(), 
                                          (char*) &hallCalls[0]));

	delete hallCalls;
}

void ElevatorController::connectToGD(char* gdAddress, int port) {
	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));
	echoserver.sin_family = AF_INET;
	echoserver.sin_addr.s_addr = inet_addr(gdAddress);
	echoserver.sin_port = htons(port);

	/* Establish connection */
	if (connect(this->sock,
				(struct sockaddr *) &(echoserver),
			sizeof(echoserver)) < 0) {
		Die("Failed to connect with server");
	}
	
	this->sendRegistration();
}

void receiveHallCall(HallCallAssignmentMessage& message) {
  std::cout << "Received hall call for floor " << (int) message.getFloor();
  std::cout << " in " << ((message.getDirection() == HALL_CALL_DIRECTION_DOWN) ? "downward" : "upward") << " direction" << std::endl;
}

void ElevatorController::sendRegistration() {
	sendMessage(RegisterWithGDMessage(this->id));
	
	receiveAck();
}

void ElevatorController::sendMessage(const Message& message) {
	this->sendMessage(message.getBuffer(), message.getLen());
}

void ElevatorController::sendMessage(const char * message, int len) {
	if (len == 0) {
		len = strlen(message);
	}
	/* Send the word to the server */
	if (send(this->sock, message, len, 0) != len) {
		Die("Mismatch in number of sent bytes");
	}
}

void ElevatorController::receiveAck() {
	char* message = this->receiveTCP(2);
	if (message[0] != REGISTRATION_ACK)
		Die("Registration not acknowledged");
}

char* ElevatorController::receiveTCP(unsigned int length) {
	char* buffer = new char[BUFFSIZE];
	unsigned int received = 0;

	/* Receive the word back from the server */
	std::cout << "Received: ";
	while (received < length) {
		int bytes = 0;
		if ((bytes = recv(sock, buffer, BUFFSIZE-1, 0)) < 1) {
			Die("Failed to receive bytes from server");
		}
		received += bytes;
		buffer[bytes] = '\0';
	}
	std::cout << buffer;
	return buffer;
}
