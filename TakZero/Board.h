#pragma once
#include <vector>
#include <array>
#include "Move.h"
#include "TimeControl.h"

typedef std::array< uint8_t, 5 * 5 * 32> FastBoard;

class Board
{
public:
	Board(uint8_t size);
	Board();
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
	void WinnerPlace(PieceType piece, Location location);
	void WinnerMove(std::vector<Location>);

	void PrintMove(Play move);

	bool ValidMove(int index);

	void UpdateTops(Location location);

	uint16_t GetPlayIndex(Play move);

	void WallCrush(Location current_square, std::vector<Piece>);

	uint8_t getArrayIndex(uint8_t x, uint8_t y);

	void PlayIndex(Play move);

	void PlayIndex(int index);

	//Index paramaters
	std::vector<uint8_t> distance_table;

	//Index functions
	uint16_t GetPlacementIndex(Play move);
	uint16_t GetMoveIndex(Play move);

	std::vector<std::vector<uint8_t>> GenerateMoveArrays(uint8_t distance, uint8_t to_move, bool cap);

	uint64_t get_hash();

	std::vector<Play> getAllPlays();

	Play GetMoveFromIndex(uint16_t move);

	void stop_clock(Player color);

	void start_clock(Player color);

	TimeControl & get_timecontrol();


	//Utility Finctions
	void print_board();
	void SaveFastBoard();

	std::array<FastBoard, 8> getMLData();

	std::array<std::vector<Piece>, 25> board;

private:
	std::vector<FastBoard> prev_boards;

	TimeControl m_timecontrol;
};
