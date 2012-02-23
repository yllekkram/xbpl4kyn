class FloorRunHeap {
	public:
		FloorRunHeap(int direction);
	
	FloorRun checkTop() const;
	FloorRun removeTop();
	
	addFloorRequest(int dest);
	addHallCall(HallCall dest);
	
	HallCall* getHallCalls() const;
	
	private:
		int* floorRequestHeap;
		int* hallCallHeap;
};
