#include <assert.h>
#include <math.h>

#include "FakeNetwork.h"
#include "Utility.h"
#include "ThreadPool.h"
#include "Random.h"

extern Utils::ThreadPool thread_pool;

// Input + residual block tower
static std::vector<std::vector<float>> conv_weights;
static std::vector<std::vector<float>> conv_biases;
static std::vector<std::vector<float>> batchnorm_means;
static std::vector<std::vector<float>> batchnorm_variances;

// Policy head
static std::vector<float> conv_pol_w;
static std::vector<float> conv_pol_b;
static std::array<float, 2> bn_pol_w1;
static std::array<float, 2> bn_pol_w2;

static std::array<float, 261364> ip_pol_w;
static std::array<float, 362> ip_pol_b;

// Value head
static std::vector<float> conv_val_w;
static std::vector<float> conv_val_b;
static std::array<float, 1> bn_val_w1;
static std::array<float, 1> bn_val_w2;

static std::array<float, 92416> ip1_val_w;
static std::array<float, 256> ip1_val_b;

static std::array<float, 1575> ip2_val_w;
static std::array<float, 1> ip2_val_b;

Netresult FakeNetwork::get_scored_moves(Board * state, Ensemble ensemble, int rotation)
{
	Netresult result;

	NNPlanes planes;
	gather_features(state, planes);

	if (ensemble == DIRECT) {
		assert(rotation >= 0 && rotation <= 7);
		result = get_scored_moves_internal(state, planes, rotation);
	}
	else {
		assert(ensemble == RANDOM_ROTATION);
		assert(rotation == -1);
		auto rand_rot = Random::get_Rng()->randfix<8>();
		result = get_scored_moves_internal(state, planes, rand_rot);
	}

	return result;
}

Netresult FakeNetwork::get_scored_moves_internal(
	Board * state, NNPlanes & planes, int rotation) {
	std::vector<scored_node> result;

	std::vector<Play> moves = state->getAllPlays();

	float black_winrate = 0.0;

	float winrate_sig = (1.0f + std::tanh(black_winrate)) / 2.0f;

	for each (Play move in moves)
	{
		result.emplace_back(1/moves.size(), move.index);
	}

	return std::make_pair(result, winrate_sig);
}

void FakeNetwork::initialize(){

}

void FakeNetwork::benchmark(Board * state, int iterations)
{
	int cpus = ConfigStore::get().ints.at("cfg_num_threads");
	int iters_per_thread = (iterations + (cpus - 1)) / cpus;

	Time start;

	Utils::ThreadGroup tg(thread_pool);
	for (int i = 0; i < cpus; i++) {
		tg.add_task([iters_per_thread, state]() {
			Board mystate = *state;
			for (int loop = 0; loop < iters_per_thread; loop++) {
				auto vec = get_scored_moves(&mystate, Ensemble::DIRECT, 0);
			}
		});
	};
	tg.wait_all();

	Time end;
	auto centiseconds = Time::timediff(start, end) / 100.0;
	Utils::myprintf("%5d evaluations in %5.2f seconds -> %d n/s\n",
		iterations, centiseconds, (int)(iterations / centiseconds));
}

void FakeNetwork::show_heatmap(Board * state, Netresult & netres, bool topmoves){

}

void FakeNetwork::softmax(const std::vector<float>& input, std::vector<float>& output, float temperature) {

}

void FakeNetwork::gather_features(Board * state, NNPlanes & planes) {

}