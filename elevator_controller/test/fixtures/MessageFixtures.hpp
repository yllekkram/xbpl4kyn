#include "ElevatorCommon.hpp"
#include "Message.hpp"

const char NUM_CALLS = 3;
const char CALLS[] = {'a', 'b', 'c'};

struct StatusMessageFixture {
	StatusMessageFixture()
		: message(
			/* Any numerical literals below are arbitrary */
			4,	/* ID */
			3, 	/* Position */
			2,	/* Destination */
			10,	/* Speed */
			NUM_CALLS,
			CALLS
		)
	{}

	StatusResponseMessage message;
};
