# lazy_vector

##What is lazy_vector?

lazy_vector works as a dynamic array that might, behind the scenes, reside in two different memory locations. 

##Motivation.
lazy_vector guarantees worst case constant time complexity of insertions at back whereas std::vector of the STL has linear time complexity in the worst case when a reallocation is needed. In order to accomplish this, lazy_vector lazily copies one element at a time from the older array to the newer one on each new insertion.
In some applications, a large reallocation for a dynamic array such as std::vector is unacceptible. For example in real-time simulations.

##Usage.

lazy_vector's implementation is abstracted and designed to be usable with many features of the STL.

* Can be used very similarly as std::vector.
* Accepts a custom allocator. By default std::allocator is used.
* Can be manipulated using an iterator.
* Elements' constructors and destructors are called appropriately.

##Examples.
```
lazy_vector<int> vec;
for (int i = 0; i <= 10; ++i){
  push.back(10-i);
}
//vec now contains [9,8,7,6,5,4,3,2,1,0].

//using the iterator

lazy_vector<int>::iterator it;
for (it = vec.begin(); it != vec.end(); ++it) {
  if (*it < 5) *it *= 2;
}
//for (int& x: vec) {...} also works
//vec now contains [9,8,7,6,5,**16,9,4,1,0**].

// random accesing
vec[7] = 100;
//vec now contains [9,8,7,6,5,16,9,**100**,1,0].

//using STL functions

//swap fourth element from beginning wand the second element from the end
std::swap(vec.begin()+4, vec.end()-2);
//vec now contains [9,8,7,6,**0**,16,9,100,1,**5**].

//sort the lazy_vector from beginning to the seventh element
std::sort(vec.begin(), vec.begin()+8);
//vec now contains [**0,6,7,8,9,9,16,100**,1,5].
