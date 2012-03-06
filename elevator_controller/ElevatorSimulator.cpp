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
:currentPosition(0), currentSpeed(0), finalDestination(0), forcedIdel(false), accTime(0), decTime(0), maxDistTime(0), distAcc(0), distDec(0), distMaxSpeed(0), numState(0){
	tp = new struct timeb;
	tpe = new struct timeb;
}

ElevatorSimulator::~ElevatorSimulator(){
	delete tp;
	delete tpe;
}

void ElevatorSimulator::reset(){
}

void ElevatorSimulator::findGraph(float travelDistance){
	std::cout << "travelDistance is " << travelDistance << std::endl;
	accTime = (MAX_SPEED - currentSpeed)/ACCELERATION;
	std::cout << "accTime is " << accTime << std::endl;
	decTime = (0 - MAX_SPEED)/DECELATRAION;
	std::cout << "decTime is " << decTime << std::endl;
	distAcc = currentSpeed*accTime + (0.5)*ACCELERATION*accTime*accTime;
	std::cout << " distAcc is " << distAcc << std::endl;
	distDec = MAX_SPEED*decTime + (0.5)*DECELATRAION*decTime*decTime;
	std::cout << " distDec is " << distDec << std::endl;
	
	float totalDistAccDec = (distAcc + distDec);
	std::cout << " totalACCDEC is " << totalDistAccDec << std::endl;	
	if(totalDistAccDec<travelDistance){
		distMaxSpeed = travelDistance - totalDistAccDec;
		std::cout << " distMaxSpeed is " << distMaxSpeed << std::endl;
		maxDistTime = distMaxSpeed/MAX_SPEED;
		std::cout << " maxDistTime is " << maxDistTime << std::endl;
		numState = 3;
	}else if(distAcc > travelDistance || (distAcc+distDec) > travelDistance){
		numState = 2;
	}
}
 
bool ElevatorSimulator::calculateValues(){
	float distance = 0;
	ftime(tpe);
	std::cout << "Current Time" << tp->time << " " << tp->millitm << std::endl;
	float elaspedTime = (tpe->time - tp->time)*1000 + (tpe->millitm - tp->millitm);
	std::cout << " elaspedTime is " << elaspedTime << std::endl;
	if(numState==3){
		if(elaspedTime < accTime){
			std::cout << " 1 " << std::endl;
			distance = (0.5)*ACCELERATION*elaspedTime*elaspedTime;
		}else if(elaspedTime < (accTime+distMaxSpeed)){
			std::cout << " 2 " << std::endl;
			float extraTime = elaspedTime-accTime;
			distance = (0.5)*ACCELERATION*accTime*accTime + MAX_SPEED*extraTime;
		}else{
			std::cout << " 3 " << std::endl;
			float extraTime = elaspedTime-(accTime+maxDistTime);
			distance = (0.5)*ACCELERATION*accTime*accTime + MAX_SPEED*maxDistTime + (MAX_SPEED*extraTime + (0.5)*DECELATRAION*extraTime*extraTime);
		}
		std::cout << " DISTANCE IS " << distance << " CURRENTPOSITION IS : " << (currentPosition + distance) << " final dest is : " << finalDestination*FLOOR_HEIGHT << std::endl;
		float updatedPosition = currentPosition + distance;
		if(updatedPosition = finalDestination*FLOOR_HEIGHT)
		{
			return true;
		}
		return false;
	}
}

void ElevatorSimulator::setFinalDestination(int floorNum)
{
	finalDestination = floorNum;
	std::cout << "floorNum "<< floorNum << " finalDestination " << finalDestination << std::endl;
	ftime(tp);
	std::cout << "End Time "<< tp->time << " " << tp->millitm << std::endl;

	float destination = finalDestination*FLOOR_HEIGHT;
	std::cout << "destination "<< destination << " currentPosition " << currentPosition << std::endl;
	float requiredDistance = destination - currentPosition;
	if(currentPosition>destination)
	{
		requiredDistance = currentPosition - destination;
	}

	findGraph(requiredDistance);
}

