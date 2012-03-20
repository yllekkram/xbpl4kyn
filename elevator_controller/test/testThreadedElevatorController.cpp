#include <cstring>
#include <cstdlib>
#include <iostream>
#include <native/task.h>
#include <signal.h>
#include <sys/mman.h>

#include "ElevatorController.hpp"
#include "Exception.hpp"
#include "UDPView.hpp"

/* Constants */
#define NUM_ELEVATORS 5
/* End Constants */

/* Function Headers */
void catch_signal(int);
void runUDPThread(void*);
/* End Function Headers */

/* Data Declaration */
RT_TASK udpThread[NUM_ELEVATORS];

ElevatorController 	ec[NUM_ELEVATORS];
UDPView 						uv[NUM_ELEVATORS];
/* End Data Declaration */

int main(int argc, char* argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);

	signal(SIGINT, catch_signal);
	signal(SIGTERM, catch_signal);

	if (argc != 5) {
		std::cerr << "USAGE: main <gd_ip> <gd_port> <gui_ip> <gui_port>" << std::endl;
		exit(1);
	}

	/* Start each elevator controller */
	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_create(&udpThread[i], NULL, 0, 99, T_JOINABLE);
		uv[i].init(argv[3], argv[4]);
		ec[i].connectToGD(argv[1], atoi(argv[2]));
		try {
			ec[i].addView(&uv[i]);
		}
		catch (Exception e) {}

		rt_task_start(&udpThread[i], runUDPThread, &uv[i]);
	}

	/* Wait for all elevator controllers to finish */
	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_join(&udpThread[i]);
	}

	exit(0);
}

void catch_signal(int sig) {
	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_delete(&udpThread[i]);
	}
}

void runUDPThread(void* cookie) {
	printf("UDP Thread\n");
	UDPView* thisUV = (UDPView*)cookie;
	thisUV->run();
}
