#ifndef FER_CIRCULARARRAY
#define FER_CIRCULARARRAY

#include <string>

template<class T>
struct CircularArray {
  T* arr;
  size_t begin;
  size_t end;
  size_t capacity;

  size_t size;

  CircularArray(size_t capacity_) {
    capacity = capacity_;
    arr = new T[capacity];
    begin = end = size = 0;
  }
  ~CircularArray() {
    delete [] arr;
  }

  T& operator[] (const size_t i) {
    size_t t = begin + i;
    if (t >= capacity) return arr[t-capacity];
    return arr[t];
  }

  void push_back(const T& a) {
    arr[end++] = a;
    ++size;
    if (end >= capacity) end -= capacity;
  }

  void pop_front() {
    ++begin;
    --size;
    if (begin >= capacity) begin -= capacity;
  }

  size_t getSize() {
    return size;
  }

  bool operator == (const std::string& s) {
    if (size != s.size()) return false;
    
    for (size_t i = 0; i < size; ++i) {
      if (s[i] != (*this)[i]) {
	return false;
      }
    }
    
    return true;
  }
};


#endif
