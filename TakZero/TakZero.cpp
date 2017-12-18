// TakZero.cpp : main project file.

#include "TestingBoard.h"
#include "FakeNetwork.h"
#include "Board.h"
#include "config.h"
#include "UCTSearch.h"
#include "ThreadPool.h"

#include <memory>

#ifdef _DEBUG
#undef _DEBUG
#include <python.h>
#define _DEBUG
#else
#include <python.h>
#endif

extern Utils::ThreadPool thread_pool;

int main(int argc, char *argv[])
{
	ConfigStore::get().ints.insert(std::pair<std::string, uint32_t>("cfg_max_playouts", 50000));
	ConfigStore::get().ints.insert(std::pair<std::string, uint32_t>("cfg_num_threads", 6));
	ConfigStore::get().ints.insert(std::pair<std::string, uint32_t>("cfg_lagbuffer_cs", 3));
	ConfigStore::get().ints.insert(std::pair<std::string, uint32_t>("cfg_random_cnt", 30));

	ConfigStore::get().bools.insert(std::pair<std::string, bool>("cfg_quiet", false));
	ConfigStore::get().bools.insert(std::pair<std::string, bool>("cfg_noise", true));

	ConfigStore::get().doubles.insert(std::pair<std::string, double>("cfg_resignpct", 0.0));
	ConfigStore::get().doubles.insert(std::pair<std::string, double>("cfg_puct", 2.8));

	//Testing
	TestCapstoneCrush();
	TestFullGame();
	TestGenerateMoves();
	TestIndexGeneration();
	TestPartGame1();

	
	//Setup the Connection back to Tensorflow
	FakeNetwork::initialize();

	//Setup threads
	thread_pool.initialize(6);

	//Create Game 
	Board & maingame = Board(5);

	while (!maingame.black_win && !maingame.white_win) {
		//Do a search for a move
		auto search = std::make_unique<UCTSearch>(maingame);

		uint16_t move;

		//get move index
		if (maingame.white_turn) {
			move = search->think(White);
		}
		else {
			move = search->think(Black);
		}
		//Print Move
		Play play_move = maingame.GetMoveFromIndex(move);

		//Play move
		maingame.PlayIndex(move);

		//Print Board 
		maingame.print_board();
	}

    return 0;
}
