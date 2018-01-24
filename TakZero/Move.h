#pragma once

#include <vector>

enum Piece
{
	Empty =0,
	White_Flat =2,
	Black_Flat =3,
	White_Standing =4,
	Black_Standing =5,
	White_Capstone =6,
	Black_Capstone =7
};

enum PieceType {
	Flat = 2,
	Standing = 4,
	Capstone = 6
};

enum MoveType
{
	Placement,
	MoveStack
};

enum Direction
{
	Up,
	Right,
	Down,
	Left
};

enum Player {
	White,
	Black,
	Tie
};

struct Location
{
	uint8_t x;
	uint8_t y;
};

struct Play
{
	MoveType type;
	uint16_t index;
	PieceType piece;
	Location location;
	Location start;
	Location end;
	std::vector<uint8_t> order;
};