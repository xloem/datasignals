#include "impl/DataPointer.hpp"

#include <iostream>
#include <vector>

using namespace datas::impl;

int main()
{
	std::vector<float> back{1.1,1.2,1.3,1.4};
	DataPointer<float, float, float> data({-1,-1}, {2, 2}, {1, 1}, back.data(), back.size());

	std::cout << data.get({0.0f,0.0f}) << std::endl;
}
