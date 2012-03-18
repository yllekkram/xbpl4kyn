#include <algorithm>

#include "ElevatorCommon.hpp"
#include "Exception.hpp"
#include "Message.hpp"

Message::Message()
  : buffer(NULL), len(-1)
{}

Message::Message(unsigned int len)
	: buffer(new char[BUFFSIZE]), len(len)
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

TypedMessage::TypedMessage(char type, unsigned int len)
  : Message(len+1), type(type)
{
  this->buffer[0] = type;
  this->buffer[len++] = MESSAGE_TERMINATOR;
}

TypedMessage::TypedMessage(const char* buffer, unsigned int len)
  : Message(buffer, len)
{
  this->type = buffer[0];
}

ElevatorControllerMessage::ElevatorControllerMessage(char type, char ecID, unsigned int len)
  : TypedMessage(type, len), ecID(ecID)
{
  this->buffer[1] = ecID;
}

ElevatorControllerMessage::ElevatorControllerMessage(const char* buffer, unsigned int len)
  :TypedMessage(buffer, len)
{
  if (len < 2)
    throw MessageException();
    
  this->ecID = buffer[1];
}

StatusResponseMessage::StatusResponseMessage(char ecID, char position, char destination, char speed, char numHallCalls, const char* hallCalls)
	: ElevatorControllerMessage(STATUS_RESPONSE, ecID), position(position), destination(destination), speed(speed), numHallCalls(numHallCalls), hallCalls(new char[numHallCalls])
{
	std::copy(hallCalls, hallCalls + numHallCalls, this->hallCalls);

	/* Calculate required buffer size */
	this->len +=	1 /* Position */
							+ 1 /* Destination */
							+ 1 /* Speed */
							+ 1 /* Number of Call Registrations */
							+ numHallCalls; /* Call Registrations */

	/* Initialize the buffer */
	this->buffer[2] = this->position;
	this->buffer[3] = this->destination;
	this->buffer[4] = this->speed;
	this->buffer[5] = this->numHallCalls;
	std::copy(hallCalls,
						hallCalls + numHallCalls,
						this->buffer + 6);
}

StatusResponseMessage::StatusResponseMessage(const char* buffer, unsigned int len)
	: ElevatorControllerMessage(buffer, len)
{
	// A status message must be at least 6 bytes long
	if (len < 6 || buffer[0] != STATUS_RESPONSE)
		throw MessageException();

	this->position 			= this->buffer[2];
	this->destination 	= this->buffer[3];
	this->speed 				= this->buffer[4];
	this->numHallCalls 	= this->buffer[5];

	this->hallCalls = new char[numHallCalls];
	std::copy(this->buffer + 6,
						this->buffer + 6 + this->numHallCalls,
						this->hallCalls);
}

StatusResponseMessage::~StatusResponseMessage() {
	delete this->hallCalls;
}

char* StatusResponseMessage::getHallCalls()	const {
	char* newCopy = new char[this->getNumHallCalls()];
	std::copy(this->hallCalls, this->hallCalls + this->getNumHallCalls(), newCopy);

	return newCopy;
}

HallCallAssignmentMessage::HallCallAssignmentMessage(char floor, char direction)
  : TypedMessage(HALL_CALL_ASSIGNMENT, 3), floor(floor), direction(direction)
{
  this->buffer[1] = floor;
  this->buffer[2] = direction;
}

HallCallAssignmentMessage::HallCallAssignmentMessage(const char* buffer)
  : TypedMessage(buffer, 3)
{
  if (buffer[0] != HALL_CALL_ASSIGNMENT)
    throw MessageException();
  
  this->floor = this->buffer[1];
  this->direction = this->buffer[2];
}

StatusRequestMessage::StatusRequestMessage()
  : TypedMessage(STATUS_REQUEST)
{}

RegisterWithGDMessage::RegisterWithGDMessage(char ecID)
  : ElevatorControllerMessage(REGISTER_MESSAGE, ecID)
{
	std::cout << "GD Registration: ";
	printBuffer(this->buffer, this->len);
	std::cout << std::endl;
}

RegistrationAckMessage::RegistrationAckMessage()
  : TypedMessage(REGISTRATION_ACK)
{}

ErrorMessage::ErrorMessage(char ecID, char errorCode, char detailsLength, char* details)
  : ElevatorControllerMessage(ERROR_RESPONSE, ecID), errorCode(errorCode), detailsLength(detailsLength)
{
  this->len =   1 /* Message Type */
              + 1 /* ID */
              + 1 /* Error Code */
              + 1 /* Length of details */
              + detailsLength;
              
  this->buffer = new char[this->len];
  
  this->buffer[2] = this->errorCode;
  this->buffer[3] = this->detailsLength;
  
  this->details = new char[this->detailsLength];
  std::copy(details, details + detailsLength, this->details);
  std::copy(details, details + detailsLength, this->buffer + 4);
}
ErrorMessage::ErrorMessage(const char* buffer, unsigned int len)
 : ElevatorControllerMessage(buffer, len)
{
  if ((len < 4) || buffer[0] != ERROR_RESPONSE)
    throw MessageException();
  
  this->errorCode = this->buffer[2];
  this->detailsLength = this->buffer[3];
}

ErrorMessage::~ErrorMessage() {
  delete this->details;
}

char ErrorMessage::getErrorCode() const {
  return this->errorCode;
}

char ErrorMessage::getDetailLength() const {
  return this->detailsLength;
}
  
char* ErrorMessage::getDetails() const {
  char* newCopy = new char[this->detailsLength];
  std::copy(this->details, this->details + this->detailsLength, newCopy);
  return newCopy;
}

GUIRegistrationAckMessage::GUIRegistrationAckMessage()
  : TypedMessage(GUI_REGISTRATION_ACK)
{}

FloorSelectionMessage::FloorSelectionMessage(char floor)
  : TypedMessage(FLOOR_SELECTION_MESSAGE, 2), floor(floor)
{
  this->buffer[1] = floor;
}

NewDirectionMessage::NewDirectionMessage(char ecID, char direction)
  : ElevatorControllerMessage(ecID, 3), direction(direction)
{
  this->buffer[2] = direction;
}

NewDirectionMessage::NewDirectionMessage(const char* buffer, unsigned int len)
  : ElevatorControllerMessage(buffer, len)
{
  this->direction = this->buffer[2];
}

GUIRegistrationMessage::GUIRegistrationMessage(char ecID)
  : ElevatorControllerMessage(GUI_REGISTER_MESSAGE, ecID)
{}
