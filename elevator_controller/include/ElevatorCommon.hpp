#ifndef ELEVATOR_COMMON_HPP
#define ELEVATOR_COMMON_HPP

#include <string>

// Common Values
#define MAXPENDING 5 /* Max connection requests */
#define BUFFSIZE 100
#define MAX_FLOORS 255 /* Currently restricted by size of char */

// Types for messages received from the GroupDispatcher
#define MAX_GD_REQUEST_SIZE 3

#define REGISTRATION_ACK 1
#define STATUS_REQUEST 2
#define HALL_CALL_ASSIGNMENT 3

#define HALL_CALL_DIRECTION_UP 1
#define HALL_CALL_DIRECTION_DOWN 2

// Types for messages sent from the ElevatorController to the GroupDispatcher
#define REGISTER_MESSAGE 0

#define STATUS_RESPONSE 1
#define ERROR_RESPONSE 2

// Types for EC to GUI
#define GUI_REGISTER_MESSAGE 0

// Common functions
void Die(std::string);

#endif
