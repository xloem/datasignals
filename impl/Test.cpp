#include "DataArray.hpp"

#include <iostream>

using namespace datas::impl;

int main()
{
	std::vector<float> back{1.1,1.2,1.3,1.4};
	DataArray<float, float, float> data({-1,-1}, {2, 2}, {1, 1}, back.data());

	std::cout << data.get({0.0,0.0}) << std::endl;
}
