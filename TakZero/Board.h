#pragma once
#include <vector>
#include <map>
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
	void PlayMove(Play move);
	void PlayPlace(Play move);
	void WinnerPlace(Piece piece, Location location);
	void WinnerMove(std::vector<Location>);

	void UpdateTops(Location location);

	void CheckWallCrush(Location current_square, std::vector<Piece>);

	//Index paramaters
	std::vector<uint8_t> distance_table;

	//Index functions
	uint16_t GetTurnIndex(Play move);
	uint16_t GetPlacementIndex(Play move);
	uint16_t GetMoveIndex(Play move);

	//Monte Carlo functions
	Board* clone();
	std::vector<Play> get_all_plays();

	//Utility Finctions
	uint16_t GetIndexFromLocation(Location location);
	Location * GetLocationFromIndex(uint16_t index);
	void print_board();

private:
	std::map<uint8_t, Cell> board;


};
