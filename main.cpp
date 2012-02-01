#include <cstring>
#include <cstdlib>
#include <iostream>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"

int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "USAGE: main <server_ip> <port> <word>" << std::endl;
		exit(1);
	}

	ElevatorController* ec = new ElevatorController();

	ec->connectToGD(argv[1], atoi(argv[2]));
	ec->sendMessage(argv[3]);
	ec->receiveMessage(strlen(argv[2]));

	std::cout << std::endl;
	delete ec;
	exit(0);
}
