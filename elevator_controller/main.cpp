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

#include "Heap.hpp"
#include "ElevatorSimulator.hpp"
#include "ElevatorCommon.hpp"

RT_TASK floor_run, supervisor, supervisorStart, release_cond, value_run, status;
RT_MUTEX mutex, mutexBuffer;
RT_COND freeCond;

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

//will hold both up call calls and up floor selection
UpwardFloorRunHeap heapUp;
//will hold both Down call calls and Down floor selection
DownwardFloorRunHeap heapDown;
//will simulate the elevator and provide status to the status thread.
ElevatorSimulator elevatorSimulator;

//double buffer used by both communication and status threads.
unsigned char statusBuffer[2][BUFFSIZE];
bool bufferSelecton = 0;

void floorRun(void *arg)
{
	int topItem;
	while(true)
	{
		if(!GDFailedEmptyHeap)
		{
			rt_mutex_acquire(&mutex, TM_INFINITE);
			rt_cond_wait(&freeCond, &mutex, TM_INFINITE);

			int upHeapSize = heapUp.getSize();
			int downHeapSize = heapDown.getSize();
			if(upHeapSize==0 && downHeapSize==0){GDFailedEmptyHeap = true;}

			if(downDirection)
			{
				if(downHeapSize > 0)
				{
					topItem = (int)(heapDown.peek());
				}else if(upHeapSize > 0)
				{
					topItem = (int)(heapUp.peek());
				}
			}else
			{
				if(upHeapSize > 0)
				{
					topItem = (int)(heapUp.peek());
				}else if(downHeapSize > 0)
				{
					topItem = (int)(heapDown.peek());
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
	int heapSize = heapUp.getSize() + heapDown.getSize();
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
					elevatorSimulator.setFinalDestination(destination);
				}else if(destination == 0 || destination != MAX_FLOORS)
				{
					destination++;
					elevatorSimulator.setFinalDestination(destination);
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
		currentFloor = elevatorSimulator.getCurrentFloor();
		taskAssigned = elevatorSimulator.getIsTaskActive();
		if(taskAssigned && (elevatorSimulator.getIsDirectionUp()))
		{
			upDirection = true;
		}
		downDirection = !(elevatorSimulator.getIsDirectionUp());

		if(upDirection){direction = HALL_CALL_DIRECTION_UP;}
		else{direction = HALL_CALL_DIRECTION_DOWN;}

		currentPosition = elevatorSimulator.geCurrentPosition();
		currentSpeed = elevatorSimulator.getCurrentSpeed();

		int upHeapSize = heapUp.getSize();
		int downHeapSize = heapDown.getSize();
		if(upHeapSize==0 && downHeapSize==0){GDFailedEmptyHeap = true;}
		
		if(upHeapSize > 0)
		{
			int topItem = (int)(heapUp.peek());
			if(currentFloor == topItem && !taskAssigned)
			{
				printf("Task Completed %d\n", destination);
				heapUp.pop();
				releaseFreeCond();	
			}else if(upDirection && topItem < destination)
			{
				releaseFreeCond();
			}
		} 
		
		if(downHeapSize > 0)
		{
			int topItem = (int)(heapDown.peek());
			if(currentFloor == topItem && !taskAssigned)
			{
				printf("Task Completed %d\n", destination);
				heapDown.pop();
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

//this function is just for my testing purposes
void randomRun(void *arg)
{
	heapUp.pushHallCall(5);
	releaseFreeCond();
	printf("Hall Call Up Floor : 5\n");
	sleep(40);

	heapUp.pushHallCall(3);
	printf("Hall Call Up Floor : 3\n");
	sleep(40);

	heapUp.pushFloorRequest(4);
	printf("Floor Request Up : 4\n");

	heapUp.pushFloorRequest(9);
	printf("Floor Request Up : 9\n");
	sleep(40);

	heapDown.pushHallCall(11);
	printf("Hall Call Down Floor : 11\n");
	sleep(40);

	heapDown.pushHallCall(5);
	printf("Hall Call Down Floor : 5\n");
	rt_task_sleep(5000000000 * 2);

	heapDown.pushFloorRequest(3);
	printf("Floor Request Down : 3\n");
	sleep(40);

	heapDown.pushFloorRequest(0);
	printf("Floor Request Down : 0\n");
	sleep(40);
}

//this function is just for my testing purposes
void runValues(void *arg)
{
	while(true)
	{
		usleep(1000000);
		elevatorSimulator.calculateValues();
		elevatorSimulator.print();
	}
}

//this function is just for my testing purposes
void supervisorStartUp(void *arg)
{
	sleep(200);
	printf("ElevatorDespatcher FAILED, Oh my god :(\n");
	GDFailed = true;
	sleep(400);
	printf("ElevatorDespatcher FIXED, good :)\n");
	GDFailed = false;

	heapUp.pushHallCall(8);
	printf("Hall Call Up Floor : 3\n");
	sleep(40);

	heapUp.pushFloorRequest(15);
	printf("Floor Request Up : 4\n");
}


void catch_signal(int sig)
{
}

int main(int argc, char* argv[]) {
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);

	//created required mutex and condition variables
	rt_mutex_create(&mutex, NULL);
	rt_mutex_create(&mutexBuffer, NULL);

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

	rt_task_delete(&floor_run);
}

