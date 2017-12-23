// TakZero.cpp : main project file.

//#include "TestingBoard.h"
#include "FakeNetwork.h"
#include "Board.h"
#include "config.h"
#include "UCTSearch.h"
#include "ThreadPool.h"
#include "Training.h"

#include <memory>
#include <string>
#include <iostream>


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
	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

	ConfigStore::get().ints.insert(std::pair<std::string, uint64_t>("cfg_max_playouts", 5000000));
	ConfigStore::get().ints.insert(std::pair<std::string, uint64_t>("cfg_num_threads", 1));
	ConfigStore::get().ints.insert(std::pair<std::string, uint64_t>("cfg_lagbuffer_cs", 10));
	ConfigStore::get().ints.insert(std::pair<std::string, uint64_t>("cfg_random_cnt", 30));
	ConfigStore::get().ints.insert(std::pair<std::string, uint64_t>("cfg_rng_seed",562312454));
	
	ConfigStore::get().bools.insert(std::pair<std::string, bool>("cfg_quiet", false));
	ConfigStore::get().bools.insert(std::pair<std::string, bool>("cfg_noise", true));

	ConfigStore::get().doubles.insert(std::pair<std::string, double>("cfg_resignpct", 0.0));
	ConfigStore::get().doubles.insert(std::pair<std::string, double>("cfg_puct", 0.8));

	//Testing
	/*
	TestCapstoneCrush();
	TestFullGame();
	TestGenerateMoves();
	TestIndexGeneration();
	TestPartGame1();
	TestFullMoveGeneration();
	TestInvalidMove();
	TestMoveGeneration1();
	TestMoveGeneration2();
	*/
	
	
	//Setup the Connection back to Tensorflow
	FakeNetwork::initialize();

	//Setup threads
	thread_pool.initialize(ConfigStore::get().ints.at("cfg_num_threads"));

	int white_win = 0;
	int black_win = 0;

	for (size_t i = 0; i < 50000; i++)
	{
		//Create Game 
		Board & maingame = Board(5);
		Training training = Training(); 
		auto search = std::make_unique<UCTSearch>(maingame);

		//Get new Prob from DeepLearning (Threaded it takes a while)
		auto ml_data = maingame.getMLData();

		while (!maingame.black_win && !maingame.white_win) {

			uint16_t move;
			bool white_turn = maingame.white_turn;

			//Wait for ML thread

			//get move index
			if (white_turn) {
				move = search->think(White, training);
			}
			else {
				move = search->think(Black, training);
			}

			//Change nodes to new node (Threaded It takes a while)
			search->swap_root(move);

			//Save Probs to disk (Threaded)


			maingame.white_turn = white_turn;
			//Print Move
			Play play_move = maingame.GetMoveFromIndex(move);

			//Play move
			maingame.PlayIndex(move);

			//Print Board 
			//maingame.print_board();

			//Save Fastboard
			maingame.SaveFastBoard();

			//Wait for Node update thread
			//training.dump_game();
		}
		if (maingame.white_win && maingame.black_win) {
			maingame.print_board();
			std::cout << "Tie" << std::endl;
		}
		else if (maingame.white_win) {
			maingame.print_board();
			std::cout << "White Win" << std::endl;
			white_win++;
		}
		else {
			maingame.print_board();
			std::cout << "Black Win" << std::endl;
			black_win++;
		}

		auto * raw = search.release();        // pointer to no-longer-managed object
		delete raw;
		
		std::cout << "White Win: " << white_win << ", Black Win: " << black_win << std::endl;
		//training.dump_game();
	}
    return 0;
}
