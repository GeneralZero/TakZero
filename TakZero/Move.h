#pragma once

#include <vector>

enum Piece
{
	PieceEmpty=0,
	White_Flat = 2,
	Black_Flat = 3,
	White_Standing = 4,
	Black_Standing = 5,
	White_Capstone = 6,
	Black_Capstone = 7
};

enum Player {
	White= 0,
	Black = 1
};

enum PieceType
{
	PieceTypeEmpty = 0,
	Flat = 2,
	Standing = 4,
	Capstone = 6,
};

enum MoveType
{
	Empty,
	Placement,
	MoveStack
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