#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <math.h>

#include "ElevatorSimulator.hpp"

ElevatorSimulator::ElevatorSimulator()
:currentPosition(0), currentSpeed(0), currentFloor(0), finalDestination(0), midLocation(0), task(false),
 midTime(0), DoorOpening(false), DoorOpen(false), DoorClosing(false), accTime(0), decTime(0), midSpeed(0),
 maxDistTime(0), distAcc(0), distDec(0), distMaxSpeed(0), numState(0), directionUP(true)
{
	tp = new struct timeb;					//structure for saving start time
	tpe = new struct timeb;					//structure for saving current time
}

ElevatorSimulator::~ElevatorSimulator(){
	delete tp;								//deallocate the structure
	delete tpe;								//deallocate the structure
}

//reset the following values back to their original state
void ElevatorSimulator::reset(){
	finalDestination = 0; midLocation = 0; accTime = 0; decTime = 0; distAcc = 0; distDec = 0; numState = 0;
	midSpeed = 0; midLocation = 0; midTime = 0; distMaxSpeed = 0; maxDistTime = 0; travelTime = 0;
	DoorOpening = false; DoorOpen = false; DoorClosing = false; task = false;
}

//this function will find necessary values to calculate currentPosition, currentSpeed
void ElevatorSimulator::findGraph(float travelDistance){
	accTime = (MAX_SPEED - currentSpeed)/ACCELERATION;		//time take to accelerate to max_speed
	decTime = (0 - MAX_SPEED)/DECELATRAION;					//time take to decelerate from max_speed to zero
	distAcc = currentSpeed*accTime + (0.5)*ACCELERATION*accTime*accTime;	//distance take to accelerate to max_speed
	distDec = MAX_SPEED*decTime + (0.5)*DECELATRAION*decTime*decTime;	//distance take to decelerate from max_speed to zero

	float totalDistAccDec = (distAcc + distDec);	//total time to accelerate and decelerate
	if(totalDistAccDec<travelDistance){				//if all three stages of movements are required : accelerate, max_speed and decelerate
		numState = 1;								//set the numState
		distMaxSpeed = travelDistance - totalDistAccDec;	//indicate the distance traveled only be max_speed (no acceleration and deceleration)
		maxDistTime = distMaxSpeed/MAX_SPEED;				//indicate the time traveled only be max_speed (no acceleration and deceleration)
		travelTime = accTime+maxDistTime+decTime;			//total travel time for the task
	}else if(distAcc*10 > travelDistance || totalDistAccDec > travelDistance){	//if only acceleration and deceleration stages are accessed
		numState = 2;									//set the numState
		midLocation = travelDistance/2;					//middle location where acceleration changes to declaration
		midTime = sqrt((2*midLocation)/ACCELERATION);	//middle time where acceleration changes to declaration
		midSpeed = ACCELERATION*midTime;				//middle speed where acceleration changes to declaration
		travelTime = midTime + (0-midSpeed)/DECELATRAION;	//total travel time for this state
	}
	doorOpenedTime = travelTime + DOOR_OPEN_TIME;			//time when the door finish opening for this task
	doorOpenedAndReadyTime = doorOpenedTime + IN_OUT_TIME;	//time until the door remain open for this task
	doorClosedTime = doorOpenedAndReadyTime + DOOR_CLOSE_TIME;	//time when the door finish closing for this task
}

//this will calculate the status of the elevator
void ElevatorSimulator::calculateValues(){
	ftime(tpe);		//current time
	if(task)		//task exist
	{
		float elaspedTime = (tpe->time - tp->time)*1000 + (tpe->millitm - tp->millitm);	//calculate the elapsed time in milliseconds
		elaspedTime /= 1000;	//change the time to seconds
		if(numState==1){		//if current status is 1
			if(elaspedTime < accTime){	//if the elapsed time is less than the time it takes to accelerate
				if(!directionUP){currentPosition = tempCurrentPosition - (0.5)*ACCELERATION*elaspedTime*elaspedTime;}	//calculate the current position using acceleration and time
				else{currentPosition = tempCurrentPosition + (0.5)*ACCELERATION*elaspedTime*elaspedTime;}
				currentSpeed += ACCELERATION*elaspedTime;	//calculate the current speed using acceleration and time
			}else if(elaspedTime < (accTime+maxDistTime)){	//if the elapsed time is less than the time it takes to accelerate and maximum speed time
				float extraTime = elaspedTime-accTime;	//amount of time spend with maximum speed
				if(!directionUP){currentPosition = tempCurrentPosition - (0.5)*ACCELERATION*accTime*accTime - MAX_SPEED*extraTime;}	//calculate the current position using acceleration and time and maximum speed
				else{currentPosition = tempCurrentPosition + (0.5)*ACCELERATION*accTime*accTime + MAX_SPEED*extraTime;}
				currentSpeed = MAX_SPEED;	//current speed will be the maximum speed
			}else{
				float extraTime = elaspedTime-(accTime+maxDistTime);	//currently in deceleration stage
				if(!directionUP){currentPosition = tempCurrentPosition - (0.5)*ACCELERATION*accTime*accTime - MAX_SPEED*maxDistTime - (MAX_SPEED*extraTime + (0.5)*DECELATRAION*extraTime*extraTime);}	//calculate the current position using acceleration and time and maximum speed and deceleration
				else{currentPosition = tempCurrentPosition + (0.5)*ACCELERATION*accTime*accTime + MAX_SPEED*maxDistTime + (MAX_SPEED*extraTime + (0.5)*DECELATRAION*extraTime*extraTime);}
				currentSpeed = DECELATRAION*extraTime + MAX_SPEED;	//calculate the current speed using acceleration and time and maximum speed and deceleration
			}
		}else if(numState==2){	//if current status is 1
			if(elaspedTime <= midTime){	//if the elapsed time is within the accelerating time period
				if(!directionUP){currentPosition = tempCurrentPosition - (0.5)*ACCELERATION*elaspedTime*elaspedTime;}	//calculate position using acceleration
				else{currentPosition = tempCurrentPosition + (0.5)*ACCELERATION*elaspedTime*elaspedTime;}
				currentSpeed += ACCELERATION*elaspedTime;	//calculate speed using acceleration
			}else if(elaspedTime > midTime){
				float extraTime = elaspedTime-midTime;		//calculate the time spend in deceleration
				if(!directionUP){currentPosition = tempCurrentPosition - (0.5)*ACCELERATION*midTime*midTime + (0.5)*DECELATRAION*extraTime*extraTime;}	//calculate current position using acceleration, deceleration
				else{currentPosition = tempCurrentPosition + (0.5)*ACCELERATION*midTime*midTime - (0.5)*DECELATRAION*extraTime*extraTime;}
				currentSpeed = DECELATRAION*extraTime + midSpeed;	//calculate speed using DECELATRAION and time
			}
		}

		if((currentPosition >= finalDestination*FLOOR_HEIGHT && directionUP) || (currentPosition <= finalDestination*FLOOR_HEIGHT && !directionUP))
		{
			currentPosition = finalDestination*FLOOR_HEIGHT;	//if the current position exceed final destination assign to final destination
			currentSpeed = 0;									//reset the current speed
		}
		if(currentSpeed >= MAX_SPEED)	//if the current speed exceed max speed
		{
			currentSpeed = MAX_SPEED;	//assign the max speed to current speed
		}

		currentFloor = (int)(currentPosition/FLOOR_HEIGHT);	//calculate current floor
		if(currentSpeed < 0 || ((currentPosition >= finalDestination*FLOOR_HEIGHT && directionUP || currentPosition <= finalDestination*FLOOR_HEIGHT && !directionUP) && elaspedTime >= travelTime && elaspedTime <= doorOpenedTime))	//if the destination is reached and door is opening
		{
			numState = 0;	//reset the state
			currentSpeed = 0;	//reset the current speed
			currentFloor = finalDestination;					//set the current floor to final destination
			currentPosition = finalDestination*FLOOR_HEIGHT;	//assign to final destination
			DoorOpening = true;	//assign the door opening variable to true
		}else if((currentPosition >= finalDestination*FLOOR_HEIGHT && directionUP || currentPosition <= finalDestination*FLOOR_HEIGHT && !directionUP) && elaspedTime >= doorOpenedTime && elaspedTime <= doorOpenedAndReadyTime)
		{
			DoorOpening = false;
			DoorOpen = true;	//assign the DoorOpen variable to true
		}else if((currentPosition >= finalDestination*FLOOR_HEIGHT && directionUP || currentPosition <= finalDestination*FLOOR_HEIGHT && !directionUP) && elaspedTime >= doorOpenedAndReadyTime && elaspedTime <= doorClosedTime)
		{
			DoorOpen = false;
			DoorClosing = true;	//assign the DoorClosing variable to true
		}else if((currentPosition >= finalDestination*FLOOR_HEIGHT && directionUP || currentPosition <= finalDestination*FLOOR_HEIGHT && !directionUP) && elaspedTime >= doorClosedTime)
		{
			DoorClosing = false;
			task = false;
			reset();	//task is done, reset all variable
		}
	}
}

//this method is called to assign a new task
//floorNum : int indicate the destination floor
void ElevatorSimulator::setFinalDestination(int floorNum)
{
	reset();
	tempCurrentPosition = currentPosition;
	finalDestination = floorNum;		//assign a new destination
	ftime(tp);							//calculate the start time
	task = true;						//assign the task value to true to indicate there is a task
	directionUP = true;
	float destination = floorNum*FLOOR_HEIGHT;	//find the location of the floor in meter
	float requiredDistance = destination - currentPosition;	//find the required distance needed travel assuming up direction
	if(currentPosition>destination)		//find we are traveling down
	{
		directionUP = false;	//set directionUp to false to indicate down
		requiredDistance = currentPosition - destination;	//find the required distance needed travel down
	}
	findGraph(requiredDistance);	//call this method
}


float ElevatorSimulator::getCurrentSpeed(){return currentSpeed;}
int ElevatorSimulator::getCurrentFloor(){return currentFloor;}
bool ElevatorSimulator::getIsCurrentTask(){return task;}
bool ElevatorSimulator::getIsDirectionUp(){return directionUP;}
bool ElevatorSimulator::getIsDoorOpening(){return DoorOpening;}
bool ElevatorSimulator::getIsDoorOpen(){return DoorOpen;}
bool ElevatorSimulator::getIsDoorClosing(){return DoorClosing;}
