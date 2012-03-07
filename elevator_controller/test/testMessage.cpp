#include "UnitTest++.h"

#include "fixtures/MessageFixtures.hpp"

#include "ElevatorCommon.hpp"
#include "Message.hpp"

SUITE(Message) {
	TEST(Constructor) {
		char data[] 	= {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
		char backup[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
		unsigned int len = 8;

		Message* m = new Message(data, len);


		CHECK_EQUAL(len, m->getLen());
		char* buffer = m->getBuffer();
		CHECK_ARRAY_EQUAL(data, buffer, len);
		delete buffer;

		delete m;
		/* The char array should have been copied, so the above
		 * delete statement should not have affected the data 
		 * array. */
		CHECK_ARRAY_EQUAL(data, backup, len);
	}
}

SUITE(StatusResponseMessage) {
	TEST(ConstructorWithValuesGiven) {
		StatusResponseMessage message(
			/* Any numerical literals below are arbitrary */
			4,	/* ID */
			3, 	/* Position */
			2,	/* Destination */
			10,	/* Speed */
			NUM_CALLS,	/* Number of call registrations */
			CALLS
		);

		CHECK_EQUAL(9U, message.getLen());
		CHECK_EQUAL(4, message.getId());
		CHECK_EQUAL(3, message.getPosition());
		CHECK_EQUAL(2, message.getDestination());
		CHECK_EQUAL(10, message.getSpeed());
		CHECK_EQUAL(NUM_CALLS, message.getNumHallCalls());
		CHECK_ARRAY_EQUAL(CALLS, message.getHallCalls(), NUM_CALLS);
	}

	TEST(ConstructorWithBufferGiven) {
		char buffer[] = {STATUS_RESPONSE, 4, 3, 2, 10, 3, 'a', 'b', 'c'};

		StatusResponseMessage message(buffer, 9);

		CHECK_EQUAL(9U, message.getLen());
		CHECK_EQUAL(4, message.getId());
		CHECK_EQUAL(3, message.getPosition());
		CHECK_EQUAL(2, message.getDestination());
		CHECK_EQUAL(10, message.getSpeed());
		CHECK_EQUAL(NUM_CALLS, message.getNumHallCalls());
		CHECK_ARRAY_EQUAL(CALLS, message.getHallCalls(), NUM_CALLS);
	}
  
  TEST(ConstructorWithInvalidBufferGiven) {
    char buffer[] = {0, 0, 0, 0, 0, 0};
    buffer[0] = STATUS_RESPONSE + 1;
    
    CHECK_THROW(StatusResponseMessage(buffer, 6), std::exception);
  }
  
  TEST(ConstructWithShortBufferGiven) {
    char buffer[] = {STATUS_RESPONSE, 0};
    
    CHECK_THROW(StatusResponseMessage(buffer, 5), std::exception);
  }

}

SUITE(HallCallAssignmentMessage) {
  TEST(ConstructorWithValuesGiven) {
    HallCallAssignmentMessage message(
      7, /* Destination */
      HALL_CALL_DIRECTION_UP
    );
    
    CHECK_EQUAL(3U, message.getLen());
    CHECK_EQUAL(7, message.getFloor());
    CHECK_EQUAL(HALL_CALL_DIRECTION_UP, message.getDirection());
  }
  
  TEST(ConstructorWithBufferGiven) {
    char buffer[] = {HALL_CALL_ASSIGNMENT, 7, HALL_CALL_DIRECTION_UP};
    
    HallCallAssignmentMessage message(buffer);
    
    CHECK_EQUAL(3U, message.getLen());
    CHECK_EQUAL(7, message.getFloor());
    CHECK_EQUAL(HALL_CALL_DIRECTION_UP, message.getDirection());
  }

  TEST(ConstructorWithInvalidBufferGiven) {
    char buffer[3]; //s = {0, 0, 0};
    buffer[0] = HALL_CALL_ASSIGNMENT + 1;
    
    CHECK_THROW(HallCallAssignmentMessage message(buffer), std::exception);
  }
}

SUITE(StatusRequestMessage) {
  TEST(Constructor) {
    StatusRequestMessage message;
    
    CHECK_EQUAL(1U, message.getLen());
  }
}
