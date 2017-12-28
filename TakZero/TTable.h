#pragma once

#include <vector>

#include "UCTNode.h"
#include "mutex.h"

class TTEntry {
public:
    TTEntry() = default;

    uint64_t m_hash{0};
    int m_visits;
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
    void update(uint64_t hash, const UCTNode * node);

    /*
        sync given node with TT
    */
    void sync(uint64_t hash, UCTNode * node);

private:
    TTable(int size = 5000000);

    SMP::Mutex m_mutex;
    std::vector<TTEntry> m_buckets;
};

