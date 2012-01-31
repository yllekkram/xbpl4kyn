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
		ElevatorController() {
			/* Create the TCP socket */
			if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
				Die("Failed to create socket");
			}
		}
		~ElevatorController() {};

		void connectToGD(char* gdAddress, int port) {
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

		int sock;
		struct sockaddr_in echoserver;
};

int main(int argc, char* argv[]) {
	ElevatorController* ec = new ElevatorController();

	char buffer[BUFFSIZE];
	unsigned int echolen;
	int received = 0;

	if (argc != 4) {
		fprintf(stderr, "USAGE: TCPecho <server_ip> <word> <port>\n");
		exit(1);
	}

	ec->connectToGD(argv[1], atoi(argv[3]));

	/* Send the word to the server */
	echolen = strlen(argv[2]);
	if (send(ec->sock, argv[2], echolen, 0) != echolen) {
		Die("Mismatch in number of sent bytes");
	}
	/* Receive the word back from the server */
	printf("Received: ");
	fflush(stdout);
	while (received < echolen) {
		int bytes = 0;
		if ((bytes = recv(ec->sock, buffer, BUFFSIZE-1, 0)) < 1) {
			Die("Failed to receive bytes from server");
		}
		received += bytes;
		buffer[bytes] = '\0';
		printf(buffer);
	}

	printf("\n");
	close(ec->sock);
	delete ec;
	exit(0);
}


