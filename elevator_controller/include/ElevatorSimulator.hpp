#ifndef ELEVATOR_SIMULATOR_HPP
#define ELEVATOR_SIMULATOR_HPP

#define MAX_SPEED	3			//maximum speed of elevator
#define ACCELERATION	0.64			//acceleration of elevator
#define DECELATRAION	-0.64			//Deceleration of elevator
#define FLOOR_HEIGHT	10			//height of floor
#define DOOR_OPEN_TIME	2			//time take to open door
#define DOOR_CLOSE_TIME	2			//time until the door is open for
#define IN_OUT_TIME	2			//time take to close door

class ElevatorSimulator {
	public:
		ElevatorSimulator();
		~ElevatorSimulator();
		void reset();
		void findGraph(float travelDistance);
		void calculateValues();
		void setFinalDestination(int floorNum);
		float getCurrentSpeed();
		float geCurrentPosition();
		int getCurrentFloor();
		bool getIsTaskActive();
		bool getIsDirectionUp();
		bool getIsDoorOpening();
		bool getIsDoorOpen();
		bool getIsDoorClosing();
		void setDoorOpen();
		void setDoorClose();
		void print();

		
	private:
		struct timeb *tp;			//start time structure
		struct timeb *tpe;			//current time structure
		float currentPosition;			//is the current height in meter
		float currentSpeed;			//is the current speed of the elevator in m/s
		float tempCurrentPosition;		//temp calue of current location at the start of a task (used for down ward calculations)
		int currentFloor;			//is the current height in meter
		int finalDestination;			//final destination is the final floor number
		float midLocation;			//final destination is the final floor number
		bool task;				//is there a task available
		bool directionUP;			//is elevator traveling up
		bool DoorOpening;			//is elevator door opening
		bool DoorOpen;				//is elevator door open
		bool DoorClosing;			//is elevator door closing

		int numState;				//the state of elevator movement
		float accTime;				//the acceleration time to maximum speed
		float decTime;				//the deceleration time from maximum speed to zero
		float maxDistTime;			//maximum time spent on maximum speed
		float distAcc;				//the acceleration distance to maximum speed
		float distDec;				//the deceleration distance from maximum speed to zero
		float distMaxSpeed;			//maximum distance spent on maximum speed
		float travelTime;			//total time spent traveling
		float doorOpenedTime;			//time until door open for current task
		bool doorOpenButton;
		bool doorCloseButton;
		float doorOpenedAndReadyTime;		//time until door open until close for current task
		float doorClosedTime;			//time until door close for current task
		float midTime;				//mid time before acceleration switch to deceleration movement
		float midSpeed;				//mid speed before acceleration switch to deceleration movement
};	

#endif
