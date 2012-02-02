#include "ElevatorCommon.hpp"
#include "UDPView.hpp"

UDPView::UDPView(char* guiAddress, int guiPort) {
	this->initUDP(guiAddress, guiPort);
}

void UDPView::initUDP(char* guiAddress, int guiPort) {
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
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */
}

void UDPView::receiveEvent() {
	ssize_t nread;
	char buf[BUFFSIZE];
	
	nread = read(this->sfd, buf, BUFFSIZE);
	if (nread == -1) {
		Die("read");
	}
	
	std::cout << "Received event" << std::endl;
}
