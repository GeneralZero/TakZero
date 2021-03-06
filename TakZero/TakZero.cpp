// TakZero.cpp : main project file.

#include "TestingBoard.h"
#include "DLNetwork.h"
#include "Board.h"
#include "config.h"
#include "UCTSearch.h"
#include "ThreadPool.h"
#include "Training.h"
#include "Random.h"
#include "Zobrist.h"
#include "LoadNetwork.h"

#include <memory>
#include <string>
#include <iostream>


#ifdef _DEBUG
#undef _DEBUG
//#include <python.h>
#define _DEBUG
#else
//#include <python.h>
#endif

extern Utils::ThreadPool thread_pool;

int main(int argc, char *argv[])
{
	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

	ConfigStore::get().ints.insert(std::pair<std::string, std::uint64_t>("cfg_max_playouts", 400));
	ConfigStore::get().ints.insert(std::pair<std::string, std::uint64_t>("cfg_num_threads", concurentThreadsSupported- 2));
	ConfigStore::get().ints.insert(std::pair<std::string, std::uint64_t>("cfg_lagbuffer_cs", -100000000));
	ConfigStore::get().ints.insert(std::pair<std::string, std::uint64_t>("cfg_random_cnt", 30));
	ConfigStore::get().ints.insert(std::pair<std::string, std::uint64_t>("cfg_rng_seed",562312454));
	ConfigStore::get().ints.insert(std::pair<std::string, std::uint64_t>("ipc_port", 64217));
	
	
	ConfigStore::get().bools.insert(std::pair<std::string, bool>("cfg_quiet", false));
	ConfigStore::get().bools.insert(std::pair<std::string, bool>("cfg_noise", true));
	ConfigStore::get().bools.insert(std::pair<std::string, bool>("skip_cache", false));

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
	DLNetwork::initialize();

	//Setup Tensorflow
	LoadNetwork::initalize("TakZero.pb");

	// Use deterministic random numbers for hashing
    auto rng = std::make_unique<Random>(ConfigStore::get().ints.at("cfg_rng_seed"));
	Zobrist::init_zobrist(*rng);

	//Setup threads
	thread_pool.initialize(ConfigStore::get().ints.at("cfg_num_threads"));

	int white_win = 0;
	int black_win = 0;
	std::string prev_game = "";

	Training training = Training(); 
	training.setFolderName("Training");

	for (size_t i = 0; i < 50000; i++)
	{
		//Create Game 
		Board* maingame = new Board(5);

		while (!maingame->black_win && !maingame->white_win) {

			std::uint16_t move;
			bool white_turn = maingame->white_turn;
			auto search = std::make_unique<UCTSearch>(*maingame);

			//Wait for ML thread

			//get move index
			if (white_turn) {
				move = search->think(White, training);
			}
			else {
				move = search->think(Black, training);
			}

			//Change nodes to new node (Threaded It takes a while)
			//search->swap_root(move);

			//Save Probs to disk (Threaded)

			maingame->white_turn = white_turn;
			//Print Move
			Play play_move = maingame->GetMoveFromIndex(move);

			//Play move
			maingame->PlayIndex(move);

			//Print Board 
			//maingame->print_board();

			//Save Fastboard
			maingame->SaveFastBoard();

			//Wait for Node update thread
			//training.dump_game();
			auto * raw = search.release();        // pointer to no-longer-managed object
			delete raw;
		}
		if (maingame->white_win && maingame->black_win) {
			maingame->print_board();
			std::cout << "Tie" << std::endl;
			if (prev_game != "") {
				training.uploadGame(prev_game);
			}
			prev_game = training.dump_game(0);
		}
		else if (maingame->white_win) {
			maingame->print_board();
			std::cout << "White Win" << std::endl;
			white_win++;
			if (prev_game != "") {
				training.uploadGame(prev_game);
			}
			prev_game = training.dump_game(-1);
		}
		else {
			maingame->print_board();
			std::cout << "Black Win" << std::endl;
			black_win++;
			if (prev_game != "") {
				training.uploadGame(prev_game);
			}
			prev_game = training.dump_game(1);
		}
		//delete search;

		delete maingame;
		
		std::cout << "White Win: " << white_win << ", Black Win: " << black_win << std::endl;
	}
    return 0;
}
