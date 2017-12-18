#pragma once

#include "config.h"
#include <limits>

/*
Random number generator xoroshiro128+
*/
class Random {
public:
	Random(int seed = -1);
	void seedrandom(uint32_t s);

	// random numbers from 0 to max
	template<int MAX>
	uint32_t randfix() {
		static_assert(0 < MAX && MAX < std::numeric_limits<uint32_t>::max(),
			"randfix out of range");
		// Last bit isn't random, so don't use it in isolation. We specialize
		// this case.
		static_assert(MAX != 2, "don't isolate the LSB with xoroshiro128+");
		return random() % MAX;
	};

	uint16_t randuint16(const uint16_t max);
	uint32_t randuint32(const uint32_t max);
	uint32_t randuint32();

	// random float from 0 to 1
	float randflt(void);

	// return the thread local RNG
	static Random* get_Rng(void);

	// UniformRandomBitGenerator interface
	using result_type = uint64_t;
	constexpr static result_type min() {
		return std::numeric_limits<result_type>::min();
	}
	constexpr static result_type max() {
		return std::numeric_limits<result_type>::max();
	}
	result_type operator()() {
		return random();
	}

private:
	uint64_t random(void);
	uint64_t m_s[2];
};

// Specialization for last bit: use sign test
template<>
inline uint32_t Random::randfix<2>() {
	return (random() > (std::numeric_limits<uint64_t>::max() / 2));
}
