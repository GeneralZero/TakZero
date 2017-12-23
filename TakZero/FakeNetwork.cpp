#include <assert.h>
#include <math.h>

#include "FakeNetwork.h"
#include "Utility.h"
#include "ThreadPool.h"
#include "Random.h"

extern Utils::ThreadPool thread_pool;

Netresult FakeNetwork::get_scored_moves(Board * state)
{
	Netresult result;

	NNPlanes planes;
	//gather_features(state, planes);
	result = get_scored_moves_internal(state, planes);
	return result;
}

Netresult FakeNetwork::get_scored_moves_internal(
	Board * state, NNPlanes & planes) {
	std::vector<scored_node> result;

	std::vector<Play> moves = state->getAllPlays();

	std::random_shuffle(moves.begin(), moves.end());

	float black_winrate = 0.0;

	float winrate_sig = (1.0f + std::tanh(black_winrate)) / 2.0f;

	for each (Play move in moves)
	{
		result.emplace_back(1/(double)moves.size(), move.index);
	}

	return std::make_pair(result, winrate_sig);
}

void FakeNetwork::initialize(){

}

void FakeNetwork::benchmark(Board * state, int iterations)
{
	/*
	int cpus = ConfigStore::get().ints.at("cfg_num_threads");
	int iters_per_thread = (iterations + (cpus - 1)) / cpus;

	Time start;

	Utils::ThreadGroup tg(thread_pool);
	for (int i = 0; i < cpus; i++) {
		tg.add_task([iters_per_thread, state]() {
			Board mystate = *state;
			for (int loop = 0; loop < iters_per_thread; loop++) {
				auto vec = get_scored_moves(&mystate);
			}
		});
	};
	tg.wait_all();

	Time end;
	auto centiseconds = Time::timediff(start, end) / 100.0;
	Utils::myprintf("%5d evaluations in %5.2f seconds -> %d n/s\n",
		iterations, centiseconds, (int)(iterations / centiseconds));
	*/
}

void FakeNetwork::show_heatmap(Board * state, Netresult & netres, bool topmoves){

}

void FakeNetwork::softmax(const std::vector<float>& input, std::vector<float>& output, float temperature) {

}

void FakeNetwork::gather_features(Board * state, NNPlanes & planes) {

}