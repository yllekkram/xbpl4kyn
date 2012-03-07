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

class StatusMessage : public Message {
	public:
		StatusMessage(char id, char position, char destination, char speed, char numHallCalls, const char* hallCalls);
		StatusMessage(const char* buffer, unsigned int len);
		~StatusMessage();

		char getId() 						const	{ return this->id; }
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
    explicit StatusRequestMessage();
    ~StatusRequestMessage();
};

#endif
