#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <math.h>
#include <sys/mman.h>
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
#define NUM_ELEVATORS 1
#define STANDARD_PAUSE 2500000000U
/* End Constants */

/* Funtion Prototypes */
void catch_signal(int);
void floorRun(void*);
bool relseaseFreeCond();
void runECThread(void*);
void runUDPThread(void*);
void setupElevatorController(ElevatorController*, UDPView*, char*, char*, char*, char*);
void sleep(int);
void statusRun(void*);
void supervisorRun(void*);
	/* Functions for testing */
	void randomRun(void*);
	void runValues(void*);
	void supervisorStartUp(void*);
	/* End Funtions for testing */
/* End Function Prototypes */

/* Global Data Declarations */
int IDs[NUM_ELEVATORS]; // Store thread identifiers in gloabl memory to ensure that they always exist

RT_MUTEX mutex[NUM_ELEVATORS];
RT_MUTEX mutexBuffer[NUM_ELEVATORS];
RT_COND freeCond[NUM_ELEVATORS];

RT_TASK ecThread[NUM_ELEVATORS];
RT_TASK frThread[NUM_ELEVATORS];
RT_TASK udpThread[NUM_ELEVATORS];
RT_TASK statusThread[NUM_ELEVATORS];
RT_TASK supervisorThread[NUM_ELEVATORS];

RT_TASK supervisorStart[NUM_ELEVATORS];
RT_TASK release_cond[NUM_ELEVATORS];
RT_TASK value_run[NUM_ELEVATORS];

ElevatorController	ec[NUM_ELEVATORS];
UDPView							uv[NUM_ELEVATORS];
ElevatorSimulator 	es[NUM_ELEVATORS];

//status updated every 75ms by the status thread
char currentFloor[NUM_ELEVATORS];
char direction[NUM_ELEVATORS];
char currentPosition[NUM_ELEVATORS];
char currentSpeed[NUM_ELEVATORS];

//will be used by the other threads.
int destination[NUM_ELEVATORS];
int taskAssigned[NUM_ELEVATORS];
bool upDirection[NUM_ELEVATORS];
bool downDirection[NUM_ELEVATORS];
bool GDFailed[NUM_ELEVATORS];
bool GDFailedEmptyHeap[NUM_ELEVATORS];

//double buffer used by both communication and status threads.
unsigned char statusBuffer[NUM_ELEVATORS][2][BUFFSIZE];
unsigned char bufferSelection[NUM_ELEVATORS];
/* End Global Data Declarations */

int main(int argc, char* argv[]) {
	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);

	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	for (int i = 0; i < NUM_ELEVATORS; i++) {
		IDs[i] = i;

		rt_mutex_create(&mutex[i], 				NULL);
		rt_mutex_create(&mutexBuffer[i], 	NULL);

		rt_cond_create(&freeCond[i], NULL);

		rt_task_create(&ecThread[i], 					NULL, 0, 99, T_JOINABLE);
		rt_task_create(&frThread[i], 					NULL, 0, 99, T_JOINABLE);
		rt_task_create(&statusThread[i], 			NULL, 0, 99, T_JOINABLE);
		rt_task_create(&supervisorThread[i], 	NULL, 0, 99, T_JOINABLE);
		rt_task_create(&udpThread[i], 				NULL, 0, 99, T_JOINABLE);

		rt_task_create(&supervisorStart[i],			NULL, 0, 99, T_JOINABLE);
		rt_task_create(&release_cond[i],					NULL, 0, 99, T_JOINABLE);
		rt_task_create(&value_run[i],						NULL, 0, 99, T_JOINABLE);

		currentFloor[i] = 0;
		currentPosition[i] = 0;
		currentSpeed[i] = 0;
		destination[i] = 0;
		direction[i] = 0;
		downDirection[i] = false;
		GDFailed[i] = false;
		GDFailedEmptyHeap[i] = false;
		upDirection[i] = false;
		taskAssigned[i] = false;
		bufferSelection[i] = 0;

		// setupElevatorController(&ec[i], &uv[i], "192.168.251.1", "5000", "192.168.251.1", "5003");

		// rt_task_start(&ecThread[i],					runECThread,		&IDs[i]);
		// rt_task_start(&udpThread[i],				runUDPThread,		&IDs[i]);
		rt_task_start(&frThread[i], 				floorRun, 			&IDs[i]);
		rt_task_start(&supervisorThread[i], supervisorRun, 	&IDs[i]);
		rt_task_start(&statusThread[i], 		statusRun, 			&IDs[i]);

		rt_task_start(&supervisorStart[i],	supervisorRun,	&IDs[i]);
		rt_task_start(&release_cond[i],			randomRun,			&IDs[i]);
		rt_task_start(&value_run[i],				runValues,			&IDs[i]);
	}

	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_join(&ecThread[i]);
		rt_task_join(&frThread[i]);
		rt_task_join(&udpThread[i]);
		rt_task_join(&statusThread[i]);
		rt_task_join(&supervisorThread[i]);

		rt_task_join(&supervisorStart[i]);
		rt_task_join(&release_cond[i]);
		rt_task_join(&value_run[i]);

		rt_cond_delete(&freeCond[i]);
		rt_mutex_delete(&mutex[i]);
	}

	return 0;
}

void runECThread(void* cookie) {
	printf("EC Thread\n");
	ElevatorController* thisEC = (ElevatorController*)cookie;
	thisEC->run();
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

void floorRun(void *arg)
{
	int ID = *((int*)arg);
	unsigned char topItem;
	while(true)
	{
		if(!GDFailedEmptyHeap[ID])
		{
			rt_mutex_acquire(&mutex[ID], TM_INFINITE);
			rt_cond_wait(&freeCond[ID], &mutex[ID], TM_INFINITE);

			int upHeapSize = ec[ID].getUpHeap().getSize();
			int downHeapSize = ec[ID].getDownHeap().getSize();
			if(upHeapSize==0 && downHeapSize==0){GDFailedEmptyHeap[ID] = true;}

			if(downDirection[ID])
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
		
			if(topItem != destination[ID])
			{
				printf("FR%d next Dest is %d\n.", ID, topItem);
				es[ID].setFinalDestination(topItem);
				destination[ID] = topItem;
			}
			rt_mutex_release(&mutex[ID]);
		}
	}
}

bool releaseFreeCond(int ID)
{
	int heapSize = ec[ID].getUpHeap().getSize() + ec[ID].getDownHeap().getSize();
	if(heapSize > 0)
	{
		GDFailedEmptyHeap[ID] = false;
		rt_cond_signal(&freeCond[ID]);
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

void supervisorRun(void *arg)
{
	int ID = *((int*)arg);
	while(true)
	{
		rt_mutex_acquire(&mutex[ID], TM_INFINITE);
		if(GDFailed[ID] && GDFailedEmptyHeap[ID])
		{
			if(!taskAssigned[ID])
			{
				if((downDirection[ID] && destination[ID]!=0) || destination[ID] == MAX_FLOORS)
				{
					destination[ID]--;
					es[ID].setFinalDestination(destination[ID]);
				}else if(destination[ID] == 0 || destination[ID] != MAX_FLOORS)
				{
					destination[ID]++;
					es[ID].setFinalDestination(destination[ID]);
				}
			}
		}
		rt_mutex_release(&mutex[ID]);
		sleep(20);
	}
}

void updateStatusBuffer(int ID)
{
	//upheap and the downheap (hallcall and floor selections should be included.
	rt_mutex_acquire(&mutexBuffer[ID], TM_INFINITE);
	bool selectedBuffer = bufferSelection[ID];
	rt_mutex_release(&mutexBuffer[ID]);
	
	rt_mutex_acquire(&mutex[ID], TM_INFINITE);
	statusBuffer[ID][selectedBuffer][0] = currentFloor[ID];
	statusBuffer[ID][selectedBuffer][1] = direction[ID];
	statusBuffer[ID][selectedBuffer][2] = currentPosition[ID];
	statusBuffer[ID][selectedBuffer][3] = currentSpeed[ID];
	//printf("writting to buffer %d %s\n", selectedBuffer, statusBuffer[selectedBuffer]);
	rt_mutex_release(&mutex[ID]);

	rt_mutex_acquire(&mutexBuffer[ID], TM_INFINITE);
	bufferSelection[ID] = ++bufferSelection[ID] % 2;
	rt_mutex_release(&mutexBuffer[ID]);
}

void statusRun(void *arg)
{
	int ID = *((int*)arg);
	while(true)
	{
		sleep(3);
		rt_mutex_acquire(&mutex[ID], TM_INFINITE);
		currentFloor[ID] = es[ID].getCurrentFloor();
		taskAssigned[ID] = es[ID].getIsTaskActive();
		if(taskAssigned[ID] && (es[ID].getIsDirectionUp()))
		{
			upDirection[ID] = true;
		}
		downDirection[ID] = !(es[ID].getIsDirectionUp());

		if(upDirection[ID]){direction[ID] = DIRECTION_UP;}
		else{direction[ID] = DIRECTION_DOWN;}

		currentPosition = ceil(elevatorSimulator.geCurrentPosition());
		taskActive = taskAssigned;

		int upHeapSize = ec[ID].getUpHeap().getSize();
		int downHeapSize = ec[ID].getDownHeap().getSize();
		if(upHeapSize==0 && downHeapSize==0){GDFailedEmptyHeap[ID] = true;}
		
		if(upHeapSize > 0)
		{
			int topItem = (int)(ec[ID].getUpHeap().peek());
			if(currentFloor[ID] == topItem && !taskAssigned)
			{
				printf("ST%d Task Completed %d\n", ID, destination[ID]);
				ec[ID].getUpHeap().pop();
				releaseFreeCond(ID);
			}else if(upDirection[ID] && topItem < destination[ID])
			{
				releaseFreeCond(ID);
			}
		} 
		
		if(downHeapSize > 0)
		{
			int topItem = (int)(ec[ID].getDownHeap().peek());
			if(currentFloor[ID] == topItem && !taskAssigned)
			{
				printf("ST%d Task Completed %d\n", ID, destination[ID]);
				ec[ID].getDownHeap().pop();
				releaseFreeCond(ID);
			}else if(downDirection[ID] && topItem > destination[ID])
			{
				releaseFreeCond(ID);
			}
		}

		rt_mutex_release(&mutex[ID]);
		updateStatusBuffer(ID);
	}
}

// this function is just for my testing purposes
void randomRun(void *arg)
{
	int ID = *((int*)arg);
	ec[ID].getUpHeap().pushHallCall(5);
	releaseFreeCond(ID);
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
	int ID = *((int*)arg);

	while(true)
	{
		usleep(1000000);
		es[ID].calculateValues();
		std::cout << "RV" << ID << " ";
		es[ID].print();
	}
}

// this function is just for my testing purposes
void supervisorStartUp(void *arg)
{
	int ID = *((int*)arg);

	sleep(200);
	printf("SSU%d GroupDispatcher FAILED, Oh my god :(\n", ID);
	GDFailed[ID] = true;
	sleep(400);
	printf("SSU%d GroupDispatcher FIXED, good :)\n", ID);
	GDFailed[ID] = false;

	ec[ID].getUpHeap().pushHallCall(8);
	printf("SSU%d Hall Call Up Floor : 3\n", ID);
	sleep(40);

	ec[ID].getUpHeap().pushFloorRequest(15);
	printf("SSU%d Floor Request Up : 4\n", ID);
}

void catch_signal(int sig) {
	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_delete(&ecThread[i]);
		rt_task_delete(&frThread[i]);
		rt_task_delete(&udpThread[i]);
		rt_task_delete(&statusThread[i]);
		rt_task_delete(&supervisorThread[i]);
		rt_task_delete(&supervisorStart[i]);
		rt_task_delete(&release_cond[i]);
		rt_task_delete(&value_run[i]);

		rt_cond_delete(&freeCond[i]);

		rt_mutex_delete(&mutexBuffer[i]);
		rt_mutex_delete(&mutex[i]);
	}

	exit(1);
}
