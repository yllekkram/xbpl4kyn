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
void floorRun(void*);
bool relseaseFreeCond();
void runECThread(void*);
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
unsigned char IDs[NUM_ELEVATORS]; // Store thread identifiers in gloabl memory to ensure that they always exist

RT_TASK supervisorStart[NUM_ELEVATORS];
RT_TASK release_cond[NUM_ELEVATORS];
RT_TASK value_run[NUM_ELEVATORS];

ElevatorController	ec[NUM_ELEVATORS];
UDPView							uv[NUM_ELEVATORS];
ElevatorSimulator 	es[NUM_ELEVATORS];
/* End Global Data Declarations */

int main(int argc, char* argv[]) {
	/* Avoids memory swapping for this program */
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	for (int i = 0; i < NUM_ELEVATORS; i++) {
		IDs[i] = i;

		rt_task_create(&supervisorStart[i],			NULL, 0, 99, T_JOINABLE);
		rt_task_create(&release_cond[i],					NULL, 0, 99, T_JOINABLE);
		rt_task_create(&value_run[i],						NULL, 0, 99, T_JOINABLE);

		setupElevatorController(IDs[i], "192.168.251.1", "5000", "192.168.251.1", "5003");

		rt_task_start(&(ec[i].rtData.ecThread),					runECThread,					&IDs[i]);
		rt_task_start(&(ec[i].rtData.frThread), 				floorRun, 						&IDs[i]);
		rt_task_start(&(ec[i].rtData.supervisorThread), runSupervisorThread, 	&IDs[i]);
		rt_task_start(&(ec[i].rtData.statusThread), 		runStatusThread, 						&IDs[i]);
		rt_task_start(&(uv[i].udpThread),								runUDPThread,					&IDs[i]);

		rt_task_start(&supervisorStart[i],	supervisorStartUp,	&IDs[i]);
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

	printf("EC %d Thread\n", ec[ID].getID());
	ec[ID].communicate();
}

void runStatusThread(void* cookie) {
	const int ID = *((int*)cookie);

	printf("Stat%d Thread", ec[ID].getID());
	ec[ID].updateStatus();
}

void runSupervisorThread(void* cookie) {
	const int ID = *((int*)cookie);

	printf("SV%d Thread\n", ec[ID].getID());;
	ec[ID].supervise();
}

void runUDPThread(void* cookie) {
	const int ID = *((int*)cookie);

	printf("UDP%d Thread\n", ec[ID].getID());
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

void floorRun(void *arg)
{
	const int ID = *((int*)arg);
	unsigned char topItem;
	while(true)
	{
		if(!ec[ID].eStat.GDFailedEmptyHeap)
		{
			rt_mutex_acquire(&(ec[ID].rtData.mutex), TM_INFINITE);
			rt_cond_wait(&(ec[ID].rtData.freeCond), &(ec[ID].rtData.mutex), TM_INFINITE);

			int upHeapSize = ec[ID].getUpHeap().getSize();
			int downHeapSize = ec[ID].getDownHeap().getSize();
			if(upHeapSize==0 && downHeapSize==0){ec[ID].eStat.GDFailedEmptyHeap = true;}

			if(ec[ID].eStat.downDirection)
			{
				if(downHeapSize > 0)
				{
					topItem = ec[ID].getDownHeap().peek();
				}else if(upHeapSize > 0)
				{
					topItem = ec[ID].getUpHeap().peek();
				}
			}else
			{
				if(upHeapSize > 0)
				{
					topItem = ec[ID].getUpHeap().peek();
				}else if(downHeapSize > 0)
				{
					topItem = ec[ID].getDownHeap().peek();
				}
			}
		
			if(topItem != ec[ID].eStat.destination)
			{
				printf("FR%d next Dest is %d\n.", ID, topItem);
				ec[ID].getSimulator()->setFinalDestination(topItem);
				ec[ID].eStat.destination = topItem;
			}
			rt_mutex_release(&(ec[ID].rtData.mutex));
		}
	}
}

bool releaseFreeCond(const int ID)
{
	int heapSize = ec[ID].getUpHeap().getSize() + ec[ID].getDownHeap().getSize();
	if(heapSize > 0)
	{
		ec[ID].eStat.GDFailedEmptyHeap = false;
		rt_cond_signal(&(ec[ID].rtData.freeCond));
		return true;
	}else
	{
		return false;
	}
}

void sleep(int numTimes)
{
	for(int i=0; i<numTimes; i++)
	{
		rt_task_sleep(250000000);
	}
}

void updateStatusBuffer(const int ID)
{
	//upheap and the downheap (hallcall and floor selections should be included.
	rt_mutex_acquire(&(ec[ID].rtData.mutexBuffer), TM_INFINITE);
	bool selectedBuffer = ec[ID].eStat.bufferSelection;
	rt_mutex_release(&(ec[ID].rtData.mutexBuffer));
	
	rt_mutex_acquire(&(ec[ID].rtData.mutex), TM_INFINITE);
	ec[ID].eStat.statusBuffer[selectedBuffer][0] = ec[ID].eStat.currentFloor;
	ec[ID].eStat.statusBuffer[selectedBuffer][1] = ec[ID].eStat.direction;
	ec[ID].eStat.statusBuffer[selectedBuffer][2] = ec[ID].eStat.currentPosition;
	ec[ID].eStat.statusBuffer[selectedBuffer][3] = ec[ID].eStat.currentSpeed;
	//printf("writting to buffer %d %s\n", selectedBuffer, statusBuffer[selectedBuffer]);
	rt_mutex_release(&(ec[ID].rtData.mutex));

	rt_mutex_acquire(&(ec[ID].rtData.mutexBuffer), TM_INFINITE);
	ec[ID].eStat.bufferSelection = ++(ec[ID].eStat.bufferSelection) % 2;
	rt_mutex_release(&(ec[ID].rtData.mutexBuffer));
}

// this function is just for my testing purposes
void randomRun(void *arg)
{
	const int ID = *((int*)arg);
	ec[ID].getUpHeap().pushHallCall(5);
	rt_mutex_acquire(&(ec[ID].rtData.mutex), TM_INFINITE);
	releaseFreeCond(ID);
	rt_mutex_release(&(ec[ID].rtData.mutex));
	printf("RR%d Hall Call Up Floor : 5\n", ID);
	sleep(40);

	ec[ID].getUpHeap().pushHallCall(3);
	printf("RR%d Hall Call Up Floor : 3\n", ID);
	sleep(40);

	ec[ID].getUpHeap().pushFloorRequest(4);
	printf("RR%d Floor Request Up : 4\n", ID);

	ec[ID].getUpHeap().pushFloorRequest(9);
	printf("RR%d Floor Request Up : 9\n", ID);
	sleep(40);

	ec[ID].getDownHeap().pushHallCall(6);
	printf("RR%d Hall Call Down Floor : 6\n", ID);
	sleep(40);

	ec[ID].getDownHeap().pushHallCall(2);
	printf("RR%d Hall Call Down Floor : 2\n", ID);
	sleep(40);

	ec[ID].getDownHeap().pushFloorRequest(3);
	printf("RR%d Floor Request Down : 3\n", ID);
	sleep(40);

	ec[ID].getDownHeap().pushFloorRequest(0);
	printf("RR%d Floor Request Down : 0\n", ID);
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
		std::cout << "RV" << ID << " ";
		ec[ID].getSimulator()->print();
	}
}

// this function is just for my testing purposes
void supervisorStartUp(void *arg)
{
	const int ID = *((int*)arg);

	sleep(200);
	printf("SSU%d GroupDispatcher FAILED, Oh my god :(\n", ID);
	ec[ID].eStat.GDFailed = true;
	sleep(400);
	printf("SSU%d GroupDispatcher FIXED, good :)\n", ID);
	ec[ID].eStat.GDFailed = false;

	ec[ID].getUpHeap().pushHallCall(8);
	printf("SSU%d Hall Call Up Floor : 3\n", ID);
	sleep(40);

	ec[ID].getUpHeap().pushFloorRequest(15);
	printf("SSU%d Floor Request Up : 4\n", ID);
}

void catch_signal(int sig) {
	exit(1);
}
