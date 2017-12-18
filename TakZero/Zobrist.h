#pragma once
#include <array>

class Zobrist {
public:
	static std::array<std::array<uint64_t, FastBoard::MAXSQ>, 4> zobrist;
	static std::array<std::array<uint64_t, FastBoard::MAXSQ * 2>, 2> zobrist_pris;
	static std::array<uint64, 5>                                   zobrist_pass;

	static void init_zobrist(Random & rng);
};