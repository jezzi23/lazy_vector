
#include <iostream>

#include "lazy_vector.h"
#include <random>
#include <algorithm>

#include <vector>
#include <ctime>

template<class T>
void printVec(const lazy_vector<T>& vec) {
	for (T& i: vec) {
		std::cout << i << ",";
	}
	std::cout << std::endl;
}

template<class T>
bool equalVec(const lazy_vector<T>& vec_first, const lazy_vector<T>& vec_second) {
	if (vec_first.size() != vec_second.size()) return 0;
	for (unsigned int i = 0; i < vec_first.size(); ++i) {
		if (vec_first[i] != vec_second[i]) return 0;
	}
	return 1;
}

int main(void) {
	lazy_vector<int> vec = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	//or lazy_vector<int, MyCrazyAllocator> using a custom allocator
	
	
	//vec now contains [9,8,7,6,5,4,3,2,1,0].
	printVec(vec);

	//using the iterator
	lazy_vector<int>::iterator it;
	for (it = vec.begin(); it != vec.end(); ++it) {
		if (*it < 5) *it *= 2;
	}
	//for (int& x: vec) {...} also works
	//vec now contains [9,8,7,6,5,8,6,4,2,0].
	printVec(vec);


	// random accesing
	vec[7] = 100;
	//vec now contains [9,8,7,6,5,8,6,100,2,0].
	printVec(vec);


	//using STL functions
	//swap fourth element from beginning wand the third element from the end
	std::iter_swap(vec.begin() + 4, vec.end() - 3);
	//vec now contains [9,8,7,6,100,8,6,5,2,0].
	printVec(vec);


	//sort the lazy_vector from beginning to the seventh element
	std::sort(vec.begin(), vec.begin() + 8);
	//vec now contains [5,6,6,7,8,8,9,100,2,0].
	printVec(vec);

	lazy_vector<int> cVec(vec);
	printVec(cVec);
	

	std::cout << "is same vec: " << equalVec(vec, cVec) << std::endl;

	cVec[5] = 502;
	std::cout << "is same vec: " << equalVec(vec, cVec) << std::endl;

	
	printVec(cVec);

	vec = cVec;
	cVec = cVec;


	int wait = 0;
	std::cin >> wait;
	
}
