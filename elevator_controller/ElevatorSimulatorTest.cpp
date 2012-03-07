#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "ElevatorSimulator.hpp"


int main(int argc, char* argv[]) {
	ElevatorSimulator es;
	es.setFinalDestination(2);
	while(true)
	{
		es.calculateValues();
		usleep(1000000);
	}
	exit(0);
}
