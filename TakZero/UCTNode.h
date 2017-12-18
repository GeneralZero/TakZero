#include <mutex>

#include "config.h"
#include "Board.h"

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
    bool create_children(std::atomic<int> & nodecount,
                         Board & state, float & eval);
    float eval_state(Board& state);
    void delete_child(UCTNode * child);
    void invalidate();
    bool valid() const;
    int get_move() const;
    int get_visits() const;
    float get_score() const;
    void set_score(float score);
    float get_eval(bool white_turn) const;
    double get_blackevals() const;
    void set_visits(int visits);
    void set_blackevals(double blacevals);
    void set_eval(float eval);
    void accumulate_eval(float eval);
    void virtual_loss(void);
    void virtual_loss_undo(void);
    void dirichlet_noise(float epsilon, float alpha);
    void randomize_first_proportionally();
    void update(float eval = std::numeric_limits<float>::quiet_NaN());

    UCTNode* uct_select_child(int color);
    UCTNode* get_first_child() const;
    UCTNode* get_sibling() const;

    void sort_root_children(int color);
    UCTNode* get_best_root_child(bool turn_white);
    std::mutex & get_mutex();

private:
    UCTNode();
    void link_child(UCTNode * newchild);
    void link_nodelist(std::atomic<int> & nodecount,
                       std::vector<Network::scored_node> & nodelist,
                       float prev_win_rate);

    // Tree data
    std::atomic<bool> m_has_children{false};
    UCTNode* m_firstchild{nullptr};
    UCTNode* m_nextsibling{nullptr};
    // Move
    int m_move;
    // UCT
    std::atomic<int> m_visits{0};
    std::atomic<int> m_virtual_loss{0};
    // UCT eval
    float m_score;
    float m_prev_win_rate;
    std::atomic<double> m_black_win_rate{0};
    // node alive (not superko)
    std::atomic<bool> m_valid{true};
    // Is someone adding scores to this node?
    // We don't need to unset this.
    bool m_is_expanding{false};
    SMP::Mutex m_nodemutex;
};
