
#ifndef __LAZY_VECTOR__
#define __LAZY_VECTOR__

#include <cassert>
#include <stdlib.h>
#include <iostream>

#define LAZY_VECTOR_DEFAULT_CAPACITY 16

/*
TODO:
	- implement the commented functions
	- fix iterator & container typedef conflicts?

*/


template<class T>
class lazy_vector {

public:
	// The usual typedef interface against the STL
	typedef T value_type;
	typedef T* pointer;
	typedef const pointer const_pointer;
	typedef T& reference;
	typedef const reference const_reference;
	typedef size_t size_type;

	class iterator;
	typedef const iterator const_iterator;


	typedef struct {
		pointer first;
		size_type size;
		size_type capacity;
	} mem_region;

private:

	mem_region head, tail;

	void mem_swap();
	void reAlloc();
	bool lazy() const;


public:

	lazy_vector();
	//construct n T objects using their default constructor
	lazy_vector(size_type n);
	//construct n T objects, with each being a copy of val
	lazy_vector(size_type n, const_reference val);
	//copy constructor
	lazy_vector(const lazy_vector& vec);
	//assignment operator
	lazy_vector<T>& operator=(const lazy_vector& vec);

	// Iterating

	iterator begin() const;
	iterator end() const;

	// Capacity

	size_type size() const;
	void resize(size_type new_size);
	size_type capacity() const;
	bool empty() const;
	void reserve();
	void shrink_to_fit();

	// Accessing

	reference at(size_type pos) const;
	reference operator[](size_type pos) const;
	reference front() const;
	reference back() const;

	// Modifying

	void push_back(value_type&& val);
	void push_back(const reference val);
	value_type pop_back();
	void insert(iterator pos, const_reference val);
	void insert(iterator pos, size_type n, const_reference val);
	void erase(iterator pos);
	void erase(iterator begin, iterator end);
	void swap(iterator pos1, iterator pos2);
	void swap(lazy_vector& vec);
	void clear();


	// Deconstructor

	~lazy_vector();

	// The iterator

	class iterator {
		
	private:
		pointer current_ptr;
		pointer head_first;
		pointer head_last;
		pointer tail_first;
		pointer tail_last;

	public:
		//pass some existing typedefs from lazy_vector<T> to lazy_vector<T>::iterator
		typedef std::random_access_iterator_tag iterator_category;
		typedef size_type difference_type;
		typedef value_type value_type;
		typedef pointer pointer;
		typedef reference reference;
		typedef size_t size_type;

		iterator();
		iterator(pointer current, pointer head_first, pointer head_last, pointer tail_first, pointer tail_last);

		bool operator==(const iterator& it);
		bool operator!=(const iterator& it);

		iterator operator+(size_type n);
		iterator& operator++(); //return new value
		iterator operator++(int); //return old value
		iterator& operator +=(size_type incr);
		iterator operator-(size_type n);
		iterator& operator--(); // return new value
		iterator operator--(int); //return old value
		size_type operator-(const iterator& it); //the distance between two iterators
		iterator& operator-=(size_type decr);
		reference operator*();
		reference operator->();

		bool operator<(const iterator& it);

		~iterator();
	};


};

/*-----------------------------------------
 | LAZY_VECTOR IMPLEMENTATION BEGINS
 *----------------------------------------*/


/*-----------------------------------------
 | LAZY_VECTOR PRIVATE METHODS
 *----------------------------------------*/

template<class T>
void lazy_vector<T>::mem_swap() {
	//all values T in head have been lazily destructed
	//free the memory
	if (lazy()) {
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
}

template<class T>
bool lazy_vector<T>::lazy() const {
	if (head.capacity == 0) return 0; //head is unused
	// returns true if a lazy copy has to be made
	assert(2*head.size + tail.size <= tail.capacity);
	return 2*head.size + tail.size == tail.capacity;
}


/*-----------------------------------------
 | LAZY_VECTOR PUBLIC METHODS
 *----------------------------------------*/

// LAZY_VECTOR : CONSTRUCTOR & ASSIGNEMNT METHODS
template<class T>
lazy_vector<T>::lazy_vector() : lazy_vector(LAZY_VECTOR_DEFAULT_CAPACITY) {
	head.capacity = 0;
	head.size = 0;
	head.first = nullptr;
	tail.capacity = LAZY_VECTOR_DEFAULT_CAPACITY;
	tail.size = 0;

	tail.first = (pointer) ::operator new(tail.capacity * sizeof(value_type));
}

template<class T>
lazy_vector<T>::lazy_vector(size_type n) {

	head.capacity = 0;
	head.size = 0;
	head.first = nullptr;

	size_type new_capacity = 1;
	//find a reasonable amount of elements to allocate for
	while (n >= new_capacity) new_capacity *= 2;

	tail.capacity = new_capacity;
	tail.size = n;
	tail.first = (pointer) ::operator new(new_capacity * sizeof(value_type));
	//construct n objects value_type using default constructor
	for (size_type i = 0; i < n; ++i) {
		//placement new into tail
		new (tail.first + i) value_type();
	}
}

template<class T>
lazy_vector<T>::lazy_vector(size_type n, const reference) {

	head.capacity = 0;
	head.size = 0;
	head.first = nullptr;

	size_type new_capacity = 1;
	while (n <= new_capcity) new_capacity *= 2;

	tail.capacity = new_capacity
	tail.size = n;
	tail.first = (pointer) ::operator new(n * sizeof(value_type));
	//construct n objects value_type using copy constructor with reference
	for (int i = 0; i < n; ++i) {
		//placement new into tail
		new (tail.first + i) value_type(reference);
	}
}

template<class T>
lazy_vector<T>::lazy_vector(const lazy_vector& cVec) {
	//free memory used by *this
	~lazy_vector();
	//alloc for new head & tail

	head.capacity = cVec.head.capacity;
	head.size = cVec.head.size;
	tail.size = cVec.tail.size;
	tail.capacity = cVec.tail.capacity;
	
	head.first = (pointer) ::operator new(cVec.head.size * sizeof(value_type));
	tail.first = (pointer) ::operator new(cVec.tail.size * sizeof(value_type));
	if (head.size > 0) memcpy(head.first, cVec.head.first, head.size * sizeof(value_type));
	memcpy(tail.first+head.size, cVec.first+head.size, tail.size * sizeof(value_type));
}

template<class T>
lazy_vector<T>& lazy_vector<T>::operator=(const lazy_vector& vec) {
	// TODO: copy swap idiom for exception safety
	this->lazy_vector(vec);
}

// LAZY_VECTOR : CAPACITY

template<class T>
typename lazy_vector<T>::size_type lazy_vector<T>::size() const {
	return head.size + tail.size;
}


template<class T>
void lazy_vector<T>::resize(size_type new_size) {
	
	//if increasing - copy head to tail & swap & reAlloc
	//if decreasing - depends on whether new_size < or >= head.capacity
}

template<class T>
typename lazy_vector<T>::size_type lazy_vector<T>::capacity() const {
	return tail.capacity;
}

template<class T>
bool lazy_vector<T>::empty() const {
	return size() == 0;
}

template<class T>
void lazy_vector<T>::reserve() {
	//if increasing - copy head to tail & swap & reAlloc
	//if decreasing - depends on whether new_size < or >= head.capacity
}

template<class T>
void lazy_vector<T>::shrink_to_fit() {
	//if increasing - copy head to tail & swap & reAlloc
	//if decreasing - depends on whether new_size < or >= head.capacity
}


// LAZY_VECTOR : ITERATORS METHODS
template<class T>
typename lazy_vector<T>::iterator lazy_vector<T>::begin() const {
	pointer head_last = head.first + head.size - 1;
	pointer tail_first = tail.first + head.size;
	pointer tail_last = tail.first + head.size + tail.size - 1;

	pointer current_ptr;
	if (lazy()) current_ptr = head.first;
	else current_ptr = tail_first;
	return iterator(current_ptr, head.first, head_last,
			tail_first, tail_last);
}

template<class T>
typename lazy_vector<T>::iterator lazy_vector<T>::end() const {
	pointer head_last = head.first + head.size - 1;
	pointer tail_first = tail.first + head.size;
	pointer tail_last = tail.first + head.size + tail.size - 1;

	return iterator(tail_last + 1, head.first, head_last,
			tail_first, tail_last);
}

// LAZY_VECTOR : ACCESSING METHODS
template<class T>
typename lazy_vector<T>::reference lazy_vector<T>::at(size_type pos) const {
	// possibly throw out of range exception
	if (pos < 0 || pos >= size())
		throw std::out_of_range;
	if (pos >= head.size) {
		return tail.first[pos];
	}
	else {
		return head.first[pos];
	}
}

template<class T>
typename lazy_vector<T>::reference lazy_vector<T>::operator[](size_type pos) const {
	// no throw
	if (pos >= head.size) {
		return tail.first[pos];
	}
	else {
		return head.first[pos];
	}
}

template<class T>
typename lazy_vector<T>::reference lazy_vector<T>::front() const {
	return *begin();
}

template<class T>
typename lazy_vector<T>::reference lazy_vector<T>::back() const {
	return *end();
}

// LAZY_VECTOR : MODIFYING METHODS template<class T>

template<class T>
void lazy_vector<T>::push_back(value_type&& val) {
	if (head.size + tail.size >= tail.capacity) {
		reAlloc();
	}

	//construct new T in tail - using placement new
	new (&tail.first[head.size + tail.size]) value_type(val);
	if (lazy()) {

		//lazy copy 1 item from head to tail using copy constructor
		new (&tail.first[head.size-1]) value_type(head.first[head.size-1]);
		tail.size++;
		//explicitly destruct unused T in head
		(&head.first[head.size-1])->~value_type();
		head.size--;
	}
	++tail.size;
}


template<class T>
void lazy_vector<T>::push_back(const reference val) {
	if (head.size + tail.size >= tail.capacity) {
		reAlloc();
	}

	//construct new T in tail - using placement new
	new (&tail.first[head.size + tail.size]) value_type(val);
	if (lazy()) {

		//lazy copy 1 item from head to tail using copy constructor
		new (&tail.first[head.size-1]) value_type(head.first[head.size-1]);
		tail.size++;
		//explicitly destruct unused T in head
		(&head.first[head.size-1])->~value_type();
		head.size--;
	}
	++tail.size;
}

template<class T>
typename lazy_vector<T>::value_type lazy_vector<T>::pop_back() {
	reference element_at_back = *(this->end() - 1);
	if (lazy()) {
		//move 1 item from tail to head using copy constructor
		reference old_element = *(tail.first + head.size);
		new (head.first + head.size) value_type(old_element);
		// deconstruct old item
		old_element.~value_type();
		++head.size;
		--tail.size;
	}
	value_type tmp = element_at_back;
	// deconstruct element at back
	element_at_back.~value_type();
	--tail.size;

	if (tail.size == 0) {
		//deallocate tail and head becomes new tail
		::operator delete(tail.first);
		tail = head;
		head.first = nullptr;
		head.size = 0;
		head.capacity = 0;
	}

	// return as a copy
	return tmp;
}

template<class T>
void lazy_vector<T>::insert(const_iterator pos, const_reference val) {
	//move elements after pos by 1 (may require reAlloc)
	//then construct 1 element at pos
}

template<class T>
void lazy_vector<T>::insert(const_iterator pos, size_type n, const_reference val) {
	//move elements after pos by n (may require reAlloc)
	//then construct n elements at pos until pos + n
}

template<class T>
void lazy_vector<T>::erase(iterator pos) {
	//deconstruct at pos
	//move elements after pos left by 1
}

template<class T>
void lazy_vector<T>::erase(iterator begin_pos, iterator end_pos) {
	//deconstruct at begin_pos until end_pos
	//for all deconstructions move elements after end_pos by N = distance between begin - end
}

template<class T>
void lazy_vector<T>::swap(iterator pos1, iterator pos2) {
	value_type tmp = *pos1;
	*pos1 = *pos2;
	*pos2 = tmp;
}

template<class T>
void lazy_vector<T>::swap(lazy_vector& vec) {
	//create tmp and swap two lazy_vectors
}

template<class T>
void lazy_vector<T>::clear() {
	//deconstruct all existing elements in head & tail
	for (size_type i = 0; i < head.size; ++i) {
		head.first[i].~value_type();
	}
	for (size_type i = 0; i < tail.size; ++i) {
		tail.first[i].~value_type();
	}
	head.size = tail.size = 0;
}

// LAZY_VECTOR : DESTRUCTOR

template<class T>
lazy_vector<T>::~lazy_vector() {
	clear();
	
	if (head.capacity >= 0) {
		::operator delete(head.first);
		head.capacity = 0;
	}
	if (tail.capacity >= 0) {
		::operator delete(tail.first);
		tail.capacity = 0;
	}
}

/*-----------------------------------------
 | LAZY_VECTOR IMPLEMENTATION ENDS
 *----------------------------------------*/


/*-----------------------------------------
 | LAZY_VECTOR ITERATOR IMPLEMENTATIONS BEGINS
 *----------------------------------------*/

template<class T>
lazy_vector<T>::iterator::iterator() {
}

template<class T>
lazy_vector<T>::iterator::iterator(pointer curr, pointer h_first, pointer h_last, pointer t_first, pointer t_last)
	: current_ptr(curr), head_first(h_first), head_last(h_last), tail_first(t_first), tail_last(t_last) {
}


template<class T>
typename lazy_vector<T>::iterator lazy_vector<T>::iterator::operator+(size_type n) {
	pointer new_ptr;
	// if current_ptr is within head and current_ptr + n exceeds head..
	if (current_ptr + n > head_last && current_ptr <= head_last) {
		size_type head_offset = head_last - current_ptr; //distance or length between
		new_ptr = tail_first + n - head_offset - 1;
	}
	else new_ptr = current_ptr + n;
	return lazy_vector<T>::iterator(new_ptr, head_first, head_last, tail_first, tail_last);
}

template<class T>
bool lazy_vector<T>::iterator::operator==(const iterator& it) {
	return current_ptr == it.current_ptr;
}

template<class T>
bool lazy_vector<T>::iterator::operator!=(const iterator& it) {
	return current_ptr != it.current_ptr;
}


template<class T>
typename lazy_vector<T>::iterator& lazy_vector<T>::iterator::operator++() {
	if (current_ptr == head_last) current_ptr = tail_first;
	else ++current_ptr;
	return *this;
}


template<class T>
typename lazy_vector<T>::iterator lazy_vector<T>::iterator::operator++(int) {
	lazy_vector<T>::iterator tmp(*this);
	++(*this);
	return tmp;
}

template<class T>
typename lazy_vector<T>::iterator& lazy_vector<T>::iterator::operator+=(size_type incr) {
	*this = *this + incr;
	return *this;
}

template<class T>
typename lazy_vector<T>::iterator lazy_vector<T>::iterator::operator-(size_type len) {
	// if current_ptr is within head and current_ptr + len exceeds head..
	pointer new_ptr;
	if (current_ptr - len < tail_first && current_ptr >= tail_first) {
		size_type tail_offset = current_ptr - tail_first; //distance or length between
		new_ptr = head_last - len + tail_offset + 1;
	}
	else new_ptr = current_ptr - len;
	return lazy_vector<T>::iterator(new_ptr, head_first, head_last, tail_first, tail_last);

}

template<class T>
typename lazy_vector<T>::iterator& lazy_vector<T>::iterator::operator--() {
	if (current_ptr == tail_first) current_ptr = head_last;
	else --current_ptr;
	return *this;
}

template<class T>
typename lazy_vector<T>::iterator lazy_vector<T>::iterator::operator--(int) {
	lazy_vector<T>::iterator tmp(*this);
	--(*this);
	return tmp;
}


template<class T>
typename lazy_vector<T>::size_type lazy_vector<T>::iterator::operator-(const iterator& it) {
	// iterator& it is the first pointer and *this is the last pointer
	// function returns last - first as size_type i.e. how many elements T are between
	if (it.current_ptr >= head_first && it.current_ptr <= head_last) {
		if (current_ptr >= head_first && current_ptr <= head_last) {
			return current_ptr - it.current_ptr;
		}
		else {
			//last iterator (*this) is in tail - calculate offset
			size_type tail_offset = current_ptr - tail_first; //distance or length between
			size_type head_offset = head_last - it.current_ptr;
			return tail_offset + head_offset;
		}
	}
	else {
		// iterator& is in tail - and (*this) must also be in tail since first <= last
		// so wen can simply return last - first
		return current_ptr - it.current_ptr;
	}
}

template<class T>
typename lazy_vector<T>::iterator& lazy_vector<T>::iterator::operator-=(size_type decr) {
	*this = *this - decr;
	return *this;
}

template<class T>
typename lazy_vector<T>::reference lazy_vector<T>::iterator::operator*() {
	return *current_ptr;
}

template<class T>
typename lazy_vector<T>::reference lazy_vector<T>::iterator::operator->() {
	return *current_ptr;
}

template<class T>
bool lazy_vector<T>::iterator::operator<(const iterator& it) {
	if (it.current_ptr >= head_first && it.current_ptr <= head_last) {
		if (current_ptr >= head_first && current_ptr <= head_last) {
			return current_ptr < it.current_ptr;
		}
		else return 0;
	}
	else {
		if (current_ptr >= head_first && current_ptr <= head_last) {
			return 1;
		}
		else return current_ptr < it.current_ptr;
	}
}

template<class T>
lazy_vector<T>::iterator::~iterator() {
}

/*-----------------------------------------
 | LAZY_VECTOR ITERATOR IMPLEMENTATIONS ENDS
 *----------------------------------------*/

#endif // __LAZY_VECTOR__

