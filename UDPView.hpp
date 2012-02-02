#ifndef UDPVIEW_HPP
#define UDP_VIEW_HPP

#include "ElevatorControllerView.hpp"

class UDPView : public ElevatorControllerView {
	public:
		UDPView(char* guiAddress, int guiPort);
	
		void receiveStatus(ElevatorControllerStatus* status);
		void receiveEvent();
		
	private:
		int sfd;
		
		void initUDP(char* guiAddress, int guiPort);
};

#endif
