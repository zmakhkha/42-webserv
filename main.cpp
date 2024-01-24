#include <iostream>
#include <vector>
#include <map>

int main () {
	std::vector < std::string > integers;
	for (size_t i = 0; i < 5; i++)
		integers.push_back(std::string("number_" + std::to_string(i)));
	std::cout << "before : \n" << std::flush;
	for (size_t i = 0; i < integers.size(); i++)
		std::cout << i << " " << integers[i] << std::endl;
	integers.erase(integers.begin() + 2);
	std::cout << "after : \n" << std::flush;
	for (size_t i = 0; i < integers.size(); i++)
		std::cout << i << " " << integers[i] << std::endl;
}