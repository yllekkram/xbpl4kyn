#include "UnitTest++.h"

#include "fixtures/MessageFixtures.hpp"

#include "ElevatorCommon.hpp"
#include "Message.hpp"

SUITE(Message) {
	TEST(Constructor) {
		char data[] 	= {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
		char backup[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
		int len = 8;

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

SUITE(StatusMessage) {
	TEST_FIXTURE(StatusMessageFixture, ConstructorWithaluesGiven) {
		CHECK_EQUAL(9, message.getLen());
	}

	TEST(ConstructorWithBufferGiven) {
		char buffer[] = {STATUS_RESPONSE, 4, 3, 2, 10, 3, 2, 4, 5};

		StatusMessage* message = new StatusMessage(buffer, 9);

		CHECK_EQUAL(9, message->getLen());

		delete message;
	}

}
