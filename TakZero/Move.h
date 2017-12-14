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

typedef uint8_t Location[2];

struct Play
{
	uint16_t index;
};


struct Placement : Play
{
	Piece piece;
	Location location[2];

};

struct Move : Play
{
	Direction direction;
	Location start[2];
	Location end[2];
	std::vector<Location> order;
};