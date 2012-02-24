#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include "ElevatorCommon.hpp"

void HandleClient(int sock) {
	char buffer[BUFFSIZE];
	int received = -1;
	/* Receive message */
	if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
		Die("Failed to receive initial bytes from client");
	}
	/* Send bytes and check for more incoming data in loop */
	while (received > 0) {
		switch(buffer[1]) {
			case 0:
				std::cout << "Register " << (int) buffer[0] << std::endl;
				buffer[0] = '0';
				buffer[1] = '\0';
				break;
			case 1:
				std::cout << "Status" << std::endl;
				buffer[0] = '1';
				buffer[1] = '\0';
				break;
		}
		
		/* Send back received data */
		if (send(sock, buffer, received, 0) != received) {
			Die("Failed to send bytes to client");
		}
		/* Check for more data */
		if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
			Die("Failed to receive additional bytes from client");
		}
	}
	close(sock);
}

int main(int argc, char* argv[]) {
	int serversock, clientsock;
	struct sockaddr_in echoserver, echoclient;

	if (argc != 2) {
		fprintf(stderr, "USAGE: echoserver <port>\n");
		exit(1);
	}

	/* Create the TCP socket */
	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Failed to create socket");
	}

	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));			/* Clear struct */
	echoserver.sin_family = AF_INET;								/* Internet/IP */
	echoserver.sin_addr.s_addr = htonl(INADDR_ANY);	/* Incoming addr */
	echoserver.sin_port = htons(atoi(argv[1]));			/* server port */

	/* Bind the server socket */
	if (bind(serversock, (struct sockaddr *) &echoserver,
				sizeof(echoserver)) < 0) {
		Die("Failed to bind the server socket");
	}
	/* Listen on the server socket */
	if (listen(serversock, MAXPENDING) < 0) {
		Die("Failed to listen on server socket");
	}

	/* Run until cancelled */
	while (1) {
		unsigned int clientlen = sizeof(echoclient);
		/* Wait for client connection */
		if ((clientsock =
					accept(serversock, (struct sockaddr *) &echoclient,
						&clientlen)) < 0) {
			Die("Failed to accept client connection");
		}
		std::cout << "Client connected: " << inet_ntoa(echoclient.sin_addr) << std::endl;
		HandleClient(clientsock);
	}
}
