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
	bool delete_node() const { return m_delete_me; }
	void was_deleted() { m_delete_me = false; }

    static SearchResult from_eval(float eval) {
        return SearchResult(eval);
    }
    static SearchResult from_score(float board_score, bool delete_me) {
        if (board_score == (float)Black) {
            return SearchResult((float)Black, delete_me, true);
        } else if (board_score == (float)White) {
            return SearchResult((float)White, delete_me, true);
		}
		else if (board_score == 3.0) {
			return SearchResult(0.5f, delete_me, true);
		}
		else{
            return SearchResult(board_score, delete_me, true);
        }
    }
	
private:
	explicit SearchResult(float eval, bool delete_me)
		: m_valid(false), m_eval(eval), m_delete_me(delete_me) {
		if (eval == (float)Black)
			m_valid = true;
		else if (eval == (float)White) {
			m_valid = true;
		}
	}
    explicit SearchResult(float eval)
        : m_valid(false), m_delete_me(false), m_eval(eval) {
		if (eval == (float)Black)
			m_valid = true;
		else if (eval == (float)White) {
			m_valid = true;
		}
	}
	explicit SearchResult(float eval, bool delete_me, bool valid)
		: m_valid(valid), m_eval(eval), m_delete_me(delete_me) {
		if (eval == (float)Black)
			m_valid = valid;
		else if (eval == (float)White) {
			m_valid = valid;
		}
	}
	bool m_delete_me{ false };
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

private:
    void dump_stats(Board & state, UCTNode & parent);
    void dump_analysis(int playouts);
    int get_best_move();

	std::string get_pv(Board & state, UCTNode & parent, uint8_t depth);

    Board & m_rootstate;
    UCTNode* m_root;
    std::atomic<int> m_nodes{0};
    std::atomic<int> m_playouts{0};
    std::atomic<bool> m_run{false};
    int m_maxplayouts;
	Player turn;
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
