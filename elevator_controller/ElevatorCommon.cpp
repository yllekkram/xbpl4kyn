#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "ElevatorCommon.hpp"

void Die(std::string mess) {
	perror(mess.c_str());
	throw std::exception();
}

std::ostream& printBuffer(char* buffer, unsigned int len, std::ostream& out) {
	out << "[ ";
	for (int i = 0; i < len; i++) {
		out << (int)buffer[i] << " ";
	}
	out << "]";

	return out;
}
