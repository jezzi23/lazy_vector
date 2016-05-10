
#ifndef __LAZY_VECTOR__
#define __LAZY_VECTOR__

#include <stdlib.h>

#define LAZY_VECTOR_DEFAULT_CAPACITY 50

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

typedef struct {
	pointer first;
	type_size size;
	type_size capacity;
} mem_region;

private:

mem_region head, tail;

public:

	lazy_vector();
	lazy_vector(type_size);

	void insert_back(const reference);
	reference at(type_size);

	iterator begin() const;
	iterator end() const;

	~lazy_vector();
};

//Implementation
template<class T>
lazy_vector<T>::() {
	lazy_vector(LAZY_VECTOR_DEFAULT_CAPACITY);
}

template<class T>
lazy_vector<T>(type_size n) {
	head->pointer = (pointer) malloc(n*sizeof(value_type));
	head->size = n;
}

template<class T>
void lazy_vetor<T>::insert_back(const reference val) {
	new (head->first + (head->size++) * sizeof(value_type)) value_type(val);
}

template<class T>
reference lazy_vetor<T>::at(type_size pos) {
	return head->first + pos * sizeof(value_type);
}


#endif // __LAZY_VECTOR__
