#define NOMINMAX
#include "config.h"
#include <algorithm>

#include "NNCache.h"
#include "Utility.h"

NNCache::NNCache(int size) : m_size(size) {}

NNCache& NNCache::get_NNCache(void) {
	static NNCache cache;
	return cache;
}

template <class T>
inline size_t hash_combine(size_t seed, const T& v) {
	std::hash<T> hasher;
	return seed ^ (hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

static size_t compute_hash(const NNInput boards) {
	auto hash = size_t{ 0 };
	for (const auto& board : boards) {
		for (const auto& p : board) {
			hash = hash_combine(hash, p);
		}
	}
	return hash;
}

bool NNCache::lookup(std::uint64_t hash, NNOutput& result) {
	std::lock_guard<std::mutex> lock(m_mutex);
	++m_lookups;

	auto iter = m_cache.find(hash);
	if (iter == m_cache.end()) {
		return false;  // Not found.
	}

	//No collision detection

	// Found it.
	++m_hits;
	result = entry->result;
	return true;
}

void NNCache::insert(std::uint64_t hash,
	const NNOutput result) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_cache.find(hash) != m_cache.end()) {
		return;  // Already in the cache.
	}

	m_cache.emplace(hash, std::make_unique<Entry>(result));
	m_order.push_back(hash);
	++m_inserts;

	// If the cache is too large, remove the oldest entry.
	if (m_order.size() > m_size) {
		m_cache.erase(m_order.front());
		m_order.pop_front();
	}
}

void NNCache::resize(int size) {
	m_size = size;
	while (m_order.size() > m_size) {
		m_cache.erase(m_order.front());
		m_order.pop_front();
	}
}

void NNCache::set_size_from_playouts(std::uint64_t max_playouts) {
	// cache hits are generally from last several moves so setting cache
	// size based on playouts increases the hit rate while balancing memory
	// usage for low playout instances. 50'000 cache entries is ~250 MB
	std::uint64_t max_size = std::max((std::uint64_t)6000, 3 * max_playouts);
	max_size = std::min((std::uint64_t)50000, max_size);
	NNCache::get_NNCache().resize(max_size);
}

void NNCache::dump_stats() {
	Utils::myprintf("NNCache: %d/%d hits/lookups = %.1f%% hitrate, %d inserts, %u size\n",
		m_hits, m_lookups, 100. * m_hits / (m_lookups + 1),
		m_inserts, m_cache.size());
}