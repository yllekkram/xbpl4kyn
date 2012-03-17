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

#define ELEVATOR_SIZE 2

RT_TASK floor_run, supervisor, supervisorStart, release_cond, value_run;
RT_MUTEX mutex;
RT_COND freeCond, reassignment;
UpwardFloorRunHeap *heapUp = new UpwardFloorRunHeap();
DownwardFloorRunHeap *heapDown = new DownwardFloorRunHeap();
ElevatorSimulator *elevatorSimulator = new ElevatorSimulator();
int destination = 0;
bool GDFailed = false;

void floorRun(void *arg)
{
	char topItem;
	while(!GDFailed)
	{
		rt_mutex_acquire(&mutex, TM_INFINITE);
		rt_cond_wait(&freeCond, &mutex, TM_INFINITE);

		int upHeapSize = heapUp -> getSize();
		int downHeapSize = heapDown -> getSize();

		if(!(elevatorSimulator -> getIsDirectionUp()))
		{
			if(downHeapSize > 0)
			{
				topItem = heapDown -> pop();
			}else if(upHeapSize > 0)
			{
				topItem = heapUp -> pop();
			}
		}else
		{
			if(upHeapSize > 0)
			{
				topItem = heapUp -> pop();
			}else if(downHeapSize > 0)
			{
				topItem = heapDown -> pop();
			}
		}

		if(topItem != destination)
		{
			int nextDestination = atoi(&topItem);
			int elevatorFloor = elevatorSimulator -> getCurrentFloor();
			if(elevatorFloor != destination)
			{
				if(elevatorSimulator -> getIsDirectionUp()){heapUp -> pushFloorRequest((char)destination);}
				else{heapDown -> pushFloorRequest((char)destination);}
			}
			printf("Floor Run. next Dest is %d\n", nextDestination);
			elevatorSimulator -> setFinalDestination(nextDestination);
			destination = nextDestination;
		}
		rt_mutex_release(&mutex);
		rt_cond_wait(&reassignment, &mutex, TM_INFINITE);
	}
}

void supervisorRun(void *arg)
{
	while(true)
	{
		//printf("supervisorRun STARTED.\n");
		if(GDFailed)
		{
			rt_mutex_acquire(&mutex, TM_INFINITE);
			if(!(elevatorSimulator -> getIsTaskActive()))
			{
				if((!(elevatorSimulator -> getIsDirectionUp()) && destination!=0) || destination == MAX_FLOORS)
				{
					destination--;
					printf("Supervisor Run. next Dest is %d\n", destination);
					elevatorSimulator -> setFinalDestination(destination);
				}else if(destination == 0 || destination != MAX_FLOORS)
				{
					destination++;
					printf("Supervisor Run. next Dest is %d\n", destination);
					elevatorSimulator -> setFinalDestination(destination);
				}
			}
			rt_mutex_release(&mutex);
		}
		rt_task_sleep(500000000);
	}
}

/*
//this function is just for my testing purposes
void randomRun(void *arg)
{
	int random_integer = rand();
	if(random_integer > 10000000)
	{
		random_integer/=1000;
	}
	while(true)
	{
		usleep(random_integer);
		bool busy = elevatorSimulator -> getIsTaskActive();
		if(!busy)
		{
			rt_cond_signal(&reassignment);
		}
		rt_cond_signal(&freeCond);
	}
}

//this function is just for my testing purposes
void runValues(void *arg)
{
	while(true)
	{
		usleep(1000000);
		elevatorSimulator -> calculateValues();
		elevatorSimulator -> print();
	}
}

//this function is just for my testing purposes
void supervisorStartUp(void *arg)
{
	usleep(30000000);
	printf("ElevatorDespatcher FAILED, Oh my god :(\n");
	GDFailed = true;
}
*/

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
	rt_cond_create(&freeCond, NULL);		//will be signaled if there isa task avaiable in the heap.
	rt_cond_create(&reassignment, NULL);		//will be signaled if there is a re assignment.

	//create and run the run floor thread
	rt_task_create(&floor_run, NULL, 0, 99, 0);
	rt_task_start(&floor_run, &floorRun, NULL);
	rt_task_create(&supervisor, NULL, 0, 99, 0);
	rt_task_start(&supervisor, &supervisorRun, NULL);
	
/*
	//following is for my testing purpose
	rt_task_create(&supervisorStart, NULL, 0, 99, 0);
	rt_task_start(&supervisorStart, &supervisorStartUp, NULL);
	heapDown -> pushFloorRequest('3');
	heapDown -> pushFloorRequest('5');
	heapDown -> pushHallCall('12');
	heapDown -> pushHallCall('9');
	heapUp -> pushFloorRequest('8');
	heapUp -> pushFloorRequest('4');
	heapUp -> pushHallCall('5');
	heapUp -> pushHallCall('4');
	rt_task_create(&release_cond, NULL, 0, 99, 0);
	rt_task_start(&release_cond, &randomRun, NULL);
	rt_task_create(&value_run, NULL, 0, 99, 0);
	rt_task_start(&value_run, &runValues, NULL);
*/
	

	pause();

	rt_task_delete(&floor_run);
}

