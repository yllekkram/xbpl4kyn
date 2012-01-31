#ifndef ELEVATOR_CONTROLER_H
#define ELEVATOR_CONTROLLER_H

#include <stdio.h>
#include <string>

// Types for messages received from the GroupDispatcher
#define STATUS_REQUEST 1

// Types for messages sent from the ElevatorController to the GroupDispatcher
#define REGISTER_MESSAGE 0

#define STATUS_RESPONSE 1
#define ERROR_RESPONSE 2

// Common functions
void Die(std::string mess) { perror(mess.c_str()); exit(1); }

#endif
