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
<pre><code>
lazy_vector\<int\> vec;
//or lazy_vector\<int, MyCrazyAllocator\> using a custom allocator
for (int i = 0; i < 10; ++i){
  vec.push_back(9-i);
}
//vec now contains [9,8,7,6,5,4,3,2,1,0].

//using the iterator
lazy_vector<int>::iterator it;
for (it = vec.begin(); it != vec.end(); ++it) {
  if (*it < 5) *it *= 2;
}
//for (int& x: vec) {...} also works
//vec now contains [9,8,7,6,5,<b>8,6,4,2,0</b>].

// random accesing
vec[7] = 100;
//vec now contains [9,8,7,6,5,8,6,<b>100</b>,2,0].

//using STL functions
//swap fourth element from beginning wand the third element from the end
std::iter_swap(vec.begin()+4, vec.end()-3);
//vec now contains [9,8,7,6,<b>100</b>,8,6,<b>5</b>,2,0].

//sort the lazy_vector from beginning to the seventh element
std::sort(vec.begin(), vec.begin()+8);
//vec now contains [<b>5,6,6,7,8,8,9,100</b>,2,0].
