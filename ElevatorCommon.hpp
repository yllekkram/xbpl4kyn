#ifndef ELEVATOR_CONTROLLER_HPP
#define ELEVATOR_CONTROLLER_HPP

#include <string>

// Common Values
#define MAXPENDING 5 /* Max connection requests */
#define BUFFSIZE 32

// Types for messages received from the GroupDispatcher
#define STATUS_REQUEST 1

// Types for messages sent from the ElevatorController to the GroupDispatcher
#define REGISTER_MESSAGE 0

#define STATUS_RESPONSE 1
#define ERROR_RESPONSE 2

// Common functions
void Die(std::string);

#endif
