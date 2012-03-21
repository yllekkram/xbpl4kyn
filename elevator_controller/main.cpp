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
void statusRun(void*);
void supervisorRun(void*);
	/* Functions for testing */
	void randomRun(void*);
	void runValues(void*);
	void supervisorStartUp(void*);
	/* End Funtions for testing */
/* End Function Prototypes */

/* Global Data Declarations */
RT_TASK supervisorStart, release_cond, value_run, status;
RT_MUTEX mutex, mutexBuffer;
RT_COND freeCond;

RT_TASK ecThread[NUM_ELEVATORS];
RT_TASK frThread[NUM_ELEVATORS];
RT_TASK udpThread[NUM_ELEVATORS];
RT_TASK statusThread[NUM_ELEVATORS];
RT_TASK supervisorThread[NUM_ELEVATORS];

ElevatorController	ec[NUM_ELEVATORS];
UDPView							uv[NUM_ELEVATORS];
ElevatorSimulator 	es[NUM_ELEVATORS];

//status updated every 75ms by the status thread
char currentFloor;
char direction;
char currentPosition;
char currentSpeed;

//will be used by the other threads.
int destination = 0;
int taskAssigned = false;
bool upDirection = false;
bool downDirection = false;
bool GDFailed = false;
bool GDFailedEmptyHeap = false;

//double buffer used by both communication and status threads.
unsigned char statusBuffer[2][BUFFSIZE];
bool bufferSelecton = 0;
/* End Global Data Declarations */

int main(int argc, char* argv[]) {
	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);

	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	//created rewuired mutex and condition variables
	rt_mutex_create(&mutex, NULL);
	//will be signaled if there isa task avaiable in the heap.
	rt_cond_create(&freeCond, NULL);

	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_create(&ecThread[i], 					NULL, 0, 99, T_JOINABLE);
		rt_task_create(&frThread[i], 					NULL, 0, 99, T_JOINABLE);
		rt_task_create(&statusThread[i], 			NULL, 0, 99, T_JOINABLE);
		rt_task_create(&supervisorThread[i], 	NULL, 0, 99, T_JOINABLE);
		rt_task_create(&udpThread[i], 				NULL, 0, 99, T_JOINABLE);

		// setupElevatorController(&ec[i], &uv[i], "192.168.251.1", "5000", "192.168.251.1", "5003");

		// rt_task_start(&ecThread[i],					runECThread,	&ec[i]);
		// rt_task_start(&udpThread[i],				runUDPThread,	&uv[i]);
		rt_task_start(&frThread[i], 				floorRun, 			&i);
		rt_task_start(&supervisorThread[i], supervisorRun, 	NULL);
		rt_task_start(&statusThread[i], 		statusRun, 			NULL);
	}

	//following is for my testing purpose
	rt_task_create(&supervisorStart, NULL, 0, 99, 0);
	rt_task_start(&supervisorStart, &supervisorStartUp, NULL);
	rt_task_create(&release_cond, NULL, 0, 99, 0);
	rt_task_start(&release_cond, &randomRun, NULL);
	rt_task_create(&value_run, NULL, 0, 99, 0);
	rt_task_start(&value_run, &runValues, NULL);

	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_join(&ecThread[i]);
		rt_task_join(&frThread[i]);
		rt_task_join(&udpThread[i]);
		rt_task_join(&statusThread[i]);
		rt_task_join(&supervisorThread[i]);
	}

	rt_task_delete(&value_run);
	rt_task_delete(&release_cond);
	rt_task_delete(&supervisorStart);
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
	int ID = *((int*)arg);
	int topItem;
	while(true)
	{
		if(!GDFailedEmptyHeap)
		{
			rt_mutex_acquire(&mutex, TM_INFINITE);
			rt_cond_wait(&freeCond, &mutex, TM_INFINITE);

			int upHeapSize = ec[ID].getUpHeap().getSize();
			int downHeapSize = ec[ID].getDownHeap().getSize();
			if(upHeapSize==0 && downHeapSize==0){GDFailedEmptyHeap = true;}

			if(downDirection)
			{
				if(downHeapSize > 0)
				{
					topItem = (int)(ec[ID].getDownHeap().peek());
				}else if(upHeapSize > 0)
				{
					topItem = (int)(ec[ID].getUpHeap().peek());
				}
			}else
			{
				if(upHeapSize > 0)
				{
					topItem = (int)(ec[ID].getUpHeap().peek());
				}else if(downHeapSize > 0)
				{
					topItem = (int)(ec[ID].getDownHeap().peek());
				}
			}
		
			if(topItem != destination)
			{
				printf("Floor Run. next Dest is %d\n.", topItem);
				es[ID].setFinalDestination(topItem);
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

void sleep(int numTimes)
{
	for(int i=0; i<numTimes; i++)
	{
		rt_task_sleep(250000000);
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
					es[0].setFinalDestination(destination);
				}else if(destination == 0 || destination != MAX_FLOORS)
				{
					destination++;
					es[0].setFinalDestination(destination);
				}
			}
		}
		rt_mutex_release(&mutex);
		sleep(20);
	}
}

void updateStatusBuffer()
{
	//upheap and the downheap (hallcall and floor selections should be included.
	rt_mutex_acquire(&mutexBuffer, TM_INFINITE);
	bool selectedBuffer = bufferSelecton;
	rt_mutex_release(&mutexBuffer);
	
	rt_mutex_acquire(&mutex, TM_INFINITE);
	statusBuffer[selectedBuffer][0] = currentFloor;
	statusBuffer[selectedBuffer][1] = direction;
	statusBuffer[selectedBuffer][2] = currentPosition;
	statusBuffer[selectedBuffer][3] = currentSpeed;
	//printf("writting to buffer %d %s\n", selectedBuffer, statusBuffer[selectedBuffer]);
	rt_mutex_release(&mutex);

	rt_mutex_acquire(&mutexBuffer, TM_INFINITE);
	bufferSelecton = !bufferSelecton;
	rt_mutex_release(&mutexBuffer);
}

void statusRun(void *arg)
{
	while(true)
	{
		sleep(3);
		rt_mutex_acquire(&mutex, TM_INFINITE);
		currentFloor = es[0].getCurrentFloor();
		taskAssigned = es[0].getIsTaskActive();
		if(taskAssigned && (es[0].getIsDirectionUp()))
		{
			upDirection = true;
		}
		downDirection = !(es[0].getIsDirectionUp());

		if(upDirection){direction = HALL_CALL_DIRECTION_UP;}
		else{direction = HALL_CALL_DIRECTION_DOWN;}

		currentPosition = (unsigned char)es[0].geCurrentPosition();
		currentSpeed = (unsigned char)es[0].getCurrentSpeed();

		int upHeapSize = ec[0].getUpHeap().getSize();
		int downHeapSize = ec[0].getDownHeap().getSize();
		if(upHeapSize==0 && downHeapSize==0){GDFailedEmptyHeap = true;}
		
		if(upHeapSize > 0)
		{
			int topItem = (int)(ec[0].getUpHeap().peek());
			if(currentFloor == topItem && !taskAssigned)
			{
				printf("Task Completed %d\n", destination);
				ec[0].getUpHeap().pop();
				releaseFreeCond();
			}else if(upDirection && topItem < destination)
			{
				releaseFreeCond();
			}
		} 
		
		if(downHeapSize > 0)
		{
			int topItem = (int)(ec[0].getDownHeap().peek());
			if(currentFloor == topItem && !taskAssigned)
			{
				printf("Task Completed %d\n", destination);
				ec[0].getDownHeap().pop();
				releaseFreeCond();
			}else if(downDirection && topItem > destination)
			{
				releaseFreeCond();
			}
		}

		rt_mutex_release(&mutex);
		updateStatusBuffer();
	}
}

// this function is just for my testing purposes
void randomRun(void *arg)
{
	ec[0].getUpHeap().pushHallCall(5);
	releaseFreeCond();
	printf("Hall Call Up Floor : 5\n");
	sleep(40);

	ec[0].getUpHeap().pushHallCall(3);
	printf("Hall Call Up Floor : 3\n");
	sleep(40);

	ec[0].getUpHeap().pushFloorRequest(4);
	printf("Floor Request Up : 4\n");

	ec[0].getUpHeap().pushFloorRequest(9);
	printf("Floor Request Up : 9\n");
	sleep(40);

	ec[0].getDownHeap().pushHallCall(6);
	printf("Hall Call Down Floor : 6\n");
	sleep(40);

	ec[0].getDownHeap().pushHallCall(2);
	printf("Hall Call Down Floor : 2\n");
	rt_task_sleep(STANDARD_PAUSE * 2);

	ec[0].getDownHeap().pushFloorRequest(3);
	printf("Floor Request Down : 3\n");
	sleep(40);

	ec[0].getDownHeap().pushFloorRequest(0);
	printf("Floor Request Down : 0\n");
	sleep(40);
}

// this function is just for my testing purposes
void runValues(void *arg)
{
	while(true)
	{
		usleep(1000000);
		es[0].calculateValues();
		es[0].print();
	}
}

// this function is just for my testing purposes
void supervisorStartUp(void *arg)
{
	sleep(200);
	printf("ElevatorDespatcher FAILED, Oh my god :(\n");
	GDFailed = true;
	sleep(400);
	printf("ElevatorDespatcher FIXED, good :)\n");
	GDFailed = false;

	ec[0].getUpHeap().pushHallCall(8);
	printf("Hall Call Up Floor : 3\n");
	sleep(40);

	ec[0].getUpHeap().pushFloorRequest(15);
	printf("Floor Request Up : 4\n");
}

void catch_signal(int sig) {
	for (int i = 0; i < NUM_ELEVATORS; i++) {
		rt_task_delete(&ecThread[i]);
		rt_task_delete(&frThread[i]);
		rt_task_delete(&udpThread[i]);
		rt_task_delete(&statusThread[i]);
		rt_task_delete(&supervisorThread[i]);
	}

	exit(1);
}
