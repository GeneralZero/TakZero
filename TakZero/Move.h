#pragma once

#include <vector>

enum Piece
{
	Empty,
	White_Flat,
	Black_Flat,
	White_Standing,
	Black_Standing,
	White_Capstone,
	Black_Capstone
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
	Black
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