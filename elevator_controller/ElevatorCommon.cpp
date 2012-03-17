#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "ElevatorCommon.hpp"
#include "Exception.hpp"

void Die(std::string mess) {
	perror(mess.c_str());
	throw Exception(mess.c_str());
}

std::ostream& printBuffer(char* buffer, unsigned int len, std::ostream& out) {
	out << "[ ";
	for (unsigned int i = 0; i < len; i++) {
		out << (int)buffer[i] << " ";
	}
	out << "]";

	return out;
}
