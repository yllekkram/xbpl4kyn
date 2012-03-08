#ifndef MESSAGE_HPP
#define MESSAGE_HPP

class Message {
	public:
		Message();
    Message(unsigned int len);
		Message(const char* buffer, unsigned int len);
		~Message();

		char* getBuffer() const;
		unsigned int getLen() const;

	protected:
		char* buffer;
		unsigned int len;
};

class ElevatorControllerMessage : public Message {
  public:
    ElevatorControllerMessage(char type, char ecID, unsigned int len=2);
    ElevatorControllerMessage(const char* buffer, unsigned int len);
    ~ElevatorControllerMessage();
    
    char getType() const { return this->type; }
    char getECID() const { return this->ecID; }
    
  private:
    char type;
    char ecID;
};

class StatusResponseMessage : public ElevatorControllerMessage {
	public:
		StatusResponseMessage(char ecID, char position, char destination, char speed, char numHallCalls, const char* hallCalls);
		StatusResponseMessage(const char* buffer, unsigned int len);
		~StatusResponseMessage();

		char getPosition() 			const	{ return this->position; }
		char getDestination() 	const { return this->destination; }
		char getSpeed() 				const { return this->speed; }
		char getNumHallCalls()	const { return this->numHallCalls; }
		char* getHallCalls() 		const;

	private:
			char id, position, destination, speed, numHallCalls;
			char* hallCalls;
};

class HallCallAssignmentMessage : public Message {
  public:
    HallCallAssignmentMessage(char floor, char direction);
    HallCallAssignmentMessage(const char* buffer);
    ~HallCallAssignmentMessage();
    
    char getFloor()     const { return this->floor; };
    char getDirection() const { return this->direction; };
    
  private:
    char floor;
    char direction;
};

class StatusRequestMessage : public Message {
  public:
    StatusRequestMessage();
    ~StatusRequestMessage();
};

class RegisterWithGDMessage : public ElevatorControllerMessage {
  public:
    RegisterWithGDMessage(char ecID);
    ~RegisterWithGDMessage();
};

class RegistrationAckMessage : public Message {
  public:
    RegistrationAckMessage();
    ~RegistrationAckMessage();
};

class ErrorMessage : public ElevatorControllerMessage {
  public:
    ErrorMessage(char ecID, char errorCode=0, char detailsLength=0, char* details=NULL);
    ErrorMessage(const char* buffer, unsigned int len);
    ~ErrorMessage();
  
    char getErrorCode() const;
    char getDetailLength() const;
    char* getDetails() const;
    
  private:
    char errorCode;
    char detailsLength;
    char* details;
};

#endif
