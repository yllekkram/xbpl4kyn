#include <cstdlib>
#include <string>

#include "ElevatorCommon.hpp"

void Die(std::string mess) { perror(mess.c_str()); exit(1); }
