#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <native/task.h>
#include <unistd.h>
#include <netinet/in.h>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"
#include "Exception.hpp"
#include "UDPView.hpp"

UDPView::UDPView() {
	rt_task_create(&(this->udpThread), NULL, 0, 99, T_JOINABLE);
}

UDPView::~UDPView() {
	rt_task_delete(&(this->udpThread));
	close(this->sock);
}

void UDPView::init(char* guiAddress, char* guiPort) {
	initUDP(guiAddress, guiPort);
}

void UDPView::receiveStatus(ElevatorControllerStatus* status) {
	printf("Received Status\n");
}

void UDPView::initUDP(char* address, char* port) {
  std::cout << "Initialising UDP...";
	if ((this->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Die("Failed to create socket");
	}
  std::cout << "done." << std::endl;
	
	/* Construct the server sockaddr_in structure */
	memset(&(this->server), 0, sizeof(this->server));		/* Clear struct */
	this->server.sin_family = AF_INET;									/* Internet/IP */
	this->server.sin_addr.s_addr = inet_addr(address);	/* IP Address */
	this->server.sin_port = htons(atoi(port));					/* Server Port */
}

void UDPView::setController(ElevatorController* ec) {
	ElevatorControllerView::setController(ec);
	
	this->registerWithViewer();
}

void UDPView::registerWithViewer() {
	printf("Register with viewer...");
	this->sendMessage(GUIRegistrationMessage(this->getEC()->getID()));
	printf("wait for ack...");
  this->receiveAck();
	printf("done\n");
}

void UDPView::run() {
	while (true) {
		try {
			waitForMessage();
		}
		catch (Exception e) {
			std::cout << e.what() << std::endl;
		}
	}
}

void UDPView::waitForMessage() {
	char* request = this->receiveMessage(MAX_GUI_REQUEST_SIZE);
	char requestType = request[0];

	std::cout << "EC" << (unsigned int)this->getEC()->getID() << ": Message: ";
	printBuffer(request, MAX_GUI_REQUEST_SIZE);
	std::cout << std::endl;

	std::cout <<  "EC" << (unsigned int)this->getEC()->getID() << ": Received ";
	switch (requestType) {
		case GUI_REGISTRATION_ACK:
			std::cout << "Reg Ack" << std::endl;
			break;
		case FLOOR_SELECTION_MESSAGE:
			std::cout << "floor selection: " << (int)request[1] << std::endl;
			break;
		case OPEN_DOOR_REQUEST:
			std::cout << "open door request" << std::endl;
			this->getEC()->openDoor();
			break;
		case CLOSE_DOOR_REQUEST:
			std::cout << "close door request" << std::endl;
			this->getEC()->closeDoor();
			break;
		case EMERGENGY_STOP_MESSAGE:
			std::cout << "emergency stop message" << std::endl;
			this->getEC()->emergencyStop();
			break;
		default:
			std::cout << "unknown message type: " << printBuffer(request, MAX_GUI_REQUEST_SIZE) << std::endl;
	}
}

void UDPView::sendMessage(const Message& message) {
  this->sendMessage(message.getBuffer(), message.getLen());
}

void UDPView::sendMessage(char* message, int len) {
	if (len == 0) {
		len = strlen(message);
	}
	
	if (sendto(this->sock, message, len, 0,
							(struct sockaddr *) &(this->server),
							sizeof(this->server)) != len) {
		Die("Mismatch in number of bytes sent");
	}
}

void UDPView::receiveAck() {
  this->receiveMessage(1);
}

// This will probably need to modify a global Message variable rather than return a new Message
char* UDPView::receiveMessage(unsigned int len) {
	char* buffer = new char[BUFFSIZE];
	struct sockaddr_in client;
	unsigned int clientlen = sizeof(client);
	int received = 0;
	
  std::cout <<  "EC" << (unsigned int)this->getEC()->getID() << ": Receiving UDP message...";
	received = recvfrom(this->sock, buffer, BUFFSIZE, 0,
														(struct sockaddr *) &client,
														&clientlen);
  std::cout << "done." << std::endl;
	
	/* Check that the client and server are using the same socket */
	if (this->server.sin_addr.s_addr != client.sin_addr.s_addr) {
		Die("Received a packet from an unexpected server");
	}
	
	return buffer;
}
