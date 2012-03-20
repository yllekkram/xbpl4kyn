#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ElevatorController.hpp"
#include "UDPView.hpp"

int main(int argc, char* argv[]) {	
	if (argc != 3) {
		fprintf(stderr, "USAGE: %s <server_ip> <port>\n", argv[0]);
		exit(1);
	}
	
	ElevatorController ec;
	UDPView uv;
	uv.init(argv[1], argv[2]);
	
	ec.addView(&uv);
	
	uv.registerWithViewer();
	//~ uv->receiveMessage(strlen(argv[3]));
	
	exit(0);
}
