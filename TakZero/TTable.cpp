#include "config.h"

#include <vector>
#include "mutex.h"
#include "ThreadPool.h"
#include "TTable.h"

TTable* TTable::get_TT(void) {
	static TTable s_ttable;
	return &s_ttable;
}

TTable::TTable(std::uint64_t size) {
	LOCK(m_mutex, lock);
	m_buckets.resize(size);
}

void TTable::update(std::uint64_t hash, const UCTNode * node) {
	LOCK(m_mutex, lock);

	auto index = hash;
	index %= m_buckets.size();

	/*
		update TT
	*/
	m_buckets[index].m_hash       = hash;
	m_buckets[index].m_visits     = node->get_visits();
	m_buckets[index].m_black_wins = node->get_black_wins();
}

void TTable::sync(std::uint64_t hash, UCTNode * node) {
	LOCK(m_mutex, lock);

	unsigned int index = (unsigned int)hash;
	index %= m_buckets.size();

	/*
		check for hash fail
	*/
	if (m_buckets[index].m_hash != hash) {
		return;
	}

	/*
		valid entry in TT should have more info than tree
	*/
	if (m_buckets[index].m_visits > node->get_visits()) {
		/*
			entry in TT has more info (new node)
		*/
		node->set_visits(m_buckets[index].m_visits);
		node->set_black_wins(m_buckets[index].m_black_wins);
	}
}
