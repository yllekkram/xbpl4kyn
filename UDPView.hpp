#ifndef UDP_VIEW_HPP
#define UDP_VIEW_HPP

#include "ElevatorControllerView.hpp"

class UDPView : public ElevatorControllerView {
	public:
		UDPView(char* guiAddress, char* guiPort);
	
		void receiveStatus(ElevatorControllerStatus* status);
		void receiveEvent();
		void registerWithGUI();
		
	private:
		int sfd;
		
		void sendMessage(char* message, size_t len=0);
		void initUDP(char* guiAddress, char* guiPort);
};

#endif
