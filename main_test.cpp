
#include <iostream>

#include "lazy_vector.h"



#include <vector>
#include <ctime>

int main(void) {
/*
	std::clock_t begin = std::clock();

	lazy_vector<int> vec(6);

	for (int i = 0; i < 10000000; i++) {
		vec.push_back(i);
	}
	
	std::clock_t end = std::clock();

	std::cout << double(end-begin)/CLOCKS_PER_SEC << std::endl;


	
	begin = std::clock();

	std::vector<int> tvec;
	for (int i = 0; i < 10000000; i++) {
		tvec.push_back(i);
	}
	end = std::clock();

	std::cout << double(end-begin)/CLOCKS_PER_SEC << std::endl;

	
	*/


	lazy_vector<int> vec;

	for (int i = 0; i < 1000; i++) {
		vec.push_back(i);
	}

	for (int i = 0; i < 1000; i++) {
		std::cout << vec[i] << std::endl;
	}

	int wait = 5;

	vec.push_back(wait);
}
