#ifndef UDP_VIEW_HPP
#define UDP_VIEW_HPP

#include <netinet/in.h>

#include "ElevatorControllerView.hpp"

class UDPView : public ElevatorControllerView {
	public:
		UDPView(char* guiAddress, char* guiPort);
		~UDPView();
		
		// Override superclass method
		virtual void setController(ElevatorController* ec);
		
		void receiveStatus(ElevatorControllerStatus* status);
		
		void sendMessage(char* message, int len=0);
		void receiveMessage(unsigned int len=0);
		
		void registerWithViewer();
		
	private:
		int sock;
		struct sockaddr_in server;
	
		void initUDP(char* address, char* port);
    void sendMessage(const Message& message);
};

#endif
