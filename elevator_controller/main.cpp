#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
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
#define NUM_ELEVATORS 8
#define STANDARD_PAUSE 2500000000U
/* End Constants */

/* Funtion Prototypes */
void catch_signal(int);
void floorRun(void*);
bool relseaseFreeCond();
void runECThread(void*);
void runUDPThread(void*);
void setupElevatorController(ElevatorController*, UDPView*, char*, char*, char*, char*);
void statusRun(void*);
void supervisorRun(void*);
	/* Functions for testing */
	void randomRun(void*);
	void runValues(void*);
	void supervisorStartUp(void*);
	/* End Funtions for testing */
/* End Function Prototypes */

/* Global Data Declarations */
RT_TASK floor_run, supervisor, supervisorStart, release_cond, value_run, status;
RT_MUTEX mutex;
RT_COND freeCond;

RT_TASK ecThread[NUM_ELEVATORS];
RT_TASK udpThread[NUM_ELEVATORS];

ElevatorController	ec[NUM_ELEVATORS];
UDPView							uv[NUM_ELEVATORS];

int currentFloor = 0;
int taskAssigned = false;
bool upDirection = false;
bool downDirection = false;
float currentPosition = 0;
float currentSpeed = 0;
int destination = 0;
bool GDFailed = false;
bool GDFailedEmptyHeap = false;

ElevatorSimulator elevatorSimulator;
/* End Global Data Declarations */

int main(int argc, char* argv[]) {
	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);

	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_create(&ecThread[i], NULL, 0, 99, T_JOINABLE);
		rt_task_create(&udpThread[i], NULL, 0, 99, T_JOINABLE);

		// setupElevatorController(&ec[i], &uv[i], "192.168.251.1", "5000", "192.168.251.1", "5003");

		// rt_task_start(&ecThread[i],		runECThread,	&ec[i]);
		// rt_task_start(&udpThread[i],	runUDPThread,	&uv[i]);
	}

	//created rewuired mutex and condition variables
	rt_mutex_create(&mutex, NULL);
	//will be signaled if there isa task avaiable in the heap.
	rt_cond_create(&freeCond, NULL);

	//create and run the run floor thread
	rt_task_create(&floor_run, NULL, 0, 99, 0);
	rt_task_start(&floor_run, &floorRun, NULL);

	rt_task_create(&supervisor, NULL, 0, 99, 0);
	rt_task_start(&supervisor, &supervisorRun, NULL);

	rt_task_create(&status, NULL, 0, 99, 0);
	rt_task_start(&status, &statusRun, NULL);
	
	//following is for my testing purpose
	rt_task_create(&supervisorStart, NULL, 0, 99, 0);
	rt_task_start(&supervisorStart, &supervisorStartUp, NULL);
	rt_task_create(&release_cond, NULL, 0, 99, 0);
	rt_task_start(&release_cond, &randomRun, NULL);
	rt_task_create(&value_run, NULL, 0, 99, 0);
	rt_task_start(&value_run, &runValues, NULL);

	pause();

	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_join(&ecThread[i]);
		rt_task_join(&udpThread[i]);
	}

	rt_task_delete(&floor_run);
	rt_task_delete(&supervisor);
	rt_task_delete(&status);

	rt_cond_delete(&freeCond);
	rt_mutex_delete(&mutex);
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
	int topItem;
	while(true)
	{
		if(!GDFailedEmptyHeap)
		{
			rt_mutex_acquire(&mutex, TM_INFINITE);
			rt_cond_wait(&freeCond, &mutex, TM_INFINITE);

			int upHeapSize = ec[0].getUpHeap().getSize();
			int downHeapSize = ec[0].getDownHeap().getSize();
			if(upHeapSize==0 && downHeapSize==0){GDFailedEmptyHeap = true;}

			if(downDirection)
			{
				if(downHeapSize > 0)
				{
					topItem = (int)(ec[0].getDownHeap().peek());
				}else if(upHeapSize > 0)
				{
					topItem = (int)(ec[0].getUpHeap().peek());
				}
			}else
			{
				if(upHeapSize > 0)
				{
					topItem = (int)(ec[0].getUpHeap().peek());
				}else if(downHeapSize > 0)
				{
					topItem = (int)(ec[0].getDownHeap().peek());
				}
			}
		
			if(topItem != destination)
			{
				printf("Floor Run. next Dest is %d\n.", topItem);
				elevatorSimulator.setFinalDestination(topItem);
				destination = topItem;
			}
			rt_mutex_release(&mutex);
		}
	}
}

bool releaseFreeCond()
{
	int heapSize = ec[0].getUpHeap().getSize() + ec[0].getDownHeap().getSize();
	if(heapSize > 0)
	{
		GDFailedEmptyHeap = false;
		rt_cond_signal(&freeCond);
		return true;
	}else
	{
		return false;
	}
}

void supervisorRun(void *arg)
{
	while(true)
	{
		rt_mutex_acquire(&mutex, TM_INFINITE);
		if(GDFailed && GDFailedEmptyHeap)
		{
			if(!taskAssigned)
			{
				if((downDirection && destination!=0) || destination == MAX_FLOORS)
				{
					destination--;
					elevatorSimulator.setFinalDestination(destination);
				}else if(destination == 0 || destination != MAX_FLOORS)
				{
					destination++;
					elevatorSimulator.setFinalDestination(destination);
				}
			}
		}
		rt_mutex_release(&mutex);
		rt_task_sleep(500000000);
	}
}

void statusRun(void *arg)
{
	while(true)
	{
		rt_task_sleep(75000000);
		currentFloor = elevatorSimulator.getCurrentFloor();
		taskAssigned = elevatorSimulator.getIsTaskActive();
		if(taskAssigned && (elevatorSimulator.getIsDirectionUp()))
		{
			upDirection = true;
		}
		downDirection = !(elevatorSimulator.getIsDirectionUp());
		currentPosition = elevatorSimulator.geCurrentPosition();
		currentSpeed = elevatorSimulator.getCurrentSpeed();

		int upHeapSize = ec[0].getUpHeap().getSize();
		int downHeapSize = ec[0].getDownHeap().getSize();
		if(upHeapSize==0 && downHeapSize==0){GDFailedEmptyHeap = true;}
		if(upDirection && (upHeapSize > 0))
		{
			int topItem = (int)(ec[0].getUpHeap().peek());
			if(currentFloor == topItem && !taskAssigned)
			{
				printf("Task Completed %d\n", destination);
				ec[0].getUpHeap().pop();
				releaseFreeCond();
			}else if(topItem < destination)
			{
				releaseFreeCond();
			}
		}else if(downDirection && (downHeapSize > 0))
		{
			int topItem = (int)(ec[0].getDownHeap().peek());
			if(currentFloor == topItem && !taskAssigned)
			{
				printf("Task Completed %d\n", destination);
				ec[0].getDownHeap().pop();
				releaseFreeCond();
			}else if((int)topItem > destination)
			{
				releaseFreeCond();
			}
		}
	}
}

// this function is just for my testing purposes
void randomRun(void *arg)
{
	ec[0].getUpHeap().pushHallCall(5);
	releaseFreeCond();
	printf("Hall Call Up Floor : 5\n");
	rt_task_sleep(STANDARD_PAUSE * 2);

	ec[0].getUpHeap().pushHallCall(3);
	printf("Hall Call Up Floor : 3\n");
	rt_task_sleep(STANDARD_PAUSE * 2);

	ec[0].getUpHeap().pushFloorRequest(4);
	printf("Floor Request Up : 4\n");

	ec[0].getUpHeap().pushFloorRequest(9);
	printf("Floor Request Up : 9\n");
	rt_task_sleep(STANDARD_PAUSE * 2);

	ec[0].getDownHeap().pushHallCall(6);
	printf("Hall Call Down Floor : 6\n");
	rt_task_sleep(STANDARD_PAUSE * 2);

	ec[0].getDownHeap().pushHallCall(2);
	printf("Hall Call Down Floor : 2\n");
	rt_task_sleep(STANDARD_PAUSE * 2);

	ec[0].getDownHeap().pushFloorRequest(3);
	printf("Floor Request Down : 3\n");
	rt_task_sleep(STANDARD_PAUSE * 2);

	ec[0].getDownHeap().pushFloorRequest(0);
	printf("Floor Request Down : 0\n");
	rt_task_sleep(STANDARD_PAUSE * 2);
}

// this function is just for my testing purposes
void runValues(void *arg)
{
	while(true)
	{
		usleep(1000000);
		elevatorSimulator.calculateValues();
		elevatorSimulator.print();
	}
}

// this function is just for my testing purposes
void supervisorStartUp(void *arg)
{
	rt_task_sleep(STANDARD_PAUSE * 10);
	printf("ElevatorDespatcher FAILED, Oh my god :(\n");
	GDFailed = true;
	rt_task_sleep(STANDARD_PAUSE * 20);
	printf("ElevatorDespatcher FIXED, good :)\n");
	GDFailed = false;

	ec[0].getUpHeap().pushHallCall(8);
	printf("Hall Call Up Floor : 3\n");
	rt_task_sleep(STANDARD_PAUSE * 2);

	ec[0].getUpHeap().pushFloorRequest(15);
	printf("Floor Request Up : 4\n");
}

void catch_signal(int sig) {
	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_delete(&ecThread[i]);
		rt_task_delete(&udpThread[i]);
	}

	exit(1);
}
