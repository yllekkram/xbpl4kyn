#include "ElevatorCommon.hpp"
#include "Heap.hpp"

struct MaxHeapFixture {
	MaxHeapFixture() : heap() {}
	~MaxHeapFixture() {}

	MaxHeap<int> heap;
};

struct EmptyMaxHeapFixture : public MaxHeapFixture {
	EmptyMaxHeapFixture() : MaxHeapFixture() {}
};

struct PopulatedMaxHeapFixture : public MaxHeapFixture {
	PopulatedMaxHeapFixture() : MaxHeapFixture() {
		heap.push(3);
		heap.push(5);
		heap.push(7);
		heap.push(6);
		heap.push(4);
		heap.push(2);
	}
};

struct MinHeapFixture {
	MinHeapFixture() : heap() {}
	~MinHeapFixture() {}
	
	MinHeap<int> heap;
};

struct EmptyMinHeapFixture : public MinHeapFixture {
	EmptyMinHeapFixture() : MinHeapFixture() {}
};

struct PopulatedMinHeapFixture : public MinHeapFixture {
	PopulatedMinHeapFixture() : MinHeapFixture() {
		heap.push(3);
		heap.push(5);
		heap.push(7);
		heap.push(6);
		heap.push(4);
		heap.push(2);
	}
};

struct UpwardFloorRunHeapFixture {
  UpwardFloorRunHeapFixture() : heap() {}
    
  UpwardFloorRunHeap heap;
};

struct EmptyUpwardFloorRunHeapFixture : public UpwardFloorRunHeapFixture {
  EmptyUpwardFloorRunHeapFixture() : UpwardFloorRunHeapFixture() {}
};

struct DownwardFloorRunHeapFixture {
  DownwardFloorRunHeapFixture() : heap() {}
    
  DownwardFloorRunHeap heap;
};

struct EmptyDownwardFloorRunHeapFixture : public DownwardFloorRunHeapFixture {
  EmptyDownwardFloorRunHeapFixture() : DownwardFloorRunHeapFixture() {}
};
