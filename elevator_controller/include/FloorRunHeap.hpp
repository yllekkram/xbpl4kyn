#ifndef FLOOR_RUN_HEAP_HPP
#define FLOOR_RUN_HEAP_HPP

#include <algorithm>
#include <functional>
#include <vector>

class EmptyHeapException : std::exception {};

template <class T>
class MaxHeap {
	public:
		MaxHeap()
			: data() {}
		MaxHeap<T>(const MaxHeap<T>& rhs);
		~MaxHeap(){}

		void push(T item) {
			this->data.push_back(item);
			push_heap(this->data.begin(), this->data.end());
		}

		T pop() {
			T temp = this->peek();

			pop_heap(this->data.begin(), this->data.end());
			this->data.pop_back();

			return temp;
		}

		T peek() const {
			if (this->getSize() == 0)
				throw EmptyHeapException();

			return this->data.front();
		}

		int getSize() const {
			return this->data.size();
		}

	private:
		std::vector<T> data;
};

template <class T>
class MinHeap {
	public:
		MinHeap() : data(), comp() {}
		~MinHeap() {}
		
		void push(T item) {
			this->data.push_back(item);
			push_heap(this->data.begin(), this->data.end(), comp);
		}
		
		T pop() {
			T temp = this->peek();
			
			pop_heap(this->data.begin(),this->data.end(), comp);
			this->data.pop_back();
			
			return temp;
		}
		
		T peek() const {
			if (this->getSize() == 0)
				throw EmptyHeapException();
			
			return this->data.front();
		}
		
		int getSize() const {
			return this->data.size();
		}
		
	private:
		std::vector<T> data;
		std::greater<T> comp;
};

class UpwardFloorRunHeap {
	public:
		UpwardFloorRunHeap(char direction);
		~UpwardFloorRunHeap();
	
		char peek() const;
		char pop();
		
		void pushFloorRequest(char dest);
		void pushHallCall(char dest);
		
		char* getHallCalls() const;
	
	private:
		char direction;
		MinHeap<char> floorRequestHeap;
		MinHeap<char> hallCallHeap;
};

#endif
