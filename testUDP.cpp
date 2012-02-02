#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "UDPView.hpp"

int main(int argc, char* argv[]) {	
	if (argc != 4) {
		fprintf(stderr, "USAGE: %s <server_ip> <port> <word>\n", argv[0]);
		exit(1);
	}
	
	UDPView* uv = new UDPView(argv[1], argv[2]);
	
	uv->sendMessage(argv[3]);
	uv->receiveMessage(strlen(argv[3]));
	
	delete uv;
	exit(0);
}
