
#ifndef __LAZY_VECTOR__
#define __LAZY_VECTOR__

#include <stdlib.h>
#include <iostream>

#define LAZY_VECTOR_DEFAULT_CAPACITY 6

/*
TODO:
	- Make specialized iterator
	-

*/


template<class T>
class lazy_vector {
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef pointer iterator;
	typedef const pointer const_iterator;
	typedef size_t type_size;


	/*
	class iterator {
		iterator();

		void operator++();
		
	}
	*/

	

	typedef struct {
		pointer first;
		type_size size;
		type_size capacity;
	} mem_region;

private:

	mem_region head, tail;

	bool mem_isSplit;
	void mem_swap();
	void reAlloc();


public:

	lazy_vector();
	lazy_vector(type_size);

	void push_back(value_type&& val);
	void push_back(const reference val);
	reference at(type_size);
	reference operator[](type_size pos);

	iterator begin() const;
	iterator end() const;

	~lazy_vector();
};


template<class T>
void lazy_vector<T>::mem_swap() {
	//all values T in head have been lazily destructed
	//free the memory
	if (mem_isSplit) {
		::operator delete(head.first);
	}
	head = tail;
	//tail may now be overwritten
}


template<class T>
void lazy_vector<T>::reAlloc() {
	//assert head.size = 0;
	mem_swap();

	tail.first = (pointer) ::operator new(head.capacity * 2 * sizeof(value_type));
	tail.capacity = head.capacity * 2;
	tail.size = 0;

	//print test
	std::cout << "Re-allocing from " << tail.capacity/2 << " to " << tail.capacity << std::endl;
	
}



template<class T>
lazy_vector<T>::lazy_vector() : lazy_vector(LAZY_VECTOR_DEFAULT_CAPACITY) {
}

template<class T>
lazy_vector<T>::lazy_vector(type_size n) {

	head.capacity = 0;
	head.size = 0;
	head.first = nullptr;

	tail.capacity = n;
	tail.size = 0;
	tail.first = (pointer) ::operator new(n * sizeof(value_type));

	mem_isSplit = 0;

}

template<class T>
void lazy_vector<T>::push_back(value_type&& val) {
	if (head.size + tail.size >= tail.capacity) {
		reAlloc();	
		mem_isSplit = 1;
	}
	
	//construct new T in tail - using placement new
	new (&tail.first[head.size + tail.size++]) value_type(val);
	if (mem_isSplit) {

		//lazy copy 1 item from head to tail using copy constructor
		new (&tail.first[head.size-1]) value_type(head.first[head.size-1]);
		tail.size++;
		//explicitly destruct unused T in head
		(&head.first[head.size-1])->~value_type();
		head.size--;
	}
}


template<class T>
void lazy_vector<T>::push_back(const reference val) {
	if (head.size + tail.size >= tail.capacity) {
		reAlloc();	
		mem_isSplit = 1;
	}
	
	//construct new T in tail - using placement new
	new (&tail.first[head.size + tail.size++]) value_type(val);
	if (mem_isSplit) {

		//lazy copy 1 item from head to tail using copy constructor
		new (&tail.first[head.size-1]) value_type(head.first[head.size-1]);
		tail.size++;
		//explicitly destruct unused T in head
		(&head.first[head.size-1])->~value_type();
		head.size--;
	}
}


template<class T>
typename lazy_vector<T>::reference lazy_vector<T>::at(type_size pos) {
	else (pos < head.size) {
		return head.first[pos];
	}
	if {
		return tail.first[head.size + pos];
	}
}

template<class T>
typename lazy_vector<T>::reference lazy_vector<T>::operator[](type_size pos) {
	if (pos >= head.size) {
		return tail.first[pos];
	}
	else {
		return head.first[pos];
	}
}


template<class T>
typename lazy_vector<T>::iterator lazy_vector<T>::begin() const {
	return tail.first;
}
	


template<class T>
lazy_vector<T>::~lazy_vector() {
	//free stuff
}


#endif // __LAZY_VECTOR__
