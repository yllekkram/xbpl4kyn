#ifndef UDP_VIEW_HPP
#define UDP_VIEW_HPP

#include <netinet/in.h>

#include "ElevatorControllerView.hpp"

class UDPView : public ElevatorControllerView {
	public:
		UDPView(char* guiAddress, char* guiPort);
		~UDPView();
		
		void receiveStatus(ElevatorControllerStatus* status);
		
		void sendMessage(char* message, unsigned int len=0);
		void receiveMessage(unsigned int len=0);
		
		void registerWithViewer();
		
	private:
		int sock;
		struct sockaddr_in server;
	
		void initUDP(char* address, char* port);
};

#endif
