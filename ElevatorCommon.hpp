#ifndef ELEVATOR_COMMON_HPP
#define ELEVATOR_COMMON_HPP

#include <string>

// Common Values
#define MAXPENDING 5 /* Max connection requests */
#define BUFFSIZE 100

// Types for messages received from the GroupDispatcher
#define MAX_GD_REQUEST_SIZE 2
#define STATUS_REQUEST 1
#define HALL_CALL_REQUEST 2

// Types for messages sent from the ElevatorController to the GroupDispatcher
#define REGISTER_MESSAGE 0

#define STATUS_RESPONSE 1
#define ERROR_RESPONSE 2

// Common functions
void Die(std::string);

#endif
