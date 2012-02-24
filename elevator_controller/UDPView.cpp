#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"
#include "UDPView.hpp"

UDPView::UDPView(char* guiAddress, char* guiPort) {
	initUDP(guiAddress, guiPort);
}

UDPView::~UDPView() {
	close(this->sock);
}

void UDPView::receiveStatus(ElevatorControllerStatus* status) {
	printf("Received Status\n");
}

void UDPView::initUDP(char* address, char* port) {
	if ((this->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Die("Failed to create socket");
	}
	
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
	char message[2];
	message[0] = this->getEC()->getID();
	message[1] = GUI_REGISTER_MESSAGE;
	
	this->sendMessage(message);
}

void UDPView::sendMessage(char* message, unsigned int len) {
	if (len == 0) {
		len = strlen(message);
	}
	
	if (sendto(this->sock, message, len, 0,
							(struct sockaddr *) &(this->server),
							sizeof(this->server)) != len) {
		Die("Mismatch in number of bytes sent");
	}
}

void UDPView::receiveMessage(unsigned int len) {
	char buffer[BUFFSIZE];
	struct sockaddr_in client;
	unsigned int clientlen = sizeof(client);
	int received = 0;
	
	if ((received = recvfrom(this->sock, buffer, BUFFSIZE, 0,
														(struct sockaddr *) &client,
														&clientlen)) != len) {
		Die("Mismatch in number of received bytes");
	}
	
	/* Check that the client and server are using the same socket */
	if (this->server.sin_addr.s_addr != client.sin_addr.s_addr) {
		Die("Received a packet from an unexpected server");
	}
	
	buffer[received] = '\0';
	printf("Received: %s\n", buffer);
}
