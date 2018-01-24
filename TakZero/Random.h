#pragma once

#include "config.h"
#include <limits>

#undef min
#undef max
/*
Random number generator xoroshiro128+
*/
class Random {
public:
	Random() = delete;
	Random(std::uint64_t seed = 0);
	void seedrandom(std::uint64_t s);

	// random numbers from 0 to max
	template<int MAX_NUM>
	std::uint32_t randfix() {
		
		static_assert(0 < MAX_NUM && MAX_NUM < std::numeric_limits<std::uint32_t>::max(),
			"randfix out of range");
		// Last bit isn't random, so don't use it in isolation. We specialize
		// this case.
		static_assert(MAX_NUM != 2, "don't isolate the LSB with xoroshiro128+");
		return gen() % MAX_NUM;
	}

	std::uint16_t randuint16(const std::uint16_t max);
	std::uint32_t randuint32(const std::uint32_t max);
	std::uint32_t randuint32();

	// random float from 0 to 1
	float randflt(void);

	// return the thread local RNG
	static Random& get_Rng(void);

	// UniformRandomBitGenerator interface
	using result_type = std::uint64_t;
	
	constexpr static result_type min() {
		return std::numeric_limits<result_type>::min();
	}
	constexpr static result_type max() {
		return std::numeric_limits<result_type>::max();
	}
	
	result_type operator()() {
		return gen();
	}

private:
	std::uint64_t gen(void);
	std::uint64_t m_s[2];
};

