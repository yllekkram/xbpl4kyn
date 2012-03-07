#include "ElevatorCommon.hpp"
#include "Heap.hpp"

UpwardFloorRunHeap::UpwardFloorRunHeap()
	: FloorRunHeap(HALL_CALL_DIRECTION_UP)
{
  this->floorRequestHeap = new MinHeap<char>();
  this->hallCallHeap = new MinHeap<char>();
}

UpwardFloorRunHeap::~UpwardFloorRunHeap() {
  delete this->floorRequestHeap;
  delete this->hallCallHeap;
}

char UpwardFloorRunHeap::peek() const {
  int frSize = floorRequestHeap->getSize();
  int hcSize = hallCallHeap->getSize();
  
  if (frSize == 0 && hcSize == 0) {
    throw EmptyHeapException();
  }
  else if (frSize == 0) {
    return hallCallHeap->peek();
  }
  else if (hcSize == 0) {
    return floorRequestHeap->peek();
  }
  
  char frTop = floorRequestHeap->peek();
	char hcTop = hallCallHeap->peek();
	
	return (frTop < hcTop) ? frTop : hcTop;
}

char UpwardFloorRunHeap::pop() {
  int frSize = floorRequestHeap->getSize();
  int hcSize = hallCallHeap->getSize();
  
  if (frSize == 0 && hcSize == 0) {
    throw EmptyHeapException();
  }
  else if (frSize == 0) {
    return hallCallHeap->pop();
  }
  else if (hcSize == 0) {
    return floorRequestHeap->pop();
  }
  
	char frTop = floorRequestHeap->peek();
	char hcTop = hallCallHeap->peek();
	
	return (frTop < hcTop) ? floorRequestHeap->pop() : hallCallHeap->pop();
}

void UpwardFloorRunHeap::pushFloorRequest(char dest) {
	floorRequestHeap->push(dest);
}

void UpwardFloorRunHeap::pushHallCall(char dest) {
	hallCallHeap->push(dest);
}

DownwardFloorRunHeap::DownwardFloorRunHeap()
	: FloorRunHeap(HALL_CALL_DIRECTION_DOWN)
{
  this->floorRequestHeap = new MaxHeap<char>();
  this->hallCallHeap = new MaxHeap<char>();
}

DownwardFloorRunHeap::~DownwardFloorRunHeap() {
  delete this->floorRequestHeap;
  delete this->hallCallHeap;
}

char DownwardFloorRunHeap::peek() const {
  int frSize = floorRequestHeap->getSize();
  int hcSize = hallCallHeap->getSize();
  
  if (frSize == 0 && hcSize == 0) {
    throw EmptyHeapException();
  }
  else if (frSize == 0) {
    return hallCallHeap->peek();
  }
  else if (hcSize == 0) {
    return floorRequestHeap->peek();
  }
  
  char frTop = floorRequestHeap->peek();
	char hcTop = hallCallHeap->peek();
	
	return (frTop > hcTop) ? frTop : hcTop;
}

char DownwardFloorRunHeap::pop() {
  int frSize = floorRequestHeap->getSize();
  int hcSize = hallCallHeap->getSize();
  
  if (frSize == 0 && hcSize == 0) {
    throw EmptyHeapException();
  }
  else if (frSize == 0) {
    return hallCallHeap->pop();
  }
  else if (hcSize == 0) {
    return floorRequestHeap->pop();
  }
  
	char frTop = floorRequestHeap->peek();
	char hcTop = hallCallHeap->peek();
	
	return (frTop > hcTop) ? floorRequestHeap->pop() : hallCallHeap->pop();
}

void DownwardFloorRunHeap::pushFloorRequest(char dest) {
	floorRequestHeap->push(dest);
}

void DownwardFloorRunHeap::pushHallCall(char dest) {
	hallCallHeap->push(dest);
}
