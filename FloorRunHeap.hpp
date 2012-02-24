#ifndef FLOOR_RUN_HEAP_HPP
#define FLOOR_RUN_HEAP_HPP

#include <algorithm>
#include <vector>

class FloorRunHeap {
	public:
		FloorRunHeap(int direction);
		~FloorRunHeap();
	
		FloorRun checkTop() const;
		FloorRun removeTop();
		
		void addFloorRequest(char dest);
		void addHallCall(const HallCall& dest);
		
		HallCall* getHallCalls() const;
	
	private:
		vecotr<char> floorRequestHeap;
		vector<char> hallCallHeap;
};

#endif
