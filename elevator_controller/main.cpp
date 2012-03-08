#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <native/cond.h>

#define ELEVATOR_SIZE 2

RT_MUTEX mutex;
RT_COND freeCond, reassignment;
UpwardFloorRunHeap heap = new UpwardFloorRunHeap();
UpwardFloorRunHeap heapTwo = new UpwardFloorRunHeap();
ElevatorSimulator elevatorSimulator = new ElevatorSimulator();
int currentDestination = 0;

void writeDatabase(void *arg)
{
	char topItem;
	rt_mutex_acquire(&mutex, TM_INFINITE);
	while(true)
	{
		rt_cond_wait(&freeCond, &mutex, TM_INFINITE);
		topItem = heap.pop();
		if(topItem == null)
		{
			topItem = heap.pop();
		}

		if(topItem != currentDestination)
		{
			int nextDestination = atoi(topItem);
			elevatorSimulator.setFinalDestination(nextDestination);
		}

		rt_cond_wait(&reassignment, &mutex, TM_INFINITE);
	}
	rt_mutex_release(&mutex);
}

int main(int argc, char* argv[]) {
	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);

	rt_mutex_create(&mutex, NULL);
	rt_cond_create(&freeCond, NULL);
	rt_cond_create(&reassignment, NULL);
	
	rt_print_auto_init(1);
	exit(0);
}

