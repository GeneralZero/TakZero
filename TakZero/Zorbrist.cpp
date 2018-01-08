#include "Zobrist.h"
#include "Random.h"

std::array<std::array<std::uint64_t, 5 * 5 * 64>, 8> Zobrist::zobrist;

void Zobrist::init_zobrist(Random & rng) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 5*5*64; j++) {
			Zobrist::zobrist[i][j] = ((std::uint64_t)rng.randuint32()) << 32;
			Zobrist::zobrist[i][j] ^= (std::uint64_t)rng.randuint32();
		}
	}
}