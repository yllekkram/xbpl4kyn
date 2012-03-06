#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "ElevatorSimulator.hpp"


int main(int argc, char* argv[]) {
	ElevatorSimulator es;
	es.setFinalDestination(10);
	while(!es.calculateValues())
	{
		sleep(1);
	}
	exit(0);
}
