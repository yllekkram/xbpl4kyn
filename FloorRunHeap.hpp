#ifndef FLOOR_RUN_HEAP_HPP
#define FLOOR_RUN_HEAP_HPP

#include <functional>
#include <vector>

template <class T>
class Heap {
	public:
		Heap() : data() {}
	
		void push(T item) {
			data.push_back(item);
			push_heap(data.begin(), data.end());
		}

		T pop() {
			pop_heap(data.begin(), data.end());
			T result = data.back();
			data.pop_back();
			return result;
		}

		T peek() const {
			return data.at(0);
		}
	private:
		std::vector<T> data;
};

class FloorRunHeap {
	public:
		FloorRunHeap(char direction);
		~FloorRunHeap();
	
		char checkTop() const;
		char removeTop();
		
		void addFloorRequest(char dest);
		void addHallCall(char dest);
		
		char* getHallCalls() const;
	
	private:
		char direction;
		Heap<char> floorRequestHeap;
		Heap<char> hallCallHeap;
};

#endif
