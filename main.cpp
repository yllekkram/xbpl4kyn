#include <cstring>

#include "ElevatorController.hpp"

int main(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "USAGE: main <server_ip> <port> <word>\n");
		exit(1);
	}

	ElevatorController* ec = new ElevatorController();

	ec->connectToGD(argv[1], atoi(argv[2]));
	ec->sendMessage(argv[3]);
	ec->receiveMessage(strlen(argv[2]));

	printf("\n");
	delete ec;
	exit(0);
}
