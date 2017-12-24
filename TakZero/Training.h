#pragma once

#include "config.h"
#include <utility>
#include "Board.h"
#include "UCTNode.h"
#include "FakeNetwork.h"
#include <iostream>
#include <cstdint>
#include <cstring>

class TimeStep {
public:
	NNInput planes;
	PlayProb probabilities;
	float net_winrate;
};

class Training {
public:
	void record(Board & state, UCTNode & root);

	void dump_game();
	static std::vector<TimeStep> m_data;
private:
	//Rotate Board
	int get_new_index(uint8_t index, uint8_t transformation);
	FastBoard rotateBoard(FastBoard board, uint8_t transformation);
	TimeStep transformTimeStep(TimeStep input, uint8_t transformation);
	int save_game(std::string foldername, std::string file_name);

	int uploadGame(std::string foldername, std::string filename);

	//Rotate Board
	Board rotate_Board{5};
};