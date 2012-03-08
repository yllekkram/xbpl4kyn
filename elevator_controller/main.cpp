#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <native/cond.h>

#include "Heap.hpp"
#include "ElevatorSimulator.hpp"

#define ELEVATOR_SIZE 2

RT_TASK floor_run;
RT_MUTEX mutex;
RT_COND freeCond, reassignment;
UpwardFloorRunHeap *heapUp = new UpwardFloorRunHeap();
DownwardFloorRunHeap *heapDown = new DownwardFloorRunHeap();
ElevatorSimulator *elevatorSimulator = new ElevatorSimulator();
int currentDestination = 0;

void floorRun(void *arg)
{
	char topItem;
	rt_mutex_acquire(&mutex, TM_INFINITE);
	while(true)
	{
		printf("Waiting for freeCond ... \n");
		rt_cond_wait(&freeCond, &mutex, TM_INFINITE);
		printf("recieved freeCond condition ...\n");
		topItem = heapUp -> peek();
		printf("Top item heapUp peeked %c\n", topItem);
		if(topItem == NULL)
		{
			topItem = heapDown -> pop();
			printf("Top item heapDown pop %c\n", topItem);
		}else
		{
			topItem = heapUp -> pop();
			printf("Top item heapUp pop %c\n", topItem);
		}

		if(topItem != currentDestination)
		{
			int nextDestination = atoi((char *) topItem);
			elevatorSimulator -> setFinalDestination(nextDestination);
			printf("Current destination is %d Next destination is %d\n", currentDestination, nextDestination);
			currentDestination = nextDestination;
		}
		printf("Waiting for reassignment ...\n");
		rt_cond_wait(&reassignment, &mutex, TM_INFINITE);
		printf("recieved reassignment condition ...\n");
	}
	rt_mutex_release(&mutex);
}



void catch_signal(int sig)
{
}

int main(int argc, char* argv[]) {
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);

	rt_mutex_create(&mutex, NULL);
	rt_cond_create(&freeCond, NULL);
	rt_cond_create(&reassignment, NULL);
	printf("Mutex and Conditions Created.\n");

	//will be removed from main once the structure is completed
	rt_task_create(&floor_run, NULL, 0, 99, 0);
	rt_task_start(&floor_run, &floorRun, NULL);
	printf("Runner Thread Created and Started.\n");
	pause();

	rt_task_delete(&floor_run);
}

