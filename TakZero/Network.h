#pragma once

#include "config.h"
#include <vector>
#include <string>
#include <bitset>
#include <memory>
#include <array>

#ifdef USE_OPENCL
#include <atomic>
class UCTNode;
#endif

#include "Board.h"

class Network {
public:
	enum Ensemble {
		DIRECT, RANDOM_ROTATION
	};
	using BoardPlane = std::bitset<5 * 5 * 64 * 4>;
	using NNPlanes = std::vector<BoardPlane>;
	using scored_node = std::pair<float, int>;
	using Netresult = std::pair<std::vector<scored_node>, float>;

	static Netresult get_scored_moves(Board * state,
		Ensemble ensemble,
		int rotation = -1);
	// File format version
	static constexpr int FORMAT_VERSION = 1;
	static constexpr int INPUT_CHANNELS = 18;

	static void initialize();
	static void benchmark(Board * state, int iterations = 1600);
	static void show_heatmap(FastBoard * state, Netresult & netres, bool topmoves);
	static void softmax(const std::vector<float>& input,
		std::vector<float>& output,
		float temperature = 1.0f);
	static void gather_features(Board* state, NNPlanes & planes);

private:
	static Netresult get_scored_moves_internal(
		Board * state, NNPlanes & planes, int rotation);
	static int rotate_nn_idx(const int vertex, int symmetry);
};
