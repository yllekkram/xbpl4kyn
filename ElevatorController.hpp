#include <netinet/in.h>
#include <sys/socket.h>

class ElevatorController {
	public:
		ElevatorController();
		~ElevatorController();

		void connectToGD(char* gdAddress, int port);
		void sendMessage(char* message);
		void receiveMessage(unsigned int echolen);

	private:
		int sock;
		struct sockaddr_in echoserver;
};
