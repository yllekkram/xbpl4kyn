#include "UnitTest++.h"

#include "FloorRunHeap.hpp"
#include "fixtures/HeapFixtures.hpp"

SUITE(MaxHeap) {
	TEST_FIXTURE(EmptyMaxHeapFixture, Constructor) {
		CHECK_EQUAL(0, heap.getSize());
	}

	TEST_FIXTURE(EmptyMaxHeapFixture, PeekEmptyMaxHeap) {
		CHECK_THROW(heap.peek(), EmptyHeapException);
	}

	TEST_FIXTURE(EmptyMaxHeapFixture, PushAddOneItem) {
		heap.push(3);

		CHECK_EQUAL(1, heap.getSize());
		CHECK_EQUAL(3, heap.peek());
	}

	TEST_FIXTURE(EmptyMaxHeapFixture, PushThreeItems) {
		heap.push(5);
		heap.push(7);
		heap.push(6);

		CHECK_EQUAL(3, heap.getSize());
		CHECK_EQUAL(7, heap.peek());
	}

	TEST_FIXTURE(EmptyMaxHeapFixture, PopEmptyMaxHeap) {
		CHECK_THROW(heap.pop(), EmptyHeapException);
		CHECK_EQUAL(0, heap.getSize());
	}

	TEST_FIXTURE(PopulatedMaxHeapFixture, PopMaxHeapOnce) {
		int initialSize = heap.getSize();

		CHECK_EQUAL(7, heap.pop());
		CHECK_EQUAL(initialSize - 1, heap.getSize());
		// The top element should be the second-greatest element
		CHECK_EQUAL(6, heap.peek());
	}
}

SUITE(MinHeap) {
	TEST_FIXTURE(EmptyMinHeapFixture, Constructor) {
		CHECK_EQUAL(0, heap.getSize());
	}
	
	TEST_FIXTURE(EmptyMinHeapFixture, PeekEmptyMinHeap) {
		CHECK_THROW(heap.peek(), EmptyHeapException);
	}
		
	TEST_FIXTURE(EmptyMinHeapFixture, PushAddOneItem) {
		heap.push(3);

		CHECK_EQUAL(1, heap.getSize());
		CHECK_EQUAL(3, heap.peek());
	}

	TEST_FIXTURE(EmptyMinHeapFixture, PushThreeItems) {
		heap.push(5);
		heap.push(7);
		heap.push(6);

		CHECK_EQUAL(3, heap.getSize());
		CHECK_EQUAL(5, heap.peek());
	}

	TEST_FIXTURE(EmptyMinHeapFixture, PopEmptyMinHeap) {
		CHECK_THROW(heap.pop(), EmptyHeapException);
		CHECK_EQUAL(0, heap.getSize());
	}

	TEST_FIXTURE(PopulatedMinHeapFixture, PopMinHeapOnce) {
		int initialSize = heap.getSize();

		CHECK_EQUAL(2, heap.pop());
		CHECK_EQUAL(initialSize - 1, heap.getSize());
		// The top element should be the second-greatest element
		CHECK_EQUAL(3, heap.peek());
	}
}

SUITE(FloorRunHeap) {
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PeekWhenBothEmpty) {
    CHECK_THROW(heap.peek(), EmptyHeapException);    
  }
  
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PeekWhenHallCallsEmpty) {
    heap.pushFloorRequest(5);
    
    CHECK_EQUAL(5, heap.peek());
  }
  
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PeekWhenFloorRequestsEmpty) {
    heap.pushHallCall(4);
    
    CHECK_EQUAL(4, heap.peek());
  }
  
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PeekWhenHallCallLower) {
    heap.pushFloorRequest(8);
    heap.pushHallCall(5);
    
    CHECK_EQUAL(5, (int)heap.peek());
  }
  
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PeekWhenFloorRequestLower) {
    heap.pushFloorRequest(4);
    heap.pushHallCall(7);
    
    CHECK_EQUAL(4, (int)heap.peek());
  }
  
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PopWhenEmpty) {
    CHECK_THROW(heap.pop(), EmptyHeapException);
  }
  
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PopWhenHallCallsEmpty) {
    heap.pushFloorRequest(5);
    
    CHECK_EQUAL(5, (int)heap.pop());
  }
  
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PopWhenFloorRequestsEmpty) {
    heap.pushHallCall(3);
    
    CHECK_EQUAL(3, (int)heap.pop());
  }
  
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PopWhenHallCallLower) {
    heap.pushFloorRequest(6);
    heap.pushHallCall(2);
    
    CHECK_EQUAL(2, (int)heap.pop());
  }
  
  TEST_FIXTURE(EmptyUpwardFloorRunHeapFixture, PopWhenFloorRequestLower) {
    heap.pushFloorRequest(3);
    heap.pushHallCall(7);
    
    CHECK_EQUAL(3, (int)heap.pop());
  }
}
