#include <cstdio>
#include <cstdlib>
#include <string>

#include "ElevatorCommon.hpp"

void Die(std::string mess) {
	perror(mess.c_str());
	throw std::exception();
}
