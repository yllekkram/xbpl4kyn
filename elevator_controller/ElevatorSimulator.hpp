#define MAX_SPEED	2.5
#define ACCELERATION	0.64
#define DECELATRAION	-0.64
#define FLOOR_HEIGHT	10
#define FLOOR_IDEL_TIME	10

class ElevatorSimulator {
	public:
		ElevatorSimulator();
		~ElevatorSimulator();
		void reset();
		void findGraph(float travelDistance);
		bool calculateValues();

		float getCurrentSpeed();
		float getCurrentLocation();
		int getCurrentFloor();

		void setFinalDestination(int floorNum);
		
	private:
		struct timeb *tp;
		struct timeb *tpe;
		float currentSpeed;		//is the current speed of the elevator in m/s
		float currentPosition;		//is the current height in meter
		int finalDestination;		//final destination is the final floor number
		bool forcedIdel;		//true if the elevator is in forced idel status

		float accTime;
		float decTime;
		float maxDistTime;
		float distAcc;
		float distDec;
		float distMaxSpeed;
		int numState;


		
};	
