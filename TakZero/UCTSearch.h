#pragma once

#include <memory>
#include <atomic>
#include <tuple>

#include "Move.h"
#include "Board.h"
#include "UCTNode.h"
#include "Training.h"

class SearchResult {
public:
    SearchResult() = default;
    bool valid() const { return m_valid;  }
    float eval() const { return m_eval;  }

    static SearchResult from_eval(float eval) {
        return SearchResult(eval);
    }
    static SearchResult from_score(float board_score) {
        if (board_score == (float)Black) {
            return SearchResult((float)Black, true);
        } else if (board_score == (float)White) {
            return SearchResult((float)White, true);
		}
		else if (board_score == 3.0) {
			return SearchResult(0.5f, true);
		}
		else{
            return SearchResult(board_score, false);
        }
    }
	
private:
    explicit SearchResult(float eval)
        : m_valid(false), m_eval(eval) {
	}
	explicit SearchResult(float eval, bool valid)
		: m_valid(valid), m_eval(eval) {
	}
    bool m_valid{false};
    float m_eval{0.0f};
};

class UCTSearch {
public:
    /*
        Maximum size of the tree in memory. Nodes are about
        40 bytes, so limit to ~1.6G.
    */
    static constexpr auto MAX_TREE_SIZE = 30'000'000;
	
    UCTSearch(Board & g);
    void set_playout_limit(int playouts);
    void ponder();
    bool is_running() const;
    bool playout_limit_reached() const;
    void increment_playouts();
	int think(Player color, Training training);
	void swap_root(int move_number);
    SearchResult play_simulation(Board & currstate, UCTNode * const node);

	uint32_t white_win{0};
	uint32_t black_win{ 0 };
	uint32_t tie_win{ 0 };

private:
    void dump_stats(Board & state, UCTNode & parent);
	int get_best_move(Player turn);
    void dump_analysis(int playouts);

	std::string get_pv(Board & state, UCTNode & parent, uint8_t depth);

    Board & m_rootstate;
    UCTNode* m_root;
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
