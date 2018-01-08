#pragma once

#include <vector>

#include "UCTNode.h"
#include "mutex.h"

class TTEntry {
public:
	TTEntry() = default;

	std::uint64_t m_hash{0};
	std::uint64_t m_visits;
	double m_black_wins;
};

class TTable {
public:
	/*
		return the global TT
	*/
	static TTable* get_TT(void);

	/*
		update corresponding entry
	*/
	void update(std::uint64_t hash, const UCTNode * node);

	/*
		sync given node with TT
	*/
	void sync(std::uint64_t hash, UCTNode * node);

private:
	TTable(std::uint64_t = 50000000);

	SMP::Mutex m_mutex;
	std::vector<TTEntry> m_buckets;
};

