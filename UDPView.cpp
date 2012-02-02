#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"
#include "UDPView.hpp"

UDPView::UDPView(char* guiAddress, char* guiPort) {
	initUDP(guiAddress, guiPort);
}

void UDPView::registerWithGUI() {
	char message[2] = {ec->getID(),GUI_REGISTER_MESSAGE};
	sendMessage(message, 2);
}

void UDPView::initUDP(char* guiAddress, char* guiPort) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */
	
	s = getaddrinfo(guiAddress, guiPort, &hints, &result);
	if (s != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(s) << std::endl;
		exit(EXIT_FAILURE);
	}
	
	/* Try connecting */
	for (rp = result; rp != NULL; rp = rp->ai_next) {
        this->sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        
        if (this->sfd == -1)
            continue;

        if (connect(this->sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

        close(this->sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        std::cerr << "Could not connect" << std::endl;
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */
}

void UDPView::receiveStatus(ElevatorControllerStatus* status) {
}

void UDPView::receiveEvent() {
	ssize_t nread;
	char buf[BUFFSIZE];
	
	nread = read(this->sfd, buf, BUFFSIZE);
	if (nread == -1) {
		Die("read");
	}
	
	std::cout << "Received event" << std::endl;
	sendMessage("Received event\n");
}

void UDPView::sendMessage(char * message, size_t len) {
	if (len=0) {
		len = strlen(message);
	}
	
	if (write(this->sfd, message, len) != len) {
		Die("partial/failed write");
	}
}
