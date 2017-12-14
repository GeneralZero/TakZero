#pragma once
#include <vector>
#include "Cell.h"
#include "Move.h"

class Board
{
public:
	Board(uint8_t size);
	~Board();
	
	//Game paramaters
	uint8_t SIZE;
	uint8_t white_capstones;
	uint8_t black_capstones;
	uint8_t white_pieces;
	uint8_t black_pieces;
	bool white_turn;
	uint8_t move_number;
	bool white_win;
	bool black_win;

	//Game functions
	void move(Play move);
	void place(Play move);
	void winner_place(Piece piece, Location location);
	void winner_move(std::vector<Location>);

	void update_tops(Location location);

	void check_wall_crush(Location current_square, std::vector<Piece>);

	//Index paramaters
	std::vector<uint8_t> distance_table;

	//Index functions
	uint16_t get_turn_index(Play move);

	//Monte Carlo functions
	Board* clone();
	std::vector<Play> get_all_plays();



private:
	std::vector<Cell> board;


};
