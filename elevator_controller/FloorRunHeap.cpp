#include "FloorRunHeap.hpp"

UpwardFloorRunHeap::UpwardFloorRunHeap(char direction)
	: direction(direction), floorRequestHeap(), hallCallHeap()
{}

UpwardFloorRunHeap::~UpwardFloorRunHeap() {}

char UpwardFloorRunHeap::peek() const {
  int frSize = floorRequestHeap.getSize();
  int hcSize = hallCallHeap.getSize();
  
  if (frSize == 0 && hcSize == 0) {
    throw EmptyHeapException();
  }
  else if (frSize == 0) {
    return hallCallHeap.peek();
  }
  else if (hcSize == 0) {
    return floorRequestHeap.peek();
  }
  
  char frTop = floorRequestHeap.peek();
	char hcTop = hallCallHeap.peek();
	
	return (frTop < hcTop) ? frTop : hcTop;
}

char UpwardFloorRunHeap::pop() {
  int frSize = floorRequestHeap.getSize();
  int hcSize = hallCallHeap.getSize();
  
  if (frSize == 0 && hcSize == 0) {
    throw EmptyHeapException();
  }
  else if (frSize == 0) {
    return hallCallHeap.pop();
  }
  else if (hcSize == 0) {
    return floorRequestHeap.pop();
  }
  
	char frTop = floorRequestHeap.peek();
	char hcTop = hallCallHeap.peek();
	
	return (frTop < hcTop) ? floorRequestHeap.pop() : hallCallHeap.pop();
}

void UpwardFloorRunHeap::pushFloorRequest(char dest) {
	floorRequestHeap.push(dest);
}

void UpwardFloorRunHeap::pushHallCall(char dest) {
	hallCallHeap.push(dest);
}

