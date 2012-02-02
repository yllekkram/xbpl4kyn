#include <cstring>
#include <cstdlib>
#include <iostream>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "USAGE: main <server_ip> <port>" << std::endl;
		exit(1);
	}

	ElevatorController* ec;
	
	for (int i = 0; i < 5; i++) {
		ec = new ElevatorController();
		ec->connectToGD(argv[1], atoi(argv[2]));
		delete ec;
		std::cout << std::endl;
	}
	exit(0);
}
