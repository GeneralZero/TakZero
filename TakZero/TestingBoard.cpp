#include "TestingBoard.h"
#include "Board.h"

void TestCapstoneCrush()
{
}

void TestFullGame()
{
}

void TestGenerateMoves()
{
}

void TestIndexGeneration()
{
}

void TestPartGame1()
{
}

void TestFullMoveGeneration()
{
}

void TestInvalidMove()
{
}

void TestMoveGeneration1()
{
	Board game(5);

	game.board[0].push_back(White_Standing);

	game.board[1].push_back(Black_Standing);

	game.board[2].push_back(Black_Standing);

	game.board[3].push_back(White_Flat);

	game.board[5].push_back(Black_Flat);
	game.board[5].push_back(Black_Standing);

	game.board[7].push_back(Black_Standing);

	game.board[8].push_back(Black_Flat);
	game.board[8].push_back(White_Flat);
	game.board[8].push_back(White_Standing);

	game.board[9].push_back(Black_Flat);

	game.board[10].push_back(Black_Standing);

	game.board[11].push_back(Black_Flat);
	game.board[11].push_back(Black_Flat);

	game.board[12].push_back(White_Flat);
	game.board[12].push_back(White_Standing);

	game.board[14].push_back(Black_Flat);
	game.board[14].push_back(Black_Flat);
	game.board[14].push_back(Black_Flat);
	game.board[14].push_back(White_Capstone);

	game.board[15].push_back(Black_Standing);

	game.board[16].push_back(Black_Flat);
	game.board[16].push_back(White_Standing);

	game.board[17].push_back(White_Flat);
	game.board[17].push_back(White_Flat);
	game.board[17].push_back(White_Flat);
	game.board[17].push_back(White_Flat);
	game.board[17].push_back(Black_Flat);

	game.board[18].push_back(White_Standing);

	game.board[20].push_back(Black_Flat);
	game.board[20].push_back(Black_Standing);

	game.board[21].push_back(Black_Flat);
	game.board[21].push_back(White_Standing);

	game.board[22].push_back(Black_Standing);

	game.board[23].push_back(Black_Standing);

	game.board[24].push_back(White_Standing);

	game.move_number = 50;
	game.white_capstones = 0;
	game.white_turn = false;

	auto moves = game.getAllPlays();

	game.print_board();
}

void TestMoveGeneration2()
{
}
