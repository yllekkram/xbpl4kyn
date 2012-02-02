#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

#include "ElevatorCommon.hpp"

int main(int argc, char* argv[]) {
	int sock;
	struct sockaddr_in echoserver;
	struct sockaddr_in echoclient;
	char buffer[BUFFSIZE];
	unsigned int echolen, clientlen;
	int received = 0;
	
	if (argc != 4) {
		fprintf(stderr, "USAGE: %s <server_ip> <port> <word>\n", argv[0]);
		exit(1);
	}
	
	/* Create UDP socket */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Die("Failed to create socket");
	}
	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));				/* Clear struct */
	echoserver.sin_family = AF_INET;									/* Clear struct */
	echoserver.sin_addr.s_addr = inet_addr(argv[1]);	/* Ineternet IP */
	echoserver.sin_port = htons(atoi(argv[3]));				/* server port */
	
	/* Send the word to the server */
	echolen = strlen(argv[2]);
	if (sendto(sock, argv[2], echolen, 0,
						(struct sockaddr *) &echoserver,
						sizeof(echoserver)) != echolen) {
		Die("Mismatch in number of bytes sent");
	}
	
	/* Receive the word back from the server */
	clientlen = sizeof(echoclient);
	if ((received = recvfrom(sock, buffer, BUFFSIZE, 0,
														(struct sockaddr *) &echoclient,
														&clientlen)) != echolen) {
		Die("Mismatch in number of received bytes");
	}
	
	/* Check that the client and server are using the same socket */
	if (echoserver.sin_addr.s_addr != echoclient.sin_addr.s_addr) {
		Die("Received a packet from an unexpected server");
	}
	
	buffer[received] = '\0';
	printf("Received: %s\n", buffer);
	
	close(sock);
	exit(0);
}
