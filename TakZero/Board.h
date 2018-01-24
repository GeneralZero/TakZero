#pragma once
#include <vector>
#include <array>

//#include "DLNetwork.h"
#include "Move.h"
#include "TimeControl.h"

typedef std::array< std::uint8_t, 5 * 5 * 32> FastBoard;

class Board
{
public:
	Board(std::uint8_t size);
	Board();
	~Board();
	
	//Game paramaters
	std::uint8_t SIZE;
	std::uint8_t white_capstones;
	std::uint8_t black_capstones;
	std::uint8_t white_pieces;
	std::uint8_t black_pieces;
	bool white_turn;
	std::uint8_t move_number;
	bool white_win;
	bool black_win;

	//Game functions
	void PlayMove(Play move);
	void PlayPlace(Play move);
	void WinnerPlace(PieceType piece, Location location);
	void WinnerMove(std::vector<Location>);
	void WallCrush(Location current_square, std::vector<Piece>);
	bool ValidMove(int index);
	void UpdateTops(Location location);

	void PrintMove(Play move);
	
	std::uint16_t GetPlayIndex(Play move);
	
	std::uint8_t getArrayIndex(std::uint8_t x, std::uint8_t y);

	//Index paramaters
	void PlayIndex(Play move);
	void PlayIndex(int index);

	
	std::vector<std::uint8_t> distance_table;

	//Index functions
	std::uint16_t GetPlacementIndex(Play move);
	std::uint16_t GetMoveIndex(Play move);
	std::vector<std::vector<std::uint8_t>> GenerateMoveArrays(std::uint8_t distance, std::uint8_t to_move, bool cap);

	std::uint64_t get_hash();

	std::vector<Play> getAllPlays();

	Play GetMoveFromIndex(std::uint16_t move);

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
