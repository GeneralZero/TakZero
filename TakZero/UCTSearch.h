#pragma once

#include <memory>
#include <atomic>
#include <tuple>

#include "Board.h"
#include "UCTNode.h"

class SearchResult {
public:
    SearchResult() = default;
    bool valid() const { return m_valid;  }
    float eval() const { return m_eval;  }
    static SearchResult from_eval(float eval) {
        return SearchResult(eval);
    }
    static SearchResult from_score(float board_score) {
        if (board_score > 0.0f) {
            return SearchResult(1.0f);
        } else if (board_score < 0.0f) {
            return SearchResult(0.0f);
        } else {
            return SearchResult(0.5f);
        }
    }
private:
    explicit SearchResult(float eval)
        : m_valid(true), m_eval(eval) {}
    bool m_valid{false};
    float m_eval{0.0f};
};

class UCTSearch {
public:
    /*
        Maximum size of the tree in memory. Nodes are about
        40 bytes, so limit to ~1.6G.
    */
    static constexpr auto MAX_TREE_SIZE = 40'000'000;

    UCTSearch(Board & g);
    int think(bool white_turn);
    void set_playout_limit(int playouts);
    void set_analyzing(bool flag);
    void set_quiet(bool flag);
    void ponder();
    bool is_running() const;
    bool playout_limit_reached() const;
    void increment_playouts();
    SearchResult play_simulation(Board & currstate, UCTNode * const node);

private:
    void dump_stats(Board & state, UCTNode & parent);
    std::string get_pv(Board & state, UCTNode & parent);
    void dump_analysis(int playouts);
    int get_best_move();

    Board & m_rootstate;
    UCTNode m_root{-1, 0.0f, 0.5f};
    std::atomic<int> m_nodes{0};
    std::atomic<int> m_playouts{0};
    std::atomic<bool> m_run{false};
    int m_maxplayouts;
};

class UCTWorker {
public:
    UCTWorker(Board & g, UCTSearch * search, UCTNode * root)
      : m_rootstate(g), m_search(search), m_root(root) {}
    void operator()();
private:
    Board & m_rootstate;
    UCTSearch * m_search;
    UCTNode * m_root;
};
