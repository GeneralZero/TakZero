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

struct Location
{
	uint16_t x;
	uint16_t y;
};

struct Play
{
	MoveType type;
	uint16_t index;
	Piece piece;
	Location location;
	Direction direction;
	Location start;
	Location end;
	std::vector<Location> order;
};