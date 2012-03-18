#include <csignal>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <native/task.h>
#include <sys/mman.h>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"
#include "Exception.hpp"
#include "UDPView.hpp"

struct ECRunData {
	ElevatorController* ec;
	char* gdAddress;
	int gdPort;
} ecRunData;

void runEC(void* cookie) {
	ECRunData* ecrd = (ECRunData*)cookie;
	ecrd->ec->connectToGD(ecrd->gdAddress, ecrd->gdPort);
	ecrd->ec->run();
}

/* Variable Declarations */
RT_TASK ecTask;

ElevatorController ec;
ECRunData ecrd;
/* End Variable Declarations*/

void catch_signal(int sig) {
	rt_task_delete(&ecTask);
}

int main(int argc, char* argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);

	signal(SIGINT, catch_signal);
	signal(SIGTERM, catch_signal);

	if (argc != 5) {
		std::cerr << "USAGE: main <gd_ip> <gd_port> <gui_ip> <gui_port>" << std::endl;
		exit(1);
	}

	rt_task_create(&ecTask, NULL, 0, 99, 0);

	ecrd.ec = &ec;
	ecrd.gdAddress = argv[1];
	ecrd.gdPort = atoi(argv[2]);

	rt_task_start(&ecTask, &runEC, &ecRunData);

	// UDPView* uv = new UDPView(argv[3], argv[4]);

  // try {
  //   ec.addView(uv);
  // }
  // catch (Exception e) {}
	
	std::cout << std::endl;

	exit(0);
}
