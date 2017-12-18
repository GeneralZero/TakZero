#pragma once

#include <array>

#include "Board.h"
#include "Random.h"

class Zobrist {
public:
	static std::array<std::array<uint64_t, 5 * 5 * 64>, 8> zobrist;

	static void init_zobrist(Random & rng);
};