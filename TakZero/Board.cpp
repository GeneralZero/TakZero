#include "board.h"
#include "Zobrist.h"

#include <algorithm>
#include <queue>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <iterator>
#include <assert.h>

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

	//Create Board
	std::fill(this->board.begin(), this->board.end(), std::vector<Piece>{0});
	
	//Populate prevboards
	this->SaveFastBoard();
	this->SaveFastBoard();
	this->SaveFastBoard();
	this->SaveFastBoard();
	this->SaveFastBoard();
	this->SaveFastBoard();
}

Board::Board()
{
	Board(5);
}

Board::~Board()
{
	//Clean up prev_boards
	/*
	for each (auto prev in this->prev_boards)
	{
		for (uint8_t i = 0; i < (this->SIZE * this->SIZE); i++)
		{
			delete prev[i];
		}
		delete prev;
	}
	*/

}

void Board::SaveFastBoard() {
	auto new_board = std::array< uint8_t, 5 * 5 * 32>{0};
	for (uint8_t i = 0; i < this->SIZE * this->SIZE; i++)
	{
		uint8_t j = 0;
		uint8_t to_put = 0;

		for (; j < this->board[i].size(); j++)
		{
			uint32_t index = (i * j);
			if (i % 2 == 0) {
				to_put = ((7 & this->board[i].at(j)) << 4);

				if (this->board[i].size() == j+1) {
					new_board.at(index) = to_put;
				}
			}
			else {
				to_put += ((7 & this->board[i].at(j)));
				new_board.at(index) = to_put;
				to_put = 0;
			}
			
		}
	}
	this->prev_boards.push_back(new_board);
}

std::array<FastBoard, 8> Board::getMLData() {
	auto start = this->prev_boards.size() - 1;
	auto end = start - 5;

	auto ret = std::array<FastBoard, 8>{};
	int j = 0;
	for (int i = start; i > end; ) {
		ret[j] = this->prev_boards[i];
		i--;
		j++;
	}
	if (this->white_turn) {
		ret[j].fill(0);
		j++;
		ret[j].fill(1);
	}
	else {
		ret[j].fill(0);
		j++;
		ret[j].fill(1);
	}
	return ret;
}


void Board::PlayMove(Play move)
{
	uint16_t count = 0;
	std::vector<Location> to_check;

	for each (auto var in move.order)
	{
		count += var;
	}
	if (count > this->SIZE ) {
		throw std::invalid_argument("Moving more tiles than board size");
	}

	auto current_square = move.start;
	to_check.push_back(move.start);

	//Validate Move
	if (move.start.x == move.end.x) {
		//Move Up and Down
		if (move.start.y > move.end.y) {
			//Down
			std::vector<Piece> pop_array;
			std::vector<Piece>* cell = &this->board[current_square.x + current_square.y *this->SIZE];

			if (cell->size() == 0) {
				throw std::invalid_argument("Cannot Move a empty cell");
			}

			for (uint8_t i = 0; i < count; i++)
			{
				pop_array.push_back(cell->back());
				cell->pop_back();
			}

			for (uint8_t i = 0; i < move.order.size(); i++)
			{
				current_square.y -= 1;
				to_check.push_back(current_square);
				cell = &this->board[current_square.x + current_square.y *this->SIZE];

				//Check for capstone wall crush 
				if (move.order.size() -1 == i && move.order[i] == 1 && pop_array.size() > 0) {
					WallCrush(current_square, pop_array);
				}

				for (uint8_t j = 0; j < move.order[i]; j++)
				{
					cell->push_back(pop_array.back());
					pop_array.pop_back();
				}

				//Update Black and White
			}
			
		}
		else {
			//Up
			std::vector<Piece> pop_array;
			std::vector<Piece>* cell = &this->board[current_square.x + current_square.y *this->SIZE];

			for (uint8_t i = 0; i < count; i++)
			{
				pop_array.push_back(cell->back());
				cell->pop_back();
			}

			for (uint8_t i = 0; i < move.order.size(); i++)
			{
				current_square.y += 1;
				to_check.push_back(current_square);
				cell = &this->board[current_square.x + current_square.y *this->SIZE];

				//Check for capstone wall crush 
				if (move.order.size() - 1 == i && move.order[i] == 1 && pop_array.size() > 0) {
					WallCrush(current_square, pop_array);
				}

				for (uint8_t j = 0; j < move.order[i]; j++)
				{
					cell->push_back(pop_array.back());
					pop_array.pop_back();
				}

				//Update Black and White
			}
		}

	}
	else if (move.start.y == move.end.y) {
		//Left and Right
		if (move.start.x > move.end.x) {
			//Left
			std::vector<Piece> pop_array;
			std::vector<Piece>* cell = &this->board[current_square.x + current_square.y *this->SIZE];

			for (uint8_t i = 0; i < count; i++)
			{
				pop_array.push_back(cell->back());
				cell->pop_back();
			}

			for (uint8_t i = 0; i < move.order.size(); i++)
			{
				current_square.x -= 1;
				to_check.push_back(current_square);
				cell = &this->board[current_square.x + current_square.y *this->SIZE];

				//Check for capstone wall crush 
				if (move.order.size() - 1 == i && move.order[i] == 1 && pop_array.size() > 0) {
					WallCrush(current_square, pop_array);
				}

				for (uint8_t j = 0; j < move.order[i]; j++)
				{
					cell->push_back(pop_array.back());
					pop_array.pop_back();
				}

				//Update Black and White
			}
		}
		else {
			//Right
			std::vector<Piece> pop_array;
			std::vector<Piece>* cell = &this->board[current_square.x + current_square.y *this->SIZE];

			for (uint8_t i = 0; i < count; i++)
			{
				pop_array.push_back(cell->back());
				cell->pop_back();
			}

			for (uint8_t i = 0; i < move.order.size(); i++)
			{
				current_square.x += 1;
				to_check.push_back(current_square);
				cell = &this->board[current_square.x + current_square.y *this->SIZE];

				//Check for capstone wall crush 
				if (move.order.size() - 1 == i && move.order[i] == 1 && pop_array.size() > 0) {
					WallCrush(current_square, pop_array);
				}

				for (uint8_t j = 0; j < move.order[i]; j++)
				{
					cell->push_back(pop_array.back());
					pop_array.pop_back();
				}

				//Update Black and White
			}
		}
	}
	else {
		throw std::invalid_argument("Invalid Move start and end.");
	}

	//Update_tops from to_check

	//Update prevboards
	//this->prev_boards.push_back(this->CreateFastBoard());

	//Check for winner
	bool white_turn = this->white_turn;
	WinnerMove(to_check);

	//Change Turn
	this->white_turn = !white_turn;
	this->move_number += 1;

}

void Board::WallCrush(Location current_square, std::vector<Piece> pop_array) {
	std::vector<Piece>* cell = &this->board[current_square.x + current_square.y *this->SIZE];

	//If cell empty then cant crush
	if (cell->size() == 0){
		return;
	}

	if ((pop_array[0] & Capstone) == Capstone && (cell->back() & Capstone) == Standing ) {
		//Capstone Crush
		cell->back() = (Piece)((uint8_t)(cell->back() & 1) + 2);
	}
}

uint8_t Board::getArrayIndex(uint8_t x, uint8_t y) {
	return x + y * this->SIZE;
}

void Board::PlayIndex(Play move) {
	if (move.type == Placement) {
		return this->PlayPlace(move);
	}
	else if (move.type == MoveStack) {
		return this->PlayMove(move);
	}
}

void Board::PlayIndex(int index) {
	Play move = GetMoveFromIndex(index);

	if (move.type == Placement) {
		return this->PlayPlace(move);
	}
	else if (move.type == MoveStack) {
		return this->PlayMove(move);
	}
}


void Board::PlayPlace(Play move)
{
	if (move.type == MoveStack) {
		throw std::invalid_argument("Cannot place a Move");
	}

	std::vector<Piece>* cell = &this->board[move.location.x + move.location.y *this->SIZE];
	bool is_white;

	if (cell->size() == 0) {
		//First move reversed piece
		if (this->move_number == 0) {
			is_white = false;
			cell->push_back((Piece)(Flat + (uint8_t)(is_white == false)));
			this->black_pieces--;
		}
		else if (this->move_number == 1) {
			is_white = true;
			cell->push_back((Piece)(Flat + (uint8_t)(is_white == false)));
			this->white_pieces--;
		}
		else {
			is_white = this->white_turn;

			if (move.piece == Flat) {
				cell->push_back((Piece)(Flat + (uint8_t)(is_white == false)));
				WinnerPlace(Flat, move.location);

				//Update Piece totals
				if (is_white) {
					this->white_pieces--;
				}
				else{
					this->black_pieces--;
				}
			}
			else if (move.piece == Standing) {
				cell->push_back((Piece)(Standing + (uint8_t)(is_white == false)));
				
				//Update Piece totals
				if (is_white) {
					this->white_pieces--;
				}
				else {
					this->black_pieces--;
				}
			}
			else {
				if (this->white_turn) {
					this->white_capstones -= 1;
					cell->push_back(White_Capstone);
				}
				else {
					this->black_capstones -= 1;
					cell->push_back(Black_Capstone);
				}

				WinnerPlace(Capstone, move.location);
			}
		}

		//Update Tops

		//Check Endgame conditions
		if (this->move_number >= this->SIZE*this->SIZE -1) {
			uint8_t empty_cells = 0;
			uint8_t white_top_count = 0;
			uint8_t black_top_count = 0;
			for (uint8_t i = 0; i < this->board.size(); i++)
			{
				if (this->board[i].size() == 0) {
					empty_cells++;
				}
				else if ((this->board[i].back() & Black) == White && (this->board[i].back() & Capstone) != Standing) {
					white_top_count++;
				}
				else if ((this->board[i].back() & Black) == Black && (this->board[i].back() & Capstone) != Standing) {
					black_top_count++;
				}
			}

			if (this->white_pieces == 0 || this->black_pieces == 0 || empty_cells == 0) {
				if (white_top_count > black_top_count) {
					this->white_win = true;
				}
				else if (white_top_count < black_top_count) {
					this->black_win = true;
				}
				else {
					this->white_win = true;
					this->black_win = true;
				}
			}
		}

		//Update prevboards
		//this->prev_boards.push_back(CreateFastBoard());

		//Change Turn
		this->white_turn = !this->white_turn;
		//Increase Movenumber
		this->move_number++;

	}
	else {
		throw std::invalid_argument("Invalid Placement Location.");
	}
}

void Board::WinnerPlace(PieceType piece, Location location)
{
	//Cannot win on placing a standing stone
	if (piece != Standing) {

		uint8_t min_x = location.x;
		uint8_t min_y = location.y;

		uint8_t max_x = location.x;
		uint8_t max_y = location.y;

		std::queue<Location> to_check;
		std::vector<Location> visited;

		to_check.push(location);

		while (!to_check.empty()){
			//Add location to visited
			Location current_location = to_check.front();
			to_check.pop();

			visited.push_back(current_location);

			std::vector<Piece>* cell = &this->board[current_location.x + current_location.y *this->SIZE];

			//Check turn and matching top
			if (cell->size() != 0 && ((cell->back() & Capstone) != Standing)) {

				if ((this->white_turn && ((cell->back() & Black) == White)) || \
					(!this->white_turn && ((cell->back() & Black) == Black))) {

					Location new_location_left{ current_location.x - 1, current_location.y };

					auto found = std::find_if(visited.begin(), visited.end(), [new_location_left](const Location& d) {
						return ((d.x == new_location_left.x) && (d.y == new_location_left.y));
					});

					if (current_location.x - 1 >= 0 && found == visited.end()) {
						to_check.push(new_location_left);
						visited.push_back(new_location_left);
					}

					Location new_location_up{ current_location.x, current_location.y - 1 };

					found = std::find_if(visited.begin(), visited.end(), [new_location_up](const Location& d) {
						return ((d.x == new_location_up.x) && (d.y == new_location_up.y));
					});

					if (current_location.y - 1 >= 0 && found == visited.end()) {
						to_check.push(new_location_up);
						visited.push_back(new_location_up);
					}

					Location new_location_right{ current_location.x + 1, current_location.y };

					found = std::find_if(visited.begin(), visited.end(), [new_location_right](const Location& d) {
						return ((d.x == new_location_right.x) && (d.y == new_location_right.y));
					});

					if (current_location.x + 1 < this->SIZE && found == visited.end()) {
						to_check.push(new_location_right);
						visited.push_back(new_location_right);
					}

					Location new_location_down{ current_location.x, current_location.y + 1 };

					found = std::find_if(visited.begin(), visited.end(), [new_location_down](const Location& d) {
						return ((d.x == new_location_down.x) && (d.y == new_location_down.y));
					});

					if (current_location.y + 1 < this->SIZE && found == visited.end()) {
						to_check.push(new_location_down);
						visited.push_back(new_location_down);
					}

					min_x = std::min(min_x, current_location.x);
					min_y = std::min(min_y, current_location.y);

					max_x = std::max(max_x, current_location.x);
					max_y = std::max(max_y, current_location.y);

				}
			}
		}

		//Check for a Win
		if ((max_x - min_x + 1) == this->SIZE || (max_y - min_y + 1) == this->SIZE) {
			if (this->white_turn) {
				this->white_win = true;
			}
			else {
				this->black_win = true;
			}
		}

	}
}

void Board::WinnerMove(std::vector<Location> locations)
{
	bool white_turn = true;
	//Cannot win on placing a standing stone

	
	for each (Location location in locations)
	{
		uint8_t min_x = location.x;
		uint8_t min_y = location.y;

		uint8_t max_x = location.x;
		uint8_t max_y = location.y;

		std::queue<Location> to_check;
		std::vector<Location> visited;

		to_check.push(location);

		while (!to_check.empty()) {
			//Add location to visited
			Location current_location = to_check.front();
			to_check.pop();

			visited.push_back(current_location);

			std::vector<Piece>* cell = &this->board[current_location.x + current_location.y *this->SIZE];

			//White check turn and matching top
			if (cell->size() != 0 && ((cell->back() & Capstone) != Standing)) {
				if ((cell->back() & Black) == White) {

					Location new_location_left{ current_location.x - 1, current_location.y };

					auto found = std::find_if(visited.begin(), visited.end(), [new_location_left](const Location& d) {
						return ((d.x == new_location_left.x) && (d.y == new_location_left.y));
					});

					if (current_location.x - 1 >= 0 && found == visited.end()) {
						to_check.push(new_location_left);
						visited.push_back(new_location_left);
					}

					Location new_location_up{ current_location.x, current_location.y - 1 };

					found = std::find_if(visited.begin(), visited.end(), [new_location_up](const Location& d) {
						return ((d.x == new_location_up.x) && (d.y == new_location_up.y));
					});

					if (current_location.y - 1 >= 0 && found == visited.end()) {
						to_check.push(new_location_up);
						visited.push_back(new_location_up);
					}

					Location new_location_right{ current_location.x + 1, current_location.y };

					found = std::find_if(visited.begin(), visited.end(), [new_location_right](const Location& d) {
						return ((d.x == new_location_right.x) && (d.y == new_location_right.y));
					});

					if (current_location.x + 1 < this->SIZE && found == visited.end()) {
						to_check.push(new_location_right);
						visited.push_back(new_location_right);
					}

					Location new_location_down{ current_location.x, current_location.y + 1 };

					found = std::find_if(visited.begin(), visited.end(), [new_location_down](const Location& d) {
						return ((d.x == new_location_down.x) && (d.y == new_location_down.y));
					});

					if (current_location.y + 1 < this->SIZE && found == visited.end()) {
						to_check.push(new_location_down);
						visited.push_back(new_location_down);
					}

					min_x = std::min(min_x, current_location.x);
					min_y = std::min(min_y, current_location.y);

					max_x = std::max(max_x, current_location.x);
					max_y = std::max(max_y, current_location.y);

				}
			}
		}

		//Check for a Win
		if ((max_x - min_x + 1) == this->SIZE || (max_y - min_y + 1) == this->SIZE) {
			this->white_win = true;
			return;
		}
	}

	white_turn = false;

	
	for each (Location location in locations)
	{
		
		uint8_t min_x = location.x;
		uint8_t min_y = location.y;

		uint8_t max_x = location.x;
		uint8_t max_y = location.y;

		std::queue<Location> to_check;
		std::vector<Location> visited;

		to_check.push(location);

		while (!to_check.empty()) {
			
			//Add location to visited
			Location current_location = to_check.front();
			to_check.pop();

			visited.push_back(current_location);
			
			std::vector<Piece>* cell = &this->board[current_location.x + current_location.y *this->SIZE];

			//Check turn and matching top
			if (cell->size() != 0 && ((cell->back() & Capstone) != Standing)) {
				if ((cell->back() & Black) == Black) {

					Location new_location_left{ current_location.x - 1, current_location.y };

					auto found = std::find_if(visited.begin(), visited.end(), [new_location_left](const Location& d) {
						return ((d.x == new_location_left.x) && (d.y == new_location_left.y));
					});

					if (current_location.x - 1 >= 0 && found == visited.end()) {
						to_check.push(new_location_left);
						visited.push_back(new_location_left);
					}

					Location new_location_up{ current_location.x, current_location.y - 1 };

					found = std::find_if(visited.begin(), visited.end(), [new_location_up](const Location& d) {
						return ((d.x == new_location_up.x) && (d.y == new_location_up.y));
					});

					if (current_location.y - 1 >= 0 && found == visited.end()) {
						to_check.push(new_location_up);
						visited.push_back(new_location_up);
					}

					Location new_location_right{ current_location.x + 1, current_location.y };

					found = std::find_if(visited.begin(), visited.end(), [new_location_right](const Location& d) {
						return ((d.x == new_location_right.x) && (d.y == new_location_right.y));
					});

					if (current_location.x + 1 < this->SIZE && found == visited.end()) {
						to_check.push(new_location_right);
						visited.push_back(new_location_right);
					}

					Location new_location_down{ current_location.x, current_location.y + 1 };

					found = std::find_if(visited.begin(), visited.end(), [new_location_down](const Location& d) {
						return ((d.x == new_location_down.x) && (d.y == new_location_down.y));
					});

					if (current_location.y + 1 < this->SIZE && found == visited.end()) {
						to_check.push(new_location_down);
						visited.push_back(new_location_down);
					}

					min_x = std::min(min_x, current_location.x);
					min_y = std::min(min_y, current_location.y);

					max_x = std::max(max_x, current_location.x);
					max_y = std::max(max_y, current_location.y);

				}
			}
			
		}

		//Check for a Black Win
		if ((max_x - min_x + 1) == this->SIZE || (max_y - min_y + 1) == this->SIZE) {
			this->black_win = true;
			return;
		}
		
	}
}

void Board::PrintMove(Play move)
{
	if (move.type == Placement) {
		printf("Play Placement: location=(%d,%d), Piece=[]", move.location.x, move.location.y, move.piece );
	}
	else {
		std::ostringstream oss;

		if (!move.order.empty()){
			// Convert all but the last element to avoid a trailing ","
			std::copy(move.order.begin(), move.order.end() - 1, std::ostream_iterator<int>(oss, ","));

			// Now add the last element with no delimiter
			oss << move.order.back();
		}
		printf("Play Move     : start=(%d,%d), end=(%d,%d), order=[] ", move.start.x, move.start.y, move.end.x, move.end.y, oss.str());
	}
	
}

bool Board::ValidMove(int index)
{
	Play move;

	//Get partial Move info
	if (index >= 1500) {
		move.type = Placement;
		uint16_t offset = index - 1500;

		uint8_t i = floorl(offset / 3);
		move.location.x = i % this->SIZE;
		move.location.y = (uint8_t)floorl(i / this->SIZE);
		move.piece = (PieceType)(offset - (i * 3));
	}
	else {
		move.type = MoveStack;
		uint16_t i = floorl(index / 60);
		uint16_t move_num = (index % 60);

		move.start.x = i % this->SIZE;
		move.start.y = (uint8_t)floorl(i / this->SIZE);
	}

	if (move.type == Placement) {
		if (this->white_turn == true && this->white_capstones ==0) {
			return false;
		}
		else if (this->white_turn == false && this->black_capstones == 0) {
			return false;
		}
		std::vector<Piece>* cell = &this->board[move.location.x + move.location.y *this->SIZE];
		if (cell->size() == 0) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (move.type == MoveStack) {
		std::vector<Piece>* cell = &this->board[move.start.x + move.start.y *this->SIZE];
		if (cell->size() != 0) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

void Board::UpdateTops(Location location)
{
	//Do I need?
}

uint16_t Board::GetPlayIndex(Play move)
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

	if ((move.piece & Capstone) == Standing) {
		return 1500 + offset + 1;
	}
	else if ((move.piece & Capstone) == Capstone) {
		return 1500 + offset + 2;
	}
	else {
		return 1500 + offset;
	}
}

uint16_t Board::GetMoveIndex(Play move) {
	uint16_t offset = (move.start.x + move.start.y * this->SIZE) * 60;
	uint16_t offset2 = 0;

	uint8_t moves_up    = this->distance_table[move.start.y];
	uint8_t moves_right = this->distance_table[this->SIZE - 1 - move.start.x];
	uint8_t moves_down  = this->distance_table[this->SIZE - 1 - move.start.y];
	uint8_t moves_left  = this->distance_table[move.start.x];

	auto move_arrays = GenerateMoveArrays(move.start.y, this->SIZE, false);

	//Validate Move
	if (move.start.x == move.end.x) {
		//Move Up and Down
		if (move.start.y > move.end.y) {
			//Up
			for (uint8_t i = 0; i < move_arrays.size(); i++)
			{
				if (move_arrays[i] == move.order) {
					offset2 += i;
					break;
				}
			}
		}
		else {
			//Down
			move_arrays = GenerateMoveArrays(this->SIZE - 1 - move.start.y, this->SIZE, false);
			offset2 = moves_up + moves_right;
			for (uint8_t i = 0; i < move_arrays.size(); i++)
			{
				if (move_arrays[i] == move.order) {
					offset2 += i;
					break;
				}
			}
		}
	}
	else if (move.start.y == move.end.y) {
		//Left and Right
		if (move.start.x > move.end.x) {
			//Left
			move_arrays = GenerateMoveArrays(move.start.x, this->SIZE, false);
			offset2 = moves_up + moves_right + moves_down;
			for (uint8_t i = 0; i < move_arrays.size(); i++)
			{
				if (move_arrays[i] == move.order) {
					offset2 += i;
					break;
				}
			}
		}

		else {
			//Right
			move_arrays = GenerateMoveArrays(this->SIZE - 1 - move.start.x, this->SIZE, false);
			offset2 = moves_up;
			for (uint8_t i = 0; i < move_arrays.size(); i++)
			{
				if (move_arrays[i] == move.order) {
					offset2 += i;
					break;
				}
			}
		}
	}
	else {
		throw std::invalid_argument("Invalid Move");
	}
	return offset + offset2;
}

std::vector<std::vector<uint8_t>> Board::GenerateMoveArrays(uint8_t distance, uint8_t to_move, bool cap) {
	if (distance == 0) {
		if (cap) {
			return std::vector<std::vector<uint8_t>>{ {1}};
		}
		return std::vector<std::vector<uint8_t>>{};
	}

	if (to_move == 0) {
		return std::vector<std::vector<uint8_t>>{};
	}

	if (to_move == 1) {
		return std::vector<std::vector<uint8_t>>{ {1}};
	}

	if (distance == 1) {
		switch (to_move)
		{
		case 2:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 1,1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }};
		case 3:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 1, 1 }, { 2,1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }};

		case 4:
			if (cap)
			{
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 2,1 }, { 3,1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }};

		default:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 } };
			}
			return std::vector<std::vector<uint8_t>>{ { 1 }, { 2 }, { 3 }, { 4 }, { 5 }};
		}
	}

	if (to_move == 2) {
		return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 1, 1 }, };
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
				{ 2, 1 }, { 2,2 }, { 3, 1 }, { 1,1,1 }, { 1,2,1 }, { 2,1,1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1,2 }, { 1,3 }, { 2, 1 }, { 2,2 }, { 3, 1 }};

		default:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1,2 }, { 1,3 }, { 1,4 },
				{ 2, 1 }, { 2,2 }, { 2,3 }, { 3, 1 }, { 3,2 }, { 4,1 }, { 1, 1, 1 }, { 1,1,2 }, { 1,1,3 }, { 1, 2, 1 }, { 1,2,2 },
				{ 1,3,1 }, { 2, 1, 1 }, { 2,1,2 }, { 2,2,1 }, { 3,1,1 }, { 1,1,1,1 }, { 1,1,2,1 }, { 1,2,1,1 }, { 2,1,1,1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1,2 }, { 1,3 }, { 1,4 },
			{ 2, 1 }, { 2,2 }, { 2,3 }, { 3, 1 }, { 3,2 }, { 4,1 }};
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

	else {
		return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 }, { 3, 2 }, { 4, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 }, { 1, 2, 1 }, { 1, 2, 2 }, { 1, 3, 1 }, { 2, 1, 1 }, { 2, 1, 2 }, { 2, 2, 1 }, { 3, 1, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 2 }, { 1, 1, 2, 1 }, { 1, 2, 1, 1 }, { 2, 1, 1, 1 }};
	}

}

uint64_t Board::get_hash()
{
	auto res = uint64_t{ 0x1234567887654321ULL };

	//Get Hash from board
	for (int i = 0; i < this->board.size(); i++) {
		for (size_t j = 0; j < this->board[i].size(); j++)
		{
			res ^= Zobrist::zobrist[this->board[i][j]][(i*64) + j];
		}
	}

	//Change Hash for turn
	if (!this->white_turn) {
		res ^= 0xABCDABCDABCDABCDULL;
	}

	return res;
}

std::vector<Play> Board::getAllPlays()
{
	std::vector<Play> ret;

	//First or second play
	if ( this->move_number <= 1 ) {
		for (uint8_t i = 0; i < this->board.size(); i++)
		{
			if (this->board[i].size() == 0 ) {
				Play temp;
				temp.type = Placement;
				temp.piece = Flat;
				temp.location.x = i % this->SIZE;
				temp.location.y = floorl(i / this->SIZE);
				temp.index = GetPlacementIndex(temp);
				ret.push_back(temp);
			}
		}
		return ret;
	}
	else {
		for (uint8_t i = 0; i < this->board.size(); i++)
		{
			if (!(this->board[i].size() == 0)) {
				//Add Moves
				bool cap = (board[i].back() & Capstone) == Capstone;

				//Get White tops
				if (this->white_turn) {
					if ((this->board[i].back() & Black) == White) {
						uint8_t distance = 0;
						uint8_t s_distance = 0;
						uint8_t to_move = std::min(this->SIZE, (uint8_t)this->board[i].size());

						//Down
						for (size_t j = (i + this->SIZE); j < this->SIZE * this->SIZE; j += this->SIZE)
						{
							if (this->board[j].size() == 0 || ((this->board[j].back() & Standing) != Standing)) {
								distance++;
							}
							else if (cap && (this->board[j].back() & Capstone) == Standing) {
								s_distance = distance + 1;
								break;
							}
							else {
								break;
							}
						}

						auto move_arrays = GenerateMoveArrays(distance, to_move, (cap & s_distance));
						for each(auto move in move_arrays) {
							Play temp;
							temp.type = MoveStack;
							temp.start = Location{ (uint8_t)(i % this->SIZE), (uint8_t)floorl(i / this->SIZE) };
							temp.end = Location{ (uint8_t)(i % this->SIZE) , (uint8_t)(floorl(i / this->SIZE) + move.size()) };
							temp.order = move;
							temp.index = this->GetMoveIndex(temp);

							ret.push_back(temp);
						}


						distance = 0;
						s_distance = 0;
						//Up
						for (int j = (i - this->SIZE); j >= 0; j -= this->SIZE)
						{
							if (this->board[j].size() == 0 || ((this->board[j].back() & Standing) != Standing)) {
								distance++;
							}
							else if (cap && (this->board[j].back() & Capstone) == Standing) {
								s_distance = distance + 1;
								break;
							}
							else {
								break;
							}
						}

						move_arrays = GenerateMoveArrays(distance, to_move, (cap & s_distance));
						for each(auto move in move_arrays) {
							Play temp;
							temp.type = MoveStack;
							temp.start = Location{ (uint8_t)(i % this->SIZE), (uint8_t)floorl(i / this->SIZE) };
							temp.end = Location{ (uint8_t)(i % this->SIZE) , static_cast<uint8_t>((uint8_t)(floorl(i / this->SIZE) - move.size())) };
							temp.order = move;
							temp.index = this->GetMoveIndex(temp);

							ret.push_back(temp);
						}

						distance = 0;
						s_distance = 0;
						//Left
						for (int j = (i - 1); j >= (i - i % this->SIZE); j--)
						{
							if (this->board[j].size() == 0 || ((this->board[j].back() & Standing) != Standing)) {
								distance++;
							}
							else if (cap && (this->board[j].back() & Capstone) == Standing) {
								s_distance = distance + 1;
								break;
							}
							else {
								break;
							}
						}

						move_arrays = GenerateMoveArrays(distance, to_move, (cap & s_distance));
						for each(auto move in move_arrays) {
							Play temp;
							temp.type = MoveStack;
							temp.start = Location{ (uint8_t)(i % this->SIZE), (uint8_t)floorl(i / this->SIZE) };
							temp.end = Location{ static_cast<uint8_t>((uint8_t)(i % this->SIZE) - move.size()) , (uint8_t)(floorl(i / this->SIZE)) };
							temp.order = move;
							temp.index = this->GetMoveIndex(temp);

							ret.push_back(temp);
						}

						distance = 0;
						s_distance = 0;
						//Right
						for (size_t j = (i + 1); j < (i - (i % this->SIZE) + this->SIZE); j++)
						{
							if (this->board[j].size() == 0 || ((this->board[j].back() & Standing) != Standing)) {
								distance++;
							}
							else if (cap && (this->board[j].back() & Capstone) == Standing) {
								s_distance = distance + 1;
								break;
							}
							else {
								break;
							}
						}

						move_arrays = GenerateMoveArrays(distance, to_move, (cap & s_distance));
						for each(auto move in move_arrays) {
							Play temp;
							temp.type = MoveStack;
							temp.start = Location{ (uint8_t)(i % this->SIZE), (uint8_t)floorl(i / this->SIZE) };
							temp.end = Location{ static_cast<uint8_t>((uint8_t)(i % this->SIZE) + (uint8_t)move.size()) , (uint8_t)(floorl(i / this->SIZE)) };
							temp.order = move;
							temp.index = this->GetMoveIndex(temp);

							ret.push_back(temp);
						}

					}
				}
				else {
					//Is Black move
					if ((this->board[i].back() & Black) == Black) {

						uint8_t distance = 0;
						uint8_t s_distance = 0;
						uint8_t to_move = (uint8_t)this->board[i].size();
						to_move = std::min(this->SIZE, to_move);

						//Down
						for (size_t j = (i + this->SIZE); j < this->SIZE * this->SIZE; j += this->SIZE)
						{
							if (this->board[j].size() == 0 || ((this->board[j].back() & Standing) != Standing)) {
								distance++;
							}
							else if (cap && (this->board[j].back() & Capstone) == Standing) {
								s_distance = distance + 1;
								break;
							}
							else {
								break;
							}
						}

						auto move_arrays = GenerateMoveArrays(distance, to_move, (cap & s_distance));
						for each(auto move in move_arrays) {
							Play temp;
							temp.type = MoveStack;
							temp.start = Location{ (uint8_t)(i % this->SIZE), (uint8_t)floorl(i / this->SIZE) };
							temp.end = Location{ (uint8_t)(i % this->SIZE) , (uint8_t)(floorl(i / this->SIZE) + move.size()) };
							temp.order = move;
							temp.index = this->GetMoveIndex(temp);

							ret.push_back(temp);
						}


						distance = 0;
						s_distance = 0;
						//Up
						for (int j = (i - this->SIZE); j >= 0; j -= this->SIZE)
						{
							if (this->board[j].size() == 0 || ((this->board[j].back() & Standing) != Standing)) {
								distance++;
							}
							else if (cap && (this->board[j].back() & Capstone) == Standing) {
								s_distance = distance + 1;
								break;
							}
							else {
								break;
							}
						}

						move_arrays = GenerateMoveArrays(distance, to_move, (cap & s_distance));
						for each(auto move in move_arrays) {
							Play temp;
							temp.type = MoveStack;
							temp.start = Location{ (uint8_t)(i % this->SIZE), (uint8_t)floorl(i / this->SIZE) };
							temp.end = Location{ (uint8_t)(i % this->SIZE) , (uint8_t)(floorl(i / this->SIZE) - move.size()) };
							temp.order = move;
							temp.index = this->GetMoveIndex(temp);

							ret.push_back(temp);
						}

						distance = 0;
						s_distance = 0;
						//Left
						for (int j = (i - 1); j >= (i - i % this->SIZE); j--)
						{
							if (this->board[j].size() == 0 || ((this->board[j].back() & Standing) != Standing)) {
								distance++;
							}
							else if (cap && (this->board[j].back() & Capstone) == Standing) {
								s_distance = distance + 1;
								break;
							}
							else {
								break;
							}
						}

						move_arrays = GenerateMoveArrays(distance, to_move, (cap & s_distance));
						for each(auto move in move_arrays) {
							Play temp;
							temp.type = MoveStack;
							temp.start = Location{ (uint8_t)(i % this->SIZE), (uint8_t)floorl(i / this->SIZE) };
							temp.end = Location{ static_cast<uint8_t>((uint8_t)(i % this->SIZE) - (uint8_t)move.size() ), (uint8_t)(floorl(i / this->SIZE)) };
							temp.order = move;
							temp.index = this->GetMoveIndex(temp);

							ret.push_back(temp);
						}

						distance = 0;
						s_distance = 0;
						//Right
						for (size_t j = (i + 1); j < (i - (i % this->SIZE) + this->SIZE); j++)
						{
							if (this->board[j].size() == 0 || ((this->board[j].back() & Standing) != Standing)) {
								distance++;
							}
							else if (cap && (this->board[j].back() & Capstone) == Standing) {
								s_distance = distance + 1;
								break;
							}
							else {
								break;
							}
						}

						move_arrays = GenerateMoveArrays(distance, to_move, (cap & s_distance));
						for each(auto move in move_arrays) {
							Play temp;
							temp.type = MoveStack;
							temp.start = Location{ (uint8_t)(i % this->SIZE), (uint8_t)floorl(i / this->SIZE) };
							temp.end = Location{ static_cast<uint8_t>((uint8_t)(i % this->SIZE) + (uint8_t)move.size()) , (uint8_t)(floorl(i / this->SIZE)) };
							temp.order = move;
							temp.index = this->GetMoveIndex(temp);

							ret.push_back(temp);
						}
					}
				}
			}

			else {
				//Add placements
				Play temp;
				Play temp1;

				temp.type = Placement;
				temp.piece = Flat;
				temp.location.x = i % this->SIZE;
				temp.location.y = floorl(i / this->SIZE);
				temp.index = GetPlacementIndex(temp);

				temp1.type = Placement;
				temp1.piece = Standing;
				temp1.location.x = temp.location.x;
				temp1.location.y = temp.location.y;
				temp1.index = temp.index + 1;

				if ((this->white_turn && this->white_capstones > 0) || (!this->white_turn && this->black_capstones > 0)) {
					Play temp2;
					temp2.type = Placement;
					temp2.piece = Capstone;

					temp2.location.x = temp.location.x;
					temp2.location.y = temp.location.y;
					temp2.index = temp.index + 2;

					ret.push_back(temp2);

					if ((this->white_turn && this->white_pieces == 0) || (!this->white_turn && this->black_pieces == 0)) {
						continue;
					}
				}

				ret.push_back(temp);
				ret.push_back(temp1);
			}
		}
	}

	return ret;
}

Play Board::GetMoveFromIndex(uint16_t move)
{
	Play ret;
	if (move >= 1500 ) {
		ret.type = Placement;
		ret.index = move;
		uint16_t offset = move - 1500;

		uint8_t i = floorl(offset / 3);
		ret.location.x = i % this->SIZE;
		ret.location.y = (uint8_t)floorl(i / this->SIZE);
		uint8_t p = (offset % 3);
		switch (p) {
		case 0:
			ret.piece = Flat;
			return ret;
		case 1:
			ret.piece = Standing;
			return ret;
		case 2:
			ret.piece = Capstone;
			return ret;
		}
	}
	else {
		ret.type = MoveStack;
		ret.index = move;
		uint16_t i = floorl(move / 60);
		uint16_t move_num = (move % 60);

		ret.start.x = i % this->SIZE;
		ret.start.y = (uint8_t)floorl(i / this->SIZE);

		//End
		uint8_t moves_up = this->distance_table[ret.start.y];
		uint8_t moves_right = this->distance_table[this->SIZE - 1 - ret.start.x];
		uint8_t moves_down = this->distance_table[this->SIZE - 1 - ret.start.y];
		uint8_t moves_left = this->distance_table[ret.start.x];

		//Move array
		if (move_num < moves_up) {
			//Move Up

			auto move_arrays = GenerateMoveArrays(ret.start.y, this->SIZE, false);

			ret.order = move_arrays.at(move_num);

			ret.end.x = ret.start.x;
			ret.end.y = ret.start.y - ret.order.size();

		}
		else if (move_num < (moves_up + moves_right)){
			//Move Right
			move_num -= moves_up;

			auto move_arrays = GenerateMoveArrays(this->SIZE - 1 - ret.start.x, this->SIZE, false);

			ret.order = move_arrays.at(move_num);

			ret.end.x = ret.start.x + ret.order.size();
			ret.end.y = ret.start.y;
		}
		else if (move_num < (moves_up + moves_right + moves_down)) {
			//Move Down
			move_num -= (moves_up + moves_right);

			auto move_arrays = GenerateMoveArrays(this->SIZE - 1 - ret.start.y, this->SIZE, false);

			ret.order = move_arrays.at(move_num);

			ret.end.x = ret.start.x;
			ret.end.y = ret.start.y + ret.order.size();
		}
		else {
			//Move Left
			move_num -= (moves_up + moves_right + moves_down);

			auto move_arrays = GenerateMoveArrays(ret.start.x, this->SIZE, false);

			ret.order = move_arrays.at(move_num);

			ret.end.x = ret.start.x - ret.order.size();
			ret.end.y = ret.start.y;
		}
	}
	return ret;
}

void Board::stop_clock(Player color)
{
	m_timecontrol.stop(color);
}

void Board::start_clock(Player color) {
	m_timecontrol.start(color);
}

TimeControl & Board::get_timecontrol()
{
	return m_timecontrol;
}

void Board::print_board()
{
	for (uint8_t i = 0; i < this->board.size(); i++)
	{
		if (i%this->SIZE == 0) {
			std::cout << std::endl;
		}

		std::cout << "[";
		for (auto j = this->board[i].begin(); j != this->board[i].end(); ++j) {
			std::cout << *j << ',';
		}
		std::cout << "]";
	}

	std::cout << std::endl;
}

