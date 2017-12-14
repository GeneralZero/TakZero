#include "board.h"


Board::Board(uint8_t size)
{
	
	this->SIZE = size;
	this->white_turn = true;
	this->white_win = false;
	this->black_win = false;
	this->move_number = 0;

	if (this->SIZE == 5) {
		this->white_capstones = 1;
		this->black_capstones = 1;
		this->white_pieces = 21;
		this->black_pieces = 21;
		this->distance_table = { 0, 5, 15, 25, 30 };
	}
}

Board::~Board()
{
}

Board * Board::clone()
{
	return nullptr;
}
