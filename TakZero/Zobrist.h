#pragma once
#include <array>
#include "Random.h"

class Zobrist {
public:
	static std::array<std::array<std::uint64_t, 5 * 5 * 64>, 8> zobrist;

	static constexpr auto zobrist_blacktomove = 0xABCDABCDABCDABCDULL;

	static void init_zobrist(Random & rng);
};