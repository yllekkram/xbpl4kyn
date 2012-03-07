#include "UnitTest++.h"

#include "Message.hpp"

SUITE(Message) {
	TEST(CONSTRUCTOR) {
		char data[] 	= {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
		char backup[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
		int len = 8;

		Message* m = new Message(data, len);


		CHECK_EQUAL(len, m->getLen());
		CHECK_ARRAY_EQUAL(data, m->getBuffer(), len);

		delete m;
		/* The char array should have been copied, so the above
		 * delete statement should not have affected the data 
		 * array. */
		CHECK_ARRAY_EQUAL(data, backup, len);
	}
}
