#pragma once
#include "UCTNode.h"
#include "Board.h"

class UCTTakGame
{
public:
	UCTTakGame(uint16_t ittermult);
	~UCTTakGame();

	UCTNode rootnode;
	std::vector<UCTNode> childNodes;

	UCTNode* ChooseMove();

	std::vector<UCTNode> Search();
	void Rollout(Board state, UCTNode node);

private:
	Board game;

	uint32_t iter;


};

