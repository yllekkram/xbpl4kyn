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
RT_MUTEX mutex;
RT_COND freeCond;

int currentFloor = 0;
int taskAssigned = false;
bool upDirection = false;
bool downDirection = false;
float currentPosition = 0;
float currentSpeed = 0;
int destination = 0;
bool GDFailed = false;
bool GDFailedEmptyHeap = false;

UpwardFloorRunHeap heapUp;
DownwardFloorRunHeap heapDown;
ElevatorSimulator elevatorSimulator;




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

void statusRun(void *arg)
{
	while(true)
	{
		sleep(3);
		currentFloor = elevatorSimulator.getCurrentFloor();
		taskAssigned = elevatorSimulator.getIsTaskActive();
		if(taskAssigned && (elevatorSimulator.getIsDirectionUp()))
		{
			upDirection = true;
		}
		downDirection = !(elevatorSimulator.getIsDirectionUp());
		currentPosition = elevatorSimulator.geCurrentPosition();
		currentSpeed = elevatorSimulator.getCurrentSpeed();

		int upHeapSize = heapUp.getSize();
		int downHeapSize = heapDown.getSize();
		if(upHeapSize==0 && downHeapSize==0){GDFailedEmptyHeap = true;}
		if(upDirection && (upHeapSize > 0))
		{
			int topItem = (int)(heapUp.peek());
			if(currentFloor == topItem && !taskAssigned)
			{
				printf("Task Completed %d\n", destination);
				heapUp.pop();
				releaseFreeCond();	
			}else if(topItem < destination)
			{
				releaseFreeCond();
			}
		}else if(downDirection && (downHeapSize > 0))
		{
			int topItem = (int)(heapDown.peek());
			if(currentFloor == topItem && !taskAssigned)
			{
				printf("Task Completed %d\n", destination);
				heapDown.pop();
				releaseFreeCond();
			}else if((int)topItem > destination)
			{
				releaseFreeCond();
			}
		}
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

	heapDown.pushHallCall(6);
	printf("Hall Call Down Floor : 6\n");
	sleep(40);

	heapDown.pushHallCall(2);
	printf("Hall Call Down Floor : 2\n");
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

	rt_task_delete(&floor_run);
}

