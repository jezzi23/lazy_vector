
#ifndef LAZY_VECTOR_H_
#define LAZY_VECTOR_H_

#include <initializer_list>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <utility>

template<class T, class Allocator = std::allocator<T>>
class lazy_vector {
public:
  // The usual typedef interface against the STL
  typedef T                 value_type;
  typedef value_type*       pointer;
  typedef const pointer     const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef std::size_t       size_type;
  typedef std::ptrdiff_t    difference_type;

  class iterator;
  typedef const iterator const_iterator;

  // Construct an empty lazy_vector
  lazy_vector();
  // Construct with n objects T, with each being a copy of val
  // val defaults to T's default constructor
  lazy_vector(const size_type n, const_reference val = value_type());
  // Construct with initializer list, e.g. { 1, 2, 3 }
  lazy_vector(const std::initializer_list<T>& rhs_list);
  // Copy constructor - exception safe
  lazy_vector(const lazy_vector& rhs_vec);
  // (Move) Assignment - exception safe
  lazy_vector& operator=(lazy_vector rhs_vec);
  // Move constructor - exception safe
  lazy_vector(lazy_vector&& rhs_vec);

  ~lazy_vector();

  // Iterator providers

  // Returns an iterator to first element
  iterator begin() const;
  // Returns an iterator to the end, with end()-1 begin an iterator to the last element
  iterator end() const;

  // Storage

  // Returns the amount of elements in the container
  size_type size() const;
  // Force a new size, amount of elements, for the container
  void resize(const size_type new_size, const_reference val = value_type());
  // Returns the maximum capacity of the container
  size_type capacity() const;
  // Returns 0 if empty, else 1
  bool empty() const;
  // Prepares the container for storing 'reserve_amount' elements
  // without the need for further allocations
  void reserve(const size_type reserve_amount);

  // Accessing

  // Returns the element at a given position - may throw std::out_of_range
  reference at(const size_type pos) const;
  // Returns the element at a given position - does not throw an exception
  reference operator[](const size_type pos) const;
  // Returns the first element
  reference front() const;
  // Returns the last element
  reference back() const;

  // Modifying

  // Inserts a new element at the end, as a copy of a given value
  void push_back(const_reference val);
  // Removes the last element and returns a copy of it
  value_type pop_back();
  // Swap two vectors of the same type
  static void swap(lazy_vector<T, Allocator>& lhs_vec, lazy_vector<T, Allocator>& rhs_vec);
  // Remove all elements
  // Capacity remains the same
  void clear();

  // Customized STL compliant iterator, lazy_vector<...>::iterator
  class iterator {
  public:
    iterator();
    iterator(pointer current, pointer head_first,
             pointer head_last, pointer tail_first,
             pointer tail_last);

    bool operator==(const iterator& it);
    bool operator!=(const iterator& it);
    bool operator<(const iterator& it);

    // operators appreciated by the STL for iterators
    iterator  operator+(const size_type n);
    iterator& operator++();
    iterator  operator++(const int);
    iterator& operator +=(const size_type incr);
    iterator  operator-(const size_type n);
    iterator& operator--();
    iterator  operator--(int);
    size_type operator-(const iterator& it); //the distance between two iterators
    iterator& operator-=(const size_type decr);
    reference operator*();
    reference operator->();

  private:
    pointer current_ptr, head_first, head_last, tail_first, tail_last;
  };

private:
  void extend();
  void shorten();

  void empty_head();
  bool lazy() const;

  typedef struct {
    pointer first;
    size_type size;
    size_type capacity;
  } mem_region;

  mem_region head, tail;
  static const size_t default_capacity;
  static Allocator allocator;
};

/*----------------------------------------*
 | BEGIN LAZY_VECTOR IMPLEMENTATION
 *----------------------------------------*/

// LAZY_VECTOR - PUBLIC METHODS

// LAZY_VECTOR : CONSTRUCTOR, ASSIGNEMNT & DESTRUCTOR METHODS
template<class T, class Allocator>
lazy_vector<T, Allocator>::lazy_vector() : head() {
  pointer tail_array = static_cast<pointer>(allocator.allocate(default_capacity));
  tail = { tail_array, 0, default_capacity };
}

template<class T, class Allocator>
lazy_vector<T, Allocator>::lazy_vector(const size_type n, const_reference val) : head() {
  size_type new_capacity = 1;
  while (n >= new_capacity) new_capacity <<= 1;
  pointer tail_array = static_cast<pointer>(allocator.allocate(new_capacity));
  tail = { tail_array, n, new_capacity };

  for (size_type i = 0; i < n; ++i) {
    new (tail.first + i) value_type(val);
  }
}

template<class T, class Allocator>
lazy_vector<T, Allocator>::lazy_vector(const std::initializer_list<T>& list) : head() {

  size_type new_capacity = 1;
  const size_type n = list.size();
  while (n >= new_capacity) new_capacity <<= 1;

  pointer tail_array = static_cast<pointer>(allocator.allocate(new_capacity));
  tail = { tail_array, n, new_capacity };

  size_type i = 0;
  for (const auto item : list) {
    new (tail.first + (i++)) value_type(item);
  }
}

template<class T, class Allocator>
lazy_vector<T, Allocator>::lazy_vector(const lazy_vector& rhs_vec) : head(rhs_vec.head),
                                                                     tail(rhs_vec.tail) {
  //alloc for new head & tail
  if (head.size > 0) {
    head.first = static_cast<pointer>(allocator.allocate(head.capacity));
    std::memcpy(head.first, 
                rhs_vec.head.first,
                head.size * sizeof(value_type));
  }
  if (tail.size > 0) { 
    tail.first = static_cast<pointer>(allocator.allocate(tail.capacity));
    std::memcpy(tail.first + head.size,
                rhs_vec.tail.first + head.size,
                tail.size * sizeof(value_type));
  }
}

template<class T, class Allocator>
lazy_vector<T, Allocator>::lazy_vector(lazy_vector&& rhs_vec) : head(rhs_vec.head),
                                                                tail(rhs_vec.tail) {
  //remove ownership from rhs_vec
  rhs_vec.head = rhs_vec.tail = { nullptr , 0, 0 };
}

// LAZY_VECTOR : DESTRUCTOR

template<class T, class Allocator>
lazy_vector<T, Allocator>::~lazy_vector() {
  clear();

  if (head.capacity > 0) {
    allocator.deallocate(head.first, head.capacity);
    head.capacity = 0;
  }
  if (tail.capacity > 0) {
    allocator.deallocate(tail.first, tail.capacity);
    tail.capacity = 0;
  }
}

template<class T, class Allocator>
lazy_vector<T, Allocator>& lazy_vector<T, Allocator>::operator=(lazy_vector vec) {
  // vec is passed by value and is created as a temporary vector
  // using the copy constructor

  // proceed to swap after successful copying
  lazy_vector<T, Allocator>::swap(*this, vec);
  return *this;
}

// LAZY_VECTOR : CAPACITY

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::size_type lazy_vector<T, Allocator>::size() const {
  return head.size + tail.size;
}

template<class T, class Allocator>
void lazy_vector<T, Allocator>::resize(const size_type new_size, const_reference val) {
  while (new_size > size()) {
    push_back(val);
  }
  while (new_size < size()) {
    pop_back();
  }
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::size_type lazy_vector<T, Allocator>::capacity() const {
  return tail.capacity;
}

template<class T, class Allocator>
bool lazy_vector<T, Allocator>::empty() const {
  return size() == 0;
}

template<class T, class Allocator>
void lazy_vector<T, Allocator>::reserve(const size_type reserve_amount) {
  if (reserve_amount > capacity()) {
    empty_head();
    if (head.first) {
      allocator.deallocate(head.first, head.capacity);
    }
  }
  head = tail;
  // tail can now be overwritten
  size_type new_capacity = head.capacity;
  while (reserve_amount >= new_capacity) new_capacity <<= 1;
  pointer tail_array = static_cast<pointer>(allocator.allocate(new_capacity));
  tail = { tail_array, 0, new_capacity };
}

// LAZY_VECTOR : ITERATORS METHODS

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator lazy_vector<T, Allocator>::begin() const {
  pointer head_last = head.first + head.size - 1;
  pointer tail_first = tail.first + head.size;
  pointer tail_last = tail.first + head.size + tail.size - 1;

  pointer current_ptr;
  if (lazy()) current_ptr = head.first;
  else current_ptr = tail_first;
  return iterator(current_ptr, head.first, head_last,
                               tail_first, tail_last);
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator lazy_vector<T, Allocator>::end() const {
  pointer head_last = head.first + head.size - 1;
  pointer tail_first = tail.first + head.size;
  pointer tail_last = tail.first + head.size + tail.size - 1;

  return iterator(tail_last + 1, head.first, head_last,
                                 tail_first, tail_last);
}

// LAZY_VECTOR : ACCESSING METHODS
template<class T, class Allocator>
typename lazy_vector<T, Allocator>::reference lazy_vector<T, Allocator>::at(
    const size_type pos) const {
  // possibly throw out of range exception
  if (pos >= size())
    throw std::out_of_range("lazy_vector.at() access out of range");
  if (pos >= head.size) {
    return tail.first[pos];
  }
  else {
    return head.first[pos];
  }
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::reference lazy_vector<T, Allocator>::operator[](
    const size_type pos) const {
  // no throw
  if (pos >= head.size) {
    return tail.first[pos];
  }
  else {
    return head.first[pos];
  }
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::reference lazy_vector<T, Allocator>::front() const {
  return *begin();
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::reference lazy_vector<T, Allocator>::back() const {
  return *(end() - 1);
}

// LAZY_VECTOR : MODIFYING METHODS

template<class T, class Allocator>
void lazy_vector<T, Allocator>::push_back(const_reference val) {
  if (head.size + tail.size >= tail.capacity) {
    extend();
  }
  if (lazy()) {
    //lazy copy 1 item from head to tail using copy constructor
    new (&tail.first[head.size - 1]) value_type(head.first[head.size - 1]);
    ++tail.size;
    //explicitly destruct unused T in head
    head.first[head.size - 1].~value_type();
    --head.size;
  }
  //construct new T in tail - using placement new
  new (&tail.first[head.size + tail.size]) value_type(val);
  ++tail.size;
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::value_type lazy_vector<T, Allocator>::pop_back() {
  reference element_at_back = *(this->end() - 1);
  if (lazy()) {
    // move 1 item from tail to head using copy constructor
    reference old_element = *(tail.first + head.size);
    new (head.first + head.size) value_type(old_element);
    old_element.~value_type();
    ++head.size;
    --tail.size;
  }
  value_type tmp = element_at_back;
  element_at_back.~value_type();
  --tail.size;

  if (tail.size == 0) {
    shorten();
  }

  // return as a copy
  return tmp;
}

template<class T, class Allocator>
void lazy_vector<T, Allocator>::clear() {
  //deconstruct all existing elements in head & tail
  for (size_type i = 0; i < head.size; ++i) {
    head.first[i].~value_type();
  }
  for (size_type i = 0; i < tail.size; ++i) {
    tail.first[head.size + i].~value_type();
  }
  head.size = tail.size = 0;
}

// the swap function is guaranteed to never throw
template <class T, class Allocator>
void lazy_vector<T, Allocator>::swap(lazy_vector& lhs_vec, lazy_vector& rhs_vec) {
  using std::swap;

  swap(lhs_vec.head, rhs_vec.head);
  swap(lhs_vec.tail, rhs_vec.tail);
}

// LAZY_VECTOR PRIVATE METHODS

//head.size must be 0
template<class T, class Allocator>
void lazy_vector<T, Allocator>::extend() {
  //all values T in head have been destructed
  //free the memory
  allocator.deallocate(head.first, head.capacity);

  head = tail; // head becomes tail
  // tail may now be overwritten

  size_type new_size = head.capacity * 2;
  pointer tail_array = static_cast<pointer>(allocator.allocate(new_size));
  tail = { tail_array, 0, new_size };
}

//tail.size must be 0
template<class T, class Allocator>
void lazy_vector<T, Allocator>::shorten() {
  //all values T in tail have been destructed
  //free memory in tail
  allocator.deallocate(tail.first, tail.capacity);
  tail = head;

  //head may now be overwritten
  head = { nullptr, 0, 0 };
}

template<class T, class Allocator>
void lazy_vector<T, Allocator>::empty_head() {
  // clear out head and move to tail
  for (size_type i = 0; i < head.size; ++i) {
    new (tail.first + i)  value_type(*(head.first + i));
  }
  for (size_type i = 0; i < head.size; ++i) {
    head.first[i].~value_type();
  }
  tail.size += head.size;
  head.size = 0;
}

template<class T, class Allocator>
bool lazy_vector<T, Allocator>::lazy() const {
  if (head.capacity == 0) return 0; //head is unused
  // returns true if a lazy copy has to be made
  return 2 * head.size + tail.size == tail.capacity;
}

template<class T, class Allocator>
const typename lazy_vector<T, Allocator>::size_type lazy_vector<T, Allocator>::default_capacity = 1 << 4;

template<class T, class Allocator>
Allocator lazy_vector<T, Allocator>::allocator;

/*-----------------------------------------
 | END LAZY_VECTOR IMPLEMENTATION
 *----------------------------------------*/

/*-----------------------------------------
 | BEGIN LAZY_VECTOR ITERATOR IMPLEMENTATION
 *----------------------------------------*/

template<class T, class Allocator>
lazy_vector<T, Allocator>::iterator::iterator() {
}

template<class T, class Allocator>
lazy_vector<T, Allocator>::iterator::iterator(pointer curr, pointer h_first, pointer h_last,
                                                            pointer t_first, pointer t_last) :
    current_ptr(curr),
    head_first(h_first), head_last(h_last),
    tail_first(t_first), tail_last(t_last) {
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator lazy_vector<T, Allocator>::iterator::operator+(
    const size_type n) {
  pointer new_ptr;
  // if current_ptr is within head and current_ptr + n exceeds head..
  if (current_ptr + n > head_last && current_ptr <= head_last) {
    size_type head_offset = head_last - current_ptr; //distance or length between
    new_ptr = tail_first + n - head_offset - 1;
  }
  else new_ptr = current_ptr + n;
  return lazy_vector<T, Allocator>::iterator(new_ptr, head_first, head_last, tail_first, tail_last);
}

template<class T, class Allocator>
bool lazy_vector<T, Allocator>::iterator::operator==(const iterator& it) {
  return current_ptr == it.current_ptr;
}

template<class T, class Allocator>
bool lazy_vector<T, Allocator>::iterator::operator!=(const iterator& it) {
  return current_ptr != it.current_ptr;
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator& lazy_vector<T, Allocator>::iterator::operator++() {
  if (current_ptr == head_last) current_ptr = tail_first;
  else ++current_ptr;
  return *this;
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator lazy_vector<T, Allocator>::iterator::operator++(int) {
  lazy_vector<T, Allocator>::iterator tmp(*this);
  ++(*this);
  return tmp;
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator& lazy_vector<T, Allocator>::iterator::operator+=(
    const size_type incr) {
  *this = *this + incr;
  return *this;
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator lazy_vector<T, Allocator>::iterator::operator-(
    const size_type len) {
  // if current_ptr is within head and current_ptr + len exceeds head..
  pointer new_ptr;
  if (current_ptr - len < tail_first && current_ptr >= tail_first) {
    size_type tail_offset = current_ptr - tail_first; //distance or length between
    new_ptr = head_last - len + tail_offset + 1;
  }
  else new_ptr = current_ptr - len;
  return lazy_vector<T, Allocator>::iterator(new_ptr, head_first, head_last, tail_first, tail_last);

}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator& lazy_vector<T, Allocator>::iterator::operator--() {
  if (current_ptr == tail_first) current_ptr = head_last;
  else --current_ptr;
  return *this;
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator lazy_vector<T, Allocator>::iterator::operator--(
    const int) {
  lazy_vector<T, Allocator>::iterator tmp(*this);
  --(*this);
  return tmp;
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::size_type lazy_vector<T, Allocator>::iterator::operator-(
    const iterator& it) {
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

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::iterator& lazy_vector<T, Allocator>::iterator::operator-=(
    const size_type decr) {
  *this = *this - decr;
  return *this;
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::reference lazy_vector<T, Allocator>::iterator::operator*() {
  return *current_ptr;
}

template<class T, class Allocator>
typename lazy_vector<T, Allocator>::reference lazy_vector<T, Allocator>::iterator::operator->() {
  return *current_ptr;
}

template<class T, class Allocator>
bool lazy_vector<T, Allocator>::iterator::operator<(const iterator& it) {
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

/*-----------------------------------------
 | END LAZY_VECTOR ITERATOR IMPLEMENTATION
 *----------------------------------------*/
  
#endif // LAZY_VECTOR_H_
