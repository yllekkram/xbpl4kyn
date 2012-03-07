#include <algorithm>

#include "ElevatorCommon.hpp"
#include "Message.hpp"

Message::Message()
	: buffer(NULL), len(-1)
{}

Message::Message(const char* buffer, unsigned int len) 
	: buffer(new char[len]), len(len)
{
	std::copy(buffer, buffer+len, this->buffer);
}

Message::~Message() {
	delete this->buffer;
}

char* Message::getBuffer() const {
	char* copiedBuffer = new char[this->len];
	std::copy(this->buffer, this->buffer + this->len, copiedBuffer);

	return copiedBuffer;
}

unsigned int Message::getLen() const {
	return this->len;
}

StatusMessage::StatusMessage(char id, char position, char destination, char speed, char numHallCalls, const char* hallCalls)
	: Message(), id(id), position(position), destination(destination), speed(speed), numHallCalls(numHallCalls), hallCalls(new char[numHallCalls])
{
	std::copy(hallCalls, hallCalls + numHallCalls, this->hallCalls);

	/* Calculate required buffer size */
	this->len =		1	/* Message Type */
							+ 1	/* ID */
							+ 1 /* Position */
							+ 1 /* Destination */
							+ 1 /* Speed */
							+ 1 /* Number of Call Registrations */
							+ numHallCalls; /* Call Registrations */

	/* Initialize the buffer */
	this->buffer = new char[len];
	this->buffer[0] = STATUS_RESPONSE;
	this->buffer[1] = this->id;
	this->buffer[2] = this->position;
	this->buffer[3] = this->destination;
	this->buffer[4] = this->speed;
	this->buffer[5] = this->numHallCalls;
	for (int i = 0; i < this->numHallCalls; i++) {
		this->buffer[6+i] = this->hallCalls[i];
	}
}

StatusMessage::StatusMessage(const char* buffer, unsigned int len)
	: Message(buffer, len)
{
	// A status message must be at least 6 bytes long
	if (len < 6 || buffer[0] != STATUS_RESPONSE)
		throw std::exception();

	this->id						= this->buffer[1];
	this->position 			= this->buffer[2];
	this->destination 	= this->buffer[3];
	this->speed 				= this->buffer[4];
	this->numHallCalls 	= this->buffer[5];


	this->hallCalls = new char[numHallCalls];
	for (int i = 0; i < this->numHallCalls; i++) {
		this->hallCalls[i] = this->buffer[6+i];
	}
}

StatusMessage::~StatusMessage() {
	delete this->hallCalls;
}

char* StatusMessage::getHallCalls() 		const {
	char* newCopy = new char[this->len];
	std::copy(newCopy, newCopy + this->len, this->hallCalls);

	return newCopy;
}
