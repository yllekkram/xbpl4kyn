#ifndef UDP_VIEW_HPP
#define UDP_VIEW_HPP

#include <netinet/in.h>

#include "ElevatorControllerView.hpp"

class UDPView : public ElevatorControllerView {
	public:
		UDPView(){};
		~UDPView();
		
		void init(char* guiAddress, char* guiPort);
		// Override superclass method
		virtual void setController(ElevatorController* ec);
		
		void run();

		void waitForMessage();

		void receiveStatus(ElevatorControllerStatus* status);
		
		void sendMessage(char* message, int len=0);
		char* receiveMessage(unsigned int len=0);
		
		void registerWithViewer();
		
	private:
		int sock;
		struct sockaddr_in server;
	
		void initUDP(char* address, char* port);
    void sendMessage(const Message& message);
    void receiveAck();
};

#endif
