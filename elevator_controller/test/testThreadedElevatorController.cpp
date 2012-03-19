#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/mman.h>

#include "ElevatorController.hpp"
#include "Exception.hpp"
#include "UDPView.hpp"

ElevatorController ec;
UDPView uv;

int main(int argc, char* argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);

	if (argc != 5) {
		std::cerr << "USAGE: main <gd_ip> <gd_port> <gui_ip> <gui_port>" << std::endl;
		exit(1);
	}

	uv.init(argv[3], argv[4]);

	ec.connectToGD(argv[1], atoi(argv[2]));
  try {
    ec.addView(&uv);
  }
  catch (Exception e) {}
	
	uv.run();

	std::cout << std::endl;

	exit(0);
}
