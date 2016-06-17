
#include <iostream>

#include "lazy_vector.h"
#include <random>
#include <algorithm>

#include <vector>
#include <ctime>

int main(void) {
	
	size_t size = 100000;
	int* nums = new int[size];


	lazy_vector<int> lVec;
	std::vector<int> nVec;
	for (int i = 0; i < size; i++) {
		nums[i] = (rand() % 500000);
	}
	// FILL LAZY
	std::cout << "LAZY_VECTOR PUSH BACK FILL TIME: ";
	std::clock_t begin = std::clock();

	for (int i = 0; i < size; i++) {
		lVec.push_back(nums[i]);
	}

	std::clock_t end = std::clock();

	std::cout << double(end-begin)/CLOCKS_PER_SEC << std::endl << std::endl;


	std::cout << "ORDINARY VECTOR PUSH BACK FILL TIME: ";
	begin = std::clock();

	for (int i = 0; i < size; i++) {
		nVec.push_back(nums[i]);
	}
	end = std::clock();

	std::cout << double(end-begin)/CLOCKS_PER_SEC << std::endl << std::endl;

	std::cout << "IS SAME VEC: ";
	bool isSame = 1;
	for (int i = 0; i < size; i++) {
		if (nVec[i] != lVec[i]) {
			isSame = 0;
			break;
		}
	}
	std::cout << isSame << std::endl << std::endl;


	std::cout << "LAZY VECTOR SORT TIME: ";
	begin = std::clock();
	std::sort(lVec.begin(), lVec.end());
	end = std::clock();
	std::cout << double(end - begin) / CLOCKS_PER_SEC << std::endl << std::endl;
	
	std::cout << "VECTOR SORT TIME: ";
	begin = std::clock();
	std::sort(nVec.begin(), nVec.end());
	end = std::clock();
	std::cout << double(end - begin) / CLOCKS_PER_SEC << std::endl << std::endl;
	
	
	
	lVec.swap(lVec.begin() + 97, lVec.begin() + 107);
	


	int wait = 0;
	std::cin >> wait;
	
}
