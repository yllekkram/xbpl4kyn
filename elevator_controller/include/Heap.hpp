#ifndef HEAP_HPP
#define HEAP_HPP

#include <algorithm>
#include <functional>
#include <vector>

class EmptyHeapException : std::exception {};

template <class T>
class Heap {
  public:
    Heap() : data(new std::vector<T>()) {}
    virtual ~Heap() { delete this->data; }
    
    virtual void push(T item) = 0;
		virtual void pushVector(std::vector<T>& items) = 0;
    virtual T pop() = 0;
    virtual T peek() {
      if (this->getSize() == 0)
				throw EmptyHeapException();

			return this->data->front();
    }
    
    int getSize() const {
      return this->data->size();
    }
    
    std::vector<T>* getVector() const {
      return new std::vector<T>(*(this->data));
    }
    
  protected:
    std::vector<T>* data;
};

template <class T>
class MaxHeap : public Heap<T> {
	public:
		MaxHeap() : Heap<T>() {}
		MaxHeap<T>(const MaxHeap<T>& rhs);
		virtual ~MaxHeap(){}

		void push(T item) {
			this->data->push_back(item);
			push_heap(this->data->begin(), this->data->end());
		}

		void pushVector(std::vector<T>& items) {
			typename std::vector<T>::iterator it;
			for (it = items.begin(); it < items.end(); ++it) {
				this->data->push_back(*it);
			}
			make_heap(this->data->begin(), this->data->end());
		}

		T pop() {
			T temp = this->peek();

			pop_heap(this->data->begin(), this->data->end());
			this->data->pop_back();

			return temp;
		}
};

template <class T>
class MinHeap : public Heap<T> {
	public:
		MinHeap() : Heap<T>(), comp() {}
		virtual ~MinHeap() {}
		
		void push(T item) {
			this->data->push_back(item);
			push_heap(this->data->begin(), this->data->end(), comp);
		}

		void pushVector(std::vector<T>& items) {
			typename std::vector<T>::iterator it;
			for (it = items.begin(); it < items.end(); ++it) {
				this->data->push_back(*it);
			}
			make_heap(this->data->begin(), this->data->end(), comp);
		}

		T pop() {
			T temp = this->peek();
			
			pop_heap(this->data->begin(),this->data->end(), comp);
			this->data->pop_back();
			
			return temp;
		}
  
  private:
    std::greater<T> comp;
};

class FloorRunHeap {
  public:
    FloorRunHeap(char direction) : direction(direction) {}
    virtual ~FloorRunHeap() {}
    
    virtual char peek() const = 0;
    virtual char pop() = 0;
    virtual void pushFloorRequest(char dest) = 0;
    virtual void pushHallCall(char dest) = 0;
    
    virtual int getSize() const {
      return floorRequestHeap->getSize() + hallCallHeap->getSize();
    }
    
    virtual std::vector<char>* getHallCalls() const {
      return this->hallCallHeap->getVector();
    }

		virtual std::vector<char>* getFloorRequests() const {
			return this->floorRequestHeap->getVector();
		}
  
  protected:
    char direction;
    Heap<char>* floorRequestHeap;
    Heap<char>* hallCallHeap;
};

class UpwardFloorRunHeap : public FloorRunHeap {
	public:
		UpwardFloorRunHeap();
		virtual ~UpwardFloorRunHeap();
	
		virtual char peek() const;
		virtual char pop();
		
		void pushFloorRequest(char dest);
		void pushFloorRequestVector(std::vector<char>& dests);
		void pushHallCall(char dest);
		void pushHallCallVector(std::vector<char>& dests);
};

class DownwardFloorRunHeap : public FloorRunHeap {
	public:
		DownwardFloorRunHeap();
		virtual ~DownwardFloorRunHeap();
	
		virtual char peek() const;
		virtual char pop();
		
		void pushFloorRequest(char dest);
		void pushFloorRequestVector(std::vector<char>& dests);
		void pushHallCall(char dest);
		void pushHallCallVector(std::vector<char>& dests);
};

#endif
