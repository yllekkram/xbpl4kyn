#ifndef CUSTOM_EXCEPTION_HPP
#define CUSTOM_EXCEPTION_HPP

#include <exception>

class Exception : public std::exception {
	public:
		Exception(const char* message) : message(message) {}

		virtual const char* what() { return message; }
	private:
		const char* message;
};

class MessageException : public Exception {
	public:
		MessageException() : Exception("Message Exception") {}
};

#endif
