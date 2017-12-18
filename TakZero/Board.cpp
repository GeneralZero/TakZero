#include "board.h"


Board::Board(uint8_t size)
{
	
	this->SIZE = size;
	this->white_turn = true;
	this->white_win = false;
	this->black_win = false;
	this->move_number = 0;

	if (this->SIZE == 5) {
		this->white_capstones = 1;
		this->black_capstones = 1;
		this->white_pieces = 21;
		this->black_pieces = 21;
		this->distance_table = { 0, 5, 15, 25, 30 };
	}
}

Board::~Board()
{
}

void Board::PlayMove(Play move)
{

}

void Board::PlayPlace(Play move)
{
}

void Board::WinnerPlace(Piece piece, Location location)
{
}

void Board::WinnerMove(std::vector<Location>)
{
}

void Board::UpdateTops(Location location)
{
}

void Board::CheckWallCrush(Location current_square, std::vector<Piece>)
{
}

uint16_t Board::GetTurnIndex(Play move)
{
	if (move.type == Placement) {
		return GetPlacementIndex(move);
	}
	else if (move.type == MoveStack) {
		return GetMoveIndex(move);
	}
	else {
		throw std::invalid_argument("Invalid Move");
	}
}

uint16_t Board::GetPlacementIndex(Play move) {
	uint16_t offset = (move.location.x + move.location.y * this->SIZE) * 3;

	if (move.piece == White_Standing || move.piece == Black_Standing) {
		return 1500 + offset + 1;
	}
	else if (move.piece == White_Capstone || move.piece == Black_Capstone) {
		return 1500 + offset + 2;
	}
	else {
		return 1500 + offset;
	}
}

uint16_t Board::GetMoveIndex(Play move) {
	uint16_t offset = (move.location.x + move.location.y * this->SIZE) * 60;
	uint16_t offset = 0;

	uint8_t moves_up    = this->distance_table[move.location.y];
	uint8_t moves_right = this->distance_table[this->SIZE - 1 + move.location.x];
	uint8_t moves_down  = this->distance_table[this->SIZE - 1 + move.location.y];
	uint8_t moves_left  = this->distance_table[move.location.x];

	switch (move.direction)
	{
	case Up:
		break;

	case Right:
		break;

	case Down:
		break;

	case Left:
		break;

	default:
		break;
	}
}

std::vector<std::vector<uint8_t>> GenerateMoveArrays(uint8_t distance, uint8_t to_move, bool cap) {
	if (distance == 0) {
		if (cap) {
			return std::vector<std::vector<uint8_t>>{{1}};
		}
		return std::vector<std::vector<uint8_t>>{{}};
	}

	if (to_move == 0) {
		return std::vector<std::vector<uint8_t>>{{}};
	}

	if (to_move == 1) {
		return std::vector<std::vector<uint8_t>>{ {1}};
	}

	if (distance == 1) {
		switch (to_move)
		{
		case 2:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, {1,1}};
			}
			return std::vector<std::vector<uint8_t>>{{1}, {2}};
		case 3:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 1, 1 }, {2,1}};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }};

		case 4:
			if (cap)
			{
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 2,1 }, {3,1}};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, {4}};

		default:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 } };
			}
			return std::vector<std::vector<uint8_t>>{ { 1 }, { 2 }, { 3 }, { 4 }, { 5 }};
		}
	}

	if (to_move == 2) {
		return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 1, 1 },};
	}

	if (distance == 2) {
		switch (to_move)
		{
		case 3:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 1, 1 }, { 1,2 }, { 2,1 }, { 1,1,1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 1, 1 }, { 1,2 }, { 2,1 }};

		case 4:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1,2 }, { 1,3 },
				{ 2, 1 }, { 2,2 }, { 3, 1 }, { 1,1,1 }, { 1,2,1 }, {2,1,1}};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1,2 }, { 1,3 }, { 2, 1 }, {2,2}, { 3, 1 }};

		default:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1,2 }, { 1,3 }, { 1,4 },
				{ 2, 1 }, { 2,2 }, { 2,3 }, { 3, 1 }, { 3,2 }, { 4,1 }, { 1, 1, 1 }, { 1,1,2 }, { 1,1,3 }, { 1, 2, 1 }, { 1,2,2 },
				{ 1,3,1 }, { 2, 1, 1 }, { 2,1,2 }, { 2,2,1 }, { 3,1,1 }, { 1,1,1,1 }, { 1,1,2,1 }, { 1,2,1,1 }, {2,1,1,1}};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1,2 }, { 1,3 }, { 1,4 },
			{ 2, 1 }, { 2,2 }, { 2,3 }, { 3, 1 }, { 3,2 }, { 4,1 }, { 1, 1, 1 }, { 1,1,2 }, { 1,1,3 }, { 1, 2, 1 }, { 1,2,2 },
			{ 1,3,1 }, { 2, 1, 1 }, { 2,1,2 }, { 2,2,1 }, { 3,1,1 }};
		}
	}

	if (to_move == 3) {
		return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 1, 1 }, { 1,2 }, { 2,1 }, { 1,1,1 }};
	}

	if (distance == 3) {
		switch (to_move)
		{
		case 4:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 2, 1 }, { 2, 2 }, { 3, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 2, 1 }, { 2, 1, 1 }, { 1, 1, 1, 1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 2, 1 }, { 2, 2 }, { 3, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 2, 1 }, { 2, 1, 1 }};

		default:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 }, { 3, 2 }, { 4, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 }, { 1, 2, 1 }, { 1, 2, 2 }, { 1, 3, 1 }, { 2, 1, 1 }, { 2, 1, 2 }, { 2, 2, 1 }, { 3, 1, 1 }, { 1, 1, 1, 1 }, { 1, 1, 2, 1 }, { 1, 2, 1, 1 }, { 2, 1, 1, 1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 }, { 3, 2 }, { 4, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 }, { 1, 2, 1 }, { 1, 2, 2 }, { 1, 3, 1 }, { 2, 1, 1 }, { 2, 1, 2 }, { 2, 2, 1 }, { 3, 1, 1 }};
		}
	}

	if (to_move == 4) {
		return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 2, 1 }, { 2, 2 }, { 3, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 2, 1 }, { 2, 1, 1 }, { 1, 1, 1, 1 }};
	}

	else{
		return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 }, { 3, 2 }, { 4, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 }, { 1, 2, 1 }, { 1, 2, 2 }, { 1, 3, 1 }, { 2, 1, 1 }, { 2, 1, 2 }, { 2, 2, 1 }, { 3, 1, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 2 }, { 1, 1, 2, 1 }, { 1, 2, 1, 1 }, { 2, 1, 1, 1 }};
	}

}

Board * Board::clone()
{
	Board cloned = Board(this->SIZE);

	cloned.white_turn = this->white_turn;
	cloned.white_win = this->white_win;
	cloned.black_win = this->black_win;
	cloned.move_number = this->move_number;
	cloned.white_capstones = this->white_capstones;
	cloned.black_capstones = this->black_capstones;
	cloned.white_pieces = this->white_pieces;
	cloned.black_pieces = this->black_pieces;
	cloned.distance_table = this->distance_table;

	//Clone Board

	return &cloned;
}

std::vector<Play> Board::get_all_plays()
{
	return std::vector<Play>();
}

uint16_t Board::GetIndexFromLocation(Location location)
{
	return uint16_t();
}

Location * Board::GetLocationFromIndex(uint16_t index)
{
	return nullptr;
}

void Board::print_board()
{
}
