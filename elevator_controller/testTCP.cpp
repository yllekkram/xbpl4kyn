#include <cstring>
#include <cstdlib>
#include <iostream>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "USAGE: main <gd_ip> <gd_port>" << std::endl;
		exit(1);
	}

	ElevatorController* ec = new ElevatorController();

	ec->connectToGD(argv[1], atoi(argv[2]));
	
	delete ec;
	
	std::cout << std::endl;

	exit(0);
}
