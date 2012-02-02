#include <netinet/in.h>
#include <sys/socket.h>

class ElevatorController {
	public:
		ElevatorController();
		~ElevatorController();

		void connectToGD(char* gdAddress, int port);

	private:
		static char nextID;
		
		char id;
		int sock;
		struct sockaddr_in echoserver;
				
		static char getNextID() {
			char temp = nextID;
			nextID++;
			return temp;
		}
		
		void sendRegistration();
		void receiveAck();
		char* receiveTCP(unsigned int length);
};

