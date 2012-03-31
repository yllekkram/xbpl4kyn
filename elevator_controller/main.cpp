#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <math.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <native/cond.h>
#include <rtdk.h>

#include "ElevatorCommon.hpp"
#include "ElevatorController.hpp"
#include "ElevatorSimulator.hpp"
#include "Exception.hpp"
#include "Heap.hpp"
#include "UDPView.hpp"

/* Constants */
#define NUM_ELEVATORS 8
#define STANDARD_PAUSE 2500000000U
/* End Constants */

/* Funtion Prototypes */
void catch_signal(int);
void runECThread(void*);
void runFRThread(void*);
void runStatusThread(void*);
void runSupervisorThread(void*);
void runUDPThread(void*);
void setupElevatorController(int id, char*, char*, char*, char*);
void sleep(int);
	/* Functions for testing */
	void randomRun(void*);
	void runValues(void*);
	void supervisorStartUp(void*);
	/* End Funtions for testing */
/* End Function Prototypes */

/* Global Data Declarations */
unsigned char IDs[NUM_ELEVATORS]; // Store thread identifiers in global memory to ensure that they always exist

RT_TASK supervisorStart[NUM_ELEVATORS];
RT_TASK release_cond[NUM_ELEVATORS];
RT_TASK value_run[NUM_ELEVATORS];

ElevatorController	ec[NUM_ELEVATORS];
UDPView							uv[NUM_ELEVATORS];
ElevatorSimulator 	es[NUM_ELEVATORS];
/* End Global Data Declarations */

int main(int argc, char* argv[]) {
	rt_print_auto_init(1);

	/* Avoids memory swapping for this program */
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	if (argc != 5) {
		std::cerr << "USAGE: main <gd_ip> <gd_port> <gui_ip> <gui_port>" << std::endl;
		exit(1);
	}

	for (int i = 0; i < NUM_ELEVATORS; i++) {
		IDs[i] = i;

		rt_task_create(&supervisorStart[i],			NULL, 0, 99, T_JOINABLE);
		rt_task_create(&release_cond[i],					NULL, 0, 99, T_JOINABLE);
		rt_task_create(&value_run[i],						NULL, 0, 99, T_JOINABLE);

		setupElevatorController(IDs[i], argv[1], argv[2], argv[3], argv[4]);

		rt_task_start(&(ec[i].rtData.ecThread),					runECThread,					&IDs[i]);
		rt_task_start(&(ec[i].rtData.frThread), 				runFRThread, 						&IDs[i]);
		rt_task_start(&(ec[i].rtData.supervisorThread), runSupervisorThread, 	&IDs[i]);
		rt_task_start(&(ec[i].rtData.statusThread), 		runStatusThread, 						&IDs[i]);
		rt_task_start(&(uv[i].udpThread),								runUDPThread,					&IDs[i]);

		// rt_task_start(&supervisorStart[i],	supervisorStartUp,	&IDs[i]);
		// rt_task_start(&release_cond[i],			randomRun,			&IDs[i]);
		rt_task_start(&value_run[i],				runValues,					&IDs[i]);
	}

	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_join(&(ec[i].rtData.ecThread));
		rt_task_join(&(ec[i].rtData.frThread));
		rt_task_join(&(ec[i].rtData.statusThread));
		rt_task_join(&(ec[i].rtData.supervisorThread));
		rt_task_join(&(uv[i].udpThread));

		rt_task_join(&supervisorStart[i]);
		rt_task_join(&release_cond[i]);
		rt_task_join(&value_run[i]);
	}

	return 0;
}

void runECThread(void* cookie) {
	const int ID = *((int*)cookie);

	rt_printf("EC%d Thread\n", ec[ID].getID());
	ec[ID].communicate();
}

void runFRThread(void* cookie) {
	int ID = *((int*)cookie);

	rt_printf("FR%d Thread\n", ec[ID].getID());
	ec[ID].floorRun();
}

void runStatusThread(void* cookie) {
	const int ID = *((int*)cookie);

	rt_printf("ST%d Thread\n", ec[ID].getID());
	ec[ID].updateStatus();
}

void runSupervisorThread(void* cookie) {
	const int ID = *((int*)cookie);

	rt_printf("SV%d Thread\n", ec[ID].getID());;
	ec[ID].supervise();
}

void runUDPThread(void* cookie) {
	const int ID = *((int*)cookie);

	rt_printf("UDP%d Thread\n", ec[ID].getID());
	uv[ID].run();
}

void setupElevatorController(const int ID, char* gdAddress, char* gdPort, char* guiAddress, char* guiPort) {
	uv[ID].init(guiAddress, guiPort);
	ec[ID].connectToGD(gdAddress, atoi(gdPort));

	try {
		ec[ID].addView(&uv[ID]);
	}
	catch (Exception e) {}

	ec[ID].addSimulator(&es[ID]);
}

void sleep(int numTimes)
{
	for(int i=0; i<numTimes; i++)
	{
		rt_task_sleep(250000000);
	}
}

// this function is just for my testing purposes
void randomRun(void *arg)
{
	const int ID = *((int*)arg);
	ec[ID].getUpHeap().pushHallCall(5);
	rt_printf("RR%d Hall Call Up Floor : 5\n", ID);
	sleep(40);

	ec[ID].getUpHeap().pushHallCall(3);
	rt_printf("RR%d Hall Call Up Floor : 3\n", ID);
	sleep(40);

	ec[ID].getUpHeap().pushFloorRequest(4);
	rt_printf("RR%d Floor Request Up : 4\n", ID);
	sleep(40);

	ec[ID].getUpHeap().pushFloorRequest(9);
	rt_printf("RR%d Floor Request Up : 9\n", ID);
	sleep(40);

	ec[ID].getDownHeap().pushHallCall(6);
	rt_printf("RR%d Hall Call Down Floor : 6\n", ID);
	sleep(40);

	ec[ID].getDownHeap().pushHallCall(2);
	rt_printf("RR%d Hall Call Down Floor : 2\n", ID);
	sleep(40);

	ec[ID].getDownHeap().pushFloorRequest(3);
	rt_printf("RR%d Floor Request Down : 3\n", ID);
	sleep(40);

	ec[ID].getDownHeap().pushFloorRequest(0);
	rt_printf("RR%d Floor Request Down : 0\n", ID);
	sleep(40);
}

// this function is just for my testing purposes
void runValues(void *arg)
{
	const int ID = *((int*)arg);

	while(true)
	{
		usleep(1000000);
		ec[ID].getSimulator()->calculateValues();
		rt_printf("RV%d ", ec[ID].getID());
		ec[ID].getSimulator()->print();
	}
}

// this function is just for my testing purposes
void supervisorStartUp(void *arg)
{
	const int ID = *((int*)arg);

	sleep(200);
	rt_printf("SSU%d GroupDispatcher FAILED, Oh my god :(\n", ID);
	ec[ID].eStat.GDFailed = true;
	sleep(400);
	rt_printf("SSU%d GroupDispatcher FIXED, good :)\n", ID);
	ec[ID].eStat.GDFailed = false;

	ec[ID].getUpHeap().pushHallCall(8);
	rt_printf("SSU%d Hall Call Up Floor : 3\n", ID);
	sleep(40);

	ec[ID].getUpHeap().pushFloorRequest(15);
	rt_printf("SSU%d Floor Request Up : 4\n", ID);
}

void catch_signal(int sig) {
	exit(1);
}
