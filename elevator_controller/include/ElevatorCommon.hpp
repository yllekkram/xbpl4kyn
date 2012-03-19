#ifndef ELEVATOR_COMMON_HPP
#define ELEVATOR_COMMON_HPP

#include <iostream>
#include <string>

// Common Values
#define MAXPENDING 5 /* Max connection requests */
#define BUFFSIZE 50
#define MAX_FLOORS 25 /* Currently restricted by size of char */
#define MESSAGE_TERMINATOR 255

// Types for messages received from the GroupDispatcher
#define MAX_GD_REQUEST_SIZE 3

#define REGISTRATION_ACK 1
#define STATUS_REQUEST 2
#define HALL_CALL_ASSIGNMENT 3

#define HALL_CALL_DIRECTION_UP 1
#define HALL_CALL_DIRECTION_DOWN 2

// Types for messages sent from the ElevatorController to the GroupDispatcher
#define REGISTER_MESSAGE 4
#define STATUS_RESPONSE 5
#define ERROR_RESPONSE 6

// Types for GUI to EC
#define MAX_GUI_REQUEST_SIZE 2

#define GUI_REGISTRATION_ACK 10
#define FLOOR_SELECTION_MESSAGE 11
#define OPEN_DOOR_REQUEST 12
#define CLOSE_DOOR_REQUEST 13
#define EMERGENGY_STOP_MESSAGE 14

// Types for EC to GUI
#define GUI_REGISTER_MESSAGE 15
#define FLOOR_REACHED_NOTIFICATION 16
#define NEW_DIRECTION_NOTIFICATION 17

// Common functions
void Die(std::string);
std::ostream& printBuffer(char* buffer, unsigned int len, std::ostream& out=std::cout);

#endif
