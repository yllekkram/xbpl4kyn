#include <algorithm>

#include "FloorRunHeap.hpp"

FloorRunHeap::FloorRunHeap(char direction)
	: direction(direction), floorRequestHeap(), hallCallHeap()
{}

char FloorRunHeap::checkTop() const {
	char frTop = floorRequestHeap.peek();
	char hcTop = hallCallHeap.peek();
	
	return (frTop > hcTop) ? frTop : hcTop;
}

char FloorRunHeap::removeTop() {
	char frTop = floorRequestHeap.peek();
	char hcTop = hallCallHeap.peek();
	
	if (frTop > hcTop) {
		return floorRequestHeap.pop();
	}
	else {
		return hallCallHeap.pop();
	}
}

void FloorRunHeap::addFloorRequest(char dest) {
	floorRequestHeap.push(dest);
}

void FloorRunHeap::addHallCall(char dest) {
	hallCallHeap.push(dest);
}

