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
#define NUM_ELEVATORS 8
/* End Constants */

/* Function Headers */
void catch_signal(int);
void runECThread(void*);
void runUDPThread(void*);
void setupElevatorController(ElevatorController*, UDPView*, char*, char*, char*, char*);
/* End Function Headers */

/* Data Declaration */
RT_TASK ecThread[NUM_ELEVATORS];
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
		rt_task_create(&ecThread[i], 	NULL, 0, 99, T_JOINABLE);
		rt_task_create(&udpThread[i], NULL, 0, 99, T_JOINABLE);

		setupElevatorController(&ec[i], &uv[i], argv[1], argv[2], argv[3], argv[4]);

		rt_task_start(&ecThread[i],		runECThread, 	&ec[i]);
		rt_task_start(&udpThread[i], 	runUDPThread, &uv[i]);
	}

	/* Wait for all elevator controllers to finish */
	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_join(&ecThread[i]);
		rt_task_join(&udpThread[i]);
	}

	exit(0);
}

void catch_signal(int sig) {
	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_delete(&ecThread[i]);
		rt_task_delete(&udpThread[i]);
	}

	exit(1);
}

void runECThread(void* cookie) {
	printf("EC Thread\n");
	ElevatorController* thisEC = (ElevatorController*)cookie;
	thisEC->communicate();
}

void runUDPThread(void* cookie) {
	printf("UDP Thread\n");
	UDPView* thisUV = (UDPView*)cookie;
	thisUV->run();
}

void setupElevatorController(ElevatorController* thisEC, UDPView* thisUV, char* gdAddress, char* gdPort, char* guiAddress, char* guiPort) {
	thisUV->init(guiAddress, guiPort);
	thisEC->connectToGD(gdAddress, atoi(gdPort));

	try {
		thisEC->addView(thisUV);
	}
	catch (Exception e) {}
}
