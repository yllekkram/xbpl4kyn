#include "ElevatorCommon.hpp"
#include "Message.hpp"

const char HALL_CALLS[] = {2, 4, 5};

struct StatusMessageFixture {
	StatusMessageFixture()
		: message(
			/* Any numerical literals below are arbitrary */
			4,	/* ID */
			3, 	/* Position */
			2,	/* Destination */
			10,	/* Speed */
			3,	/* Number of call registrations */
			HALL_CALLS
		)
	{}

	StatusMessage message;
};
