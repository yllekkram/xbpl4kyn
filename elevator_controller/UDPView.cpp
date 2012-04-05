#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <native/task.h>
#include <netinet/in.h>
#include <rtdk.h>
#include <sys/socket.h>
#include <unistd.h>

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
	rt_printf("Received Status\n");
}

void UDPView::initUDP(char* address, char* port) {
	rt_printf("Initialising UDP...");
	if ((this->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Die("Failed to create socket");
	}
	rt_printf("done.\n");
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
	rt_printf("Register with viewer...");
	this->sendMessage(GUIRegistrationMessage(this->getEC()->getID()));
	rt_printf("wait for ack...");
  this->receiveAck();
	rt_printf("done\n");
}

void UDPView::run() {
	while (true) {
		try {
			waitForMessage();
		}
		catch (Exception e) {
			rt_printf("%s\n", e.what());
		}
	}
}

void UDPView::waitForMessage() {
	char* request = this->receiveMessage(MAX_GUI_REQUEST_SIZE);
	char requestType = request[0];

	rt_printf("EC%d: Message: ", (unsigned int)this->getEC()->getID());
	printBuffer(request, MAX_GUI_REQUEST_SIZE);
	rt_printf("\n");

	rt_printf("EC%d: Received ", (unsigned int)this->getEC()->getID());
	switch (requestType) {
		case GUI_REGISTRATION_ACK:
			rt_printf("Reg Ack\n");
			break;
		case FLOOR_SELECTION_MESSAGE:
			rt_printf("floor selection: %d\n", (unsigned int)request[1]);
			this->getEC()->addFloorSelection(request[1]);
			break;
		case OPEN_DOOR_REQUEST:
			rt_printf("open door request\n");
			this->getEC()->openDoor();
			break;
		case CLOSE_DOOR_REQUEST:
			rt_printf("close door request\n");
			this->getEC()->closeDoor();
			break;
		case EMERGENGY_STOP_MESSAGE:
			rt_printf("emergency stop message\n");
			this->getEC()->emergencyStop();
			break;
		default:
			rt_printf("unknown message type: ");
			printBuffer(request, MAX_GUI_REQUEST_SIZE);
			rt_printf("\n");
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
	
	rt_printf("EC%d: Receiving UDPMessage...", (unsigned int)this->getEC()->getID());
	received = recvfrom(this->sock, buffer, BUFFSIZE, 0,
														(struct sockaddr *) &client,
														&clientlen);
	rt_printf("done.\n");
	
	/* Check that the client and server are using the same socket */
	if (this->server.sin_addr.s_addr != client.sin_addr.s_addr) {
		Die("Received a packet from an unexpected server");
	}
	
	return buffer;
}

void UDPView::notifyFloorReached(unsigned char floor){	
	int len = 3;	
	char message[len];
	message[0] = FLOOR_REACHED_NOTIFICATION;
	message[1] = (unsigned int)this->getEC()->getID();
	message[2] = floor;
	this->sendMessage(message, len);
}

void UDPView::notifyDirectionChanged(unsigned char direction){
	int len = 3;	
	char message[len];
	message[0] = NEW_DIRECTION_NOTIFICATION;
	message[1] = (unsigned int)this->getEC()->getID();
	message[2] = direction;
	this->sendMessage(message, len);
}
