#ifndef ELEVATOR_CONTROLLER_VIEW_HPP
#define ELEVATOR_CONTROLLER_VIEW_HPP

#include "ElevatorCommon.hpp"

class ElevatorController;
class ElevatorControllerStatus;

class ElevatorControllerView {
	public:		
		virtual void receiveStatus(ElevatorControllerStatus* status) = 0;
		
		void setController(ElevatorController* ec) {
			this->ec = ec;
		}
		
		ElevatorController* ec;
};

#endif
