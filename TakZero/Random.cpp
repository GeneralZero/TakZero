#include <time.h>
#include <limits.h>
#include <thread>
#include "config.h"

#include "Random.h"
#include "Utility.h"

Random* Random::get_Rng(void) {
	static thread_local Random s_rng;
	return &s_rng;
}

Random::Random(int seed) {
	if (seed == -1) {
		size_t thread_id =
			std::hash<std::thread::id>()(std::this_thread::get_id());
		seedrandom((uint32_t)time(0) ^ (uint32_t)thread_id);
	}
	else {
		seedrandom(seed);
	}
}

// This is xoroshiro128+.
// Note that the last bit isn't entirely random, so don't use it,
// if possible.
uint64_t Random::random(void) {
	const uint64_t s0 = m_s[0];
	uint64_t s1 = m_s[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	m_s[0] = Utils::rotl(s0, 55) ^ s1 ^ (s1 << 14);
	m_s[1] = Utils::rotl(s1, 36);

	return result;
}

uint16_t Random::randuint16(const uint16_t max) {
	return ((random() >> 48) * max) >> 16;
}

uint32_t Random::randuint32(const uint32_t max) {
	return ((random() >> 32) * (uint64_t)max) >> 32;
}

uint32_t Random::randuint32() {
	return random() >> 32;
}

void Random::seedrandom(uint32_t seed) {
	// Magic values from Pierre L'Ecuyer,
	// "Tables of Linear Congruental Generators of different sizes and
	// good lattice structure"
	m_s[0] = (741103597 * seed);
	m_s[1] = (741103597 * m_s[0]);
}

float Random::randflt(void) {
	// We need a 23 bit mantissa + implicit 1 bit = 24 bit number
	// starting from a 64 bit random.
	constexpr float umax = 1.0f / (UINT32_C(1) << 24);
	uint32_t rnd = random() >> 40;
	return ((float)rnd) * umax;
}