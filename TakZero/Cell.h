#pragma once

#include <bitset>

class Cell
{
public:
	Cell();
	~Cell();



private:
	std::bitset<3*64> stack;


};
