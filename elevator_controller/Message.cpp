#include <algorithm>

#include "Message.hpp"

Message::Message(char* buffer, int len) 
	: buffer(new char(len)), len(len)
{
	std::copy(buffer, buffer + len, this->buffer);
}

Message::~Message() {
	delete this->buffer;
}

char* Message::getBuffer() const {
	char* copiedBuffer = new char(this->len);
	std::copy(this->buffer, this->buffer + this->len, copiedBuffer);

	return copiedBuffer;
}

int Message::getLen() const {
	return this->len;
}
