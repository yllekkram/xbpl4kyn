#ifndef ELEVATOR_CONTROLLER_VIEW_HPP
#define ELEVATOR_CONTROLLER_VIEW_HPP

#include "ElevatorCommon.hpp"

class ElevatorController;
class ElevatorControllerStatus;

class ElevatorControllerView {
	public:		
    virtual ~ElevatorControllerView() {}
		virtual void receiveStatus(ElevatorControllerStatus* status) = 0;
		
		virtual void setController(ElevatorController* ec) {
			this->ec = ec;
		}
		
		ElevatorController* getEC() const {
			return this->ec;
		}
		
	private:
		
		ElevatorController* ec;
};

#endif
