#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ElevatorController.hpp"

ElevatorController::ElevatorController() {
	/* Create the TCP socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Failed to create socket");
	}
}

ElevatorController::~ElevatorController() {
	close(sock);
}

void ElevatorController::connectToGD(char* gdAddress, int port) {
	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));
	echoserver.sin_family = AF_INET;
	echoserver.sin_addr.s_addr = inet_addr(gdAddress);
	echoserver.sin_port = htons(port);

	/* Establish connection */
	if (connect(sock,
				(struct sockaddr *) &(echoserver),
			sizeof(echoserver)) < 0) {
		Die("Failed to connect with server");
	}
}

void ElevatorController::sendMessage(char* message) {
	unsigned int echolen = strlen(message);

	/* Send the word to the server */
	if (send(sock, message, echolen, 0) != echolen) {
		Die("Mismatch in number of sent bytes");
	}
}

void ElevatorController::receiveMessage(unsigned int echolen) {
	char buffer[BUFFSIZE];
	int received = 0;

	/* Receive the word back from the server */
	printf("Received: ");
	fflush(stdout);
	while (received < echolen) {
		int bytes = 0;
		if ((bytes = recv(sock, buffer, BUFFSIZE-1, 0)) < 1) {
			Die("Failed to receive bytes from server");
		}
		received += bytes;
		buffer[bytes] = '\0';
		printf(buffer);
	}
}


int main(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "USAGE: TCPecho <server_ip> <word> <port>\n");
		exit(1);
	}

	ElevatorController* ec = new ElevatorController();

	ec->connectToGD(argv[1], atoi(argv[3]));
	ec->sendMessage(argv[2]);
	ec->receiveMessage(strlen(argv[2]));

	printf("\n");
	delete ec;
	exit(0);
}


