#include <cstring>
#include <cstdlib>
#include <iostream>
#include <native/task.h>
#include <signal.h>
#include <sys/mman.h>

#include "ElevatorController.hpp"
#include "Exception.hpp"
#include "UDPView.hpp"

/* Function Headers */
void catch_signal(int);
void runUDPThread(void*);
/* End Function Headers */

/* Data Declaration */
RT_TASK udpThread;

ElevatorController ec;
UDPView uv;
/* End Data Declaration */

int main(int argc, char* argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);

	signal(SIGINT, catch_signal);
	signal(SIGTERM, catch_signal);

	if (argc != 5) {
		std::cerr << "USAGE: main <gd_ip> <gd_port> <gui_ip> <gui_port>" << std::endl;
		exit(1);
	}

	rt_task_create(&udpThread, NULL, 0, 99, T_JOINABLE);

	uv.init(argv[3], argv[4]);

	ec.connectToGD(argv[1], atoi(argv[2]));
  try {
    ec.addView(&uv);
  }
  catch (Exception e) {}

	rt_task_start(&udpThread, &runUDPThread, &uv);

	rt_task_join(&udpThread);
	
	exit(0);
}

void catch_signal(int sig) {
	rt_task_delete(&udpThread);
}

void runUDPThread(void* cookie) {
	printf("UDP Thread\n");
	UDPView* thisUV = (UDPView*)cookie;
	thisUV->run();
}
