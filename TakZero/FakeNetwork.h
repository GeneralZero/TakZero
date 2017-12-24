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

typedef std::pair<float, int> scored_node;
typedef std::pair<std::vector<scored_node>, float> Netresult;

typedef std::array<float, 1575> PlayProb;
typedef std::array<FastBoard, 8> NNInput;
typedef std::pair<PlayProb, float> NNOutput;

typedef std::vector<FastBoard> NNPlanes;

typedef float net_t;

class FakeNetwork
{
public:
	enum Ensemble {
		DIRECT, RANDOM_ROTATION
	};

	static Netresult get_scored_moves(Board * state);
	// File format version

	static void initialize();
	static void benchmark(Board * state, int iterations = 1600);
	static void show_heatmap(Board * state, Netresult & netres, bool topmoves);
	static void softmax(const std::vector<float>& input,
		std::vector<float>& output,
		float temperature = 1.0f);
	static void gather_features(Board* state, NNPlanes & planes);

private:
	static Netresult get_scored_moves_internal(
		Board * state, NNPlanes & planes);
};