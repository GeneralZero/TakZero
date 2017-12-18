#include "Board.h"

#include <string>
#include <iostream>

#include "TestingBoard.h"

void TestCapstoneCrush() {
	Board game(5);

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 3,1 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 2,2 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Capstone, Location{ 3,2 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Capstone, Location{ 2,1 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Standing, Location{ 3,0 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Standing, Location{ 1,1 } });

	game.print_board();

	game.PlayMove(Play{ MoveStack, 0, (PieceType)0, Location{}, Location{ 3,0 }, Location{ 3,1 }, std::vector<uint8_t>{1} });

	game.print_board();

	game.PlayMove(Play{ MoveStack, 0, (PieceType)0, Location{}, Location{ 2,1 }, Location{ 1,1 }, std::vector<uint8_t>{1} });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 2,1 } });

	game.print_board();

	game.PlayMove(Play{ MoveStack, 0, (PieceType)0, Location{}, Location{ 1,1 }, Location{ 3,1 }, std::vector<uint8_t>{1, 1} });

	game.print_board();
}

void TestGenerateMoves() {
	Board game(5);

	auto test = game.getAllPlays();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 3,1 } });

	test = game.getAllPlays();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 2,2 } });

	test = game.getAllPlays();

	game.PlayPlace(Play{ Placement, 0, Capstone, Location{ 3,2 } });

	test = game.getAllPlays();

	game.PlayPlace(Play{ Placement, 0, Capstone, Location{ 2,1 } });

	test = game.getAllPlays();

	game.PlayPlace(Play{ Placement, 0, Standing, Location{ 3,0 } });

	test = game.getAllPlays();

	game.PlayPlace(Play{ Placement, 0, Standing, Location{ 1,1 } });
}

void TestIndexGeneration() {
	Board game(5);

	Play move = Play{ Placement, 0, Flat, Location{ 0,0 } };

	game.PlayPlace(move);

	move = Play{ MoveStack, 0, (PieceType)0, Location{}, Location{ 0,0 }, Location{ 1,0 }, std::vector<uint8_t>{1} };

	auto index = game.GetMoveIndex(move);

	Play move2 = game.GetMoveFromIndex(index);

	game.PlayMove(move);
}

void TestPartGame1() {
	Board game(5);
	
	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 4,4 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 4,3 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 4,2 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 4,1 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 4,0 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 3,4 } });

	std::cout << game.white_win << game.black_win;
}

void TestFullGame() {
	Board game(5);
	
	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 4,4 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 3,4 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 3,3 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 3,2 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 2,3 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 4,3 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 4,2 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 3,1 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 1,3 } });

	game.print_board();

	game.PlayMove(Play{ MoveStack, 0, (PieceType)0, Location{}, Location{ 3,2 }, Location{ 4,2 }, std::vector<uint8_t>{1} });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Capstone, Location{ 3,2 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 4,1 } });

	game.print_board();

	game.PlayMove(Play{ MoveStack, 0, (PieceType)0, Location{}, Location{ 3,2 }, Location{ 4,2 }, std::vector<uint8_t>{1} });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 0,3 } });

	game.print_board();

	game.PlayMove(Play{ MoveStack, 0, (PieceType)0, Location{}, Location{ 4,2 }, Location{ 4,4 }, std::vector<uint8_t>{1, 2} });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 0,2 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 0,4 } });

	game.print_board();

	game.PlayMove(Play{ MoveStack, 0, (PieceType)0, Location{}, Location{ 0,3 }, Location{ 1,3 }, std::vector<uint8_t>{1} });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 0,3 } });

	game.print_board();

	game.PlayMove(Play{ MoveStack, 0, (PieceType)0, Location{},  Location{ 0,2 }, Location{ 0,3 }, std::vector<uint8_t>{1} });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 2,2 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 1,2 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 1,1 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 2,1 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Flat, Location{ 1,4 } });

	game.print_board();

	game.PlayPlace(Play{ Placement, 0, Standing, Location{ 2,4 } });

	game.print_board();

	game.PlayMove(Play{ MoveStack, 0, (PieceType)0, Location{}, Location{ 4,4 }, Location{ 2,4 }, std::vector<uint8_t>{2,1} });

	game.print_board();

	std::cout << game.white_win << game.black_win;
}