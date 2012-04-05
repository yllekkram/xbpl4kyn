#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
using namespace std;
#define assertTrue(X) assert(X)

#include "ElevatorSimulator.hpp"

int destination = 3;
int timeSleep = 1000000;
int main(int argc, char* argv[]) {

	std::cout << "ELEVATOR SIMULATOR Test 1." << std::endl;
	std::cout << "Simulating a run from floor 0 to floor 3." << std::endl;
	std::cout << "This will include door functionality along with the elevator movement." << std::endl;
	ElevatorSimulator es;
	es.setFinalDestination(destination);
	int initialFloor = es.getCurrentFloor();
	while(true)
	{
		es.calculateValues();
		usleep(timeSleep);
		assertTrue(es.getDirection() == DIRECTION_UP);
		assertTrue(es.getCurrentFloor() >= initialFloor);
		if(es.getCurrentSpeed()>0)
		{
			assertTrue(es.getIsTaskActive());
		}

		if(es.getIsTaskActive() && es.getCurrentFloor() == destination)
		{
			if(es.getIsDoorOpening())
			{
				assertTrue(!es.getIsDoorClosing() && !es.getIsDoorOpen());
			}else if(es.getIsDoorClosing())
			{
				assertTrue(!es.getIsDoorOpening() && !es.getIsDoorOpen());
			}else if(es.getIsDoorOpen())
			{
				assertTrue(!es.getIsDoorOpening() && !es.getIsDoorClosing());
			}
		}

		if(!es.getIsTaskActive())
		{
			assertTrue(!es.getIsDoorOpening() && !es.getIsDoorClosing() && !es.getIsDoorOpen() && es.getCurrentSpeed() == 0);
			break;
		}
	}
	assertTrue(es.getCurrentFloor() == destination);
	std::cout << "ELEVATOR SIMULATOR Test 1 : Passed." << std::endl << std::endl;

	std::cout << "ELEVATOR SIMULATOR Test 2." << std::endl;
	std::cout << "Simulating a run from floor 3 to floor 12." << std::endl;
	std::cout << "At floor 8 new target of floor 5 is assigned to the elevator (opposite direction)." << std::endl;
	std::cout << "This will include door functionalities." << std::endl;
	destination = 12;
	initialFloor = es.getCurrentFloor();
	es.setFinalDestination(destination);
	while(true)
	{
		es.calculateValues();
		usleep(timeSleep/2);
		assertTrue(es.getCurrentFloor() >= 3);
		assertTrue(es.getDirection() == DIRECTION_UP);
		if(es.getCurrentFloor() >= 8 && up)
		{
			up = false;
			destination = 5;
			es.setFinalDestination(destination);
			initialFloor = es.getCurrentFloor();
		}

		if(es.getCurrentSpeed()==0 && es.getCurrentFloor()==destination && !es.getIsTaskActive())
		{
			break;
		}
	}
	assertTrue(es.getCurrentFloor() == destination);
	std::cout << "ELEVATOR SIMULATOR Test 2: Passed." << std::endl << std::endl;

	std::cout << "ELEVATOR SIMULATOR Test 3." << std::endl;
	std::cout << "Simulating a run from floor 5 to floor 12." << std::endl;
	std::cout << "At floor 7 new target of floor 8 is assigned to the elevator (same direction)." << std::endl;
	std::cout << "This will include door functionalities." << std::endl;
	destination = 12;
	es.setFinalDestination(destination);
	initialFloor = es.getCurrentFloor();
	bool assignTarget = false;
	while(true)
	{
		es.calculateValues();
		usleep(timeSleep/2);
		assertTrue(es.getCurrentFloor() >= 5);
		if(es.getCurrentFloor() >= 7 && !assignTarget)
		{
			destination = 8;
			es.setFinalDestination(destination);
			initialFloor = es.getCurrentFloor();
			assignTarget = true;
		}

		if(es.getCurrentSpeed()==0 && es.getCurrentFloor()==destination && !es.getIsTaskActive())
		{
			break;
		}
	}
	assertTrue(es.getCurrentFloor() == destination);
	std::cout << "ELEVATOR SIMULATOR Test 3: Passed." << std::endl  << std::endl;


	std::cout << "ELEVATOR SIMULATOR Test 4." << std::endl;
	std::cout << "Simulating a run from floor 8 to floor 12." << std::endl;
	std::cout << "At floor 11 (when elevator is decelerating) new target of floor 12 is assigned to the elevator." << std::endl;
	std::cout << "This will include door functionalities." << std::endl;
	destination = 12;
	es.setFinalDestination(destination);
	initialFloor = es.getCurrentFloor();
	assignTarget = false;
	while(true)
	{
		es.calculateValues();
		usleep(timeSleep/4);
		assertTrue(es.getCurrentFloor() >= 8);
		if(es.getCurrentFloor() >= 11 && !assignTarget)
		{
			destination = 15;
			es.setFinalDestination(destination);
			initialFloor = es.getCurrentFloor();
			assignTarget = true;
		}

		if(es.getCurrentSpeed()==0 && es.getCurrentFloor()==destination && !es.getIsTaskActive())
		{
			break;
		}
	}
	std::cout << "ELEVATOR SIMULATOR Test 4: Passed." << std::endl  << std::endl;
	exit(0);
}
