#pragma once

#include "config.h"
#include "mutex.h"
#include "Board.h"
#include "FakeNetwork.h"

class UCTNode {
public:
    using sortnode_t = std::tuple<float, int, float, UCTNode*>;

    // When we visit a node, add this amount of virtual losses
    // to it to encourage other CPUs to explore other parts of the
    // search tree.
    static constexpr auto VIRTUAL_LOSS_COUNT = 3;

    explicit UCTNode(int vertex, float score, float prev_win_rate);
    ~UCTNode();
    bool first_visit() const;
    bool has_children() const;
	void set_visits(uint64_t visits);
    bool create_children(std::atomic<int> & nodecount,
                         Board & state, float & eval);
	int add_move_nodes(std::vector<scored_node> nodelist, float win_rate);
    float eval_state(Board& state);
    int get_move() const;
    int get_visits() const;
    float get_score() const;
	float get_prev_score() const;
    void set_score(float score);
    float get_eval(bool white_turn) const;
	double get_black_wins() const;
    void virtual_loss(void);
    void virtual_loss_undo(void);
    void dirichlet_noise(float epsilon, float alpha);
    void update(float eval = std::numeric_limits<float>::quiet_NaN());
	double get_percent_black_wins() const;
	void set_black_wins(double black_wins);
	void accumulate_eval(double win);
	UCTNode* uct_select_child(int color);
	void sort_moves(bool white_turn);
    UCTNode* get_best_root_child(bool turn_white);
	SMP::Mutex& UCTNode::get_mutex();

	std::vector<UCTNode*> possoble_moves;

private:
    UCTNode();

    // Tree data
    std::atomic<bool> m_has_children{false};
    
    // Move
    int m_move;
    // UCT
    std::atomic<uint64_t> m_visits{0};
    std::atomic<uint64_t> m_virtual_loss{0};
    // UCT eval
    float m_score;
    float m_prev_win_rate;
    std::atomic<double> m_black_win{0};
    // Is someone adding scores to this node?
    // We don't need to unset this.
    bool m_is_expanding{false};
    SMP::Mutex m_nodemutex;
};
