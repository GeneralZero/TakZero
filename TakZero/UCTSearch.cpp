/*
    This file is part of Leela Zero.
    Copyright (C) 2017 Gian-Carlo Pascutto

    Leela Zero is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leela Zero is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Leela Zero.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include "Board.h"
#include "UCTSearch.h"
#include "TTable.h"
#include "ThreadPool.h"
#include "Timing.h"
#include "Utility.h"

#include <cassert>



UCTSearch::UCTSearch(Board & g)
    : m_rootstate(g) {
    set_playout_limit(cfg_max_playouts);
}

SearchResult UCTSearch::play_simulation(Board & currstate, UCTNode* const node) {
    const bool white_turn = currstate.white_turn;
    const auto hash = currstate.get_hash();
    const auto prev_boards = currstate.prev_boards;

    auto result = SearchResult{};

    TTable::get_TT()->sync(hash, prev_boards, node);
    node->virtual_loss();


    if (!node->has_children()) {

        if (m_nodes < MAX_TREE_SIZE) {
            float eval;
            auto success = node->create_children(m_nodes, currstate, eval);
            if (success) {
                result = SearchResult::from_eval(eval);
            }
        } else {
            auto eval = node->eval_state(currstate);
            result = SearchResult::from_eval(eval);
        }
    }

    if (node->has_children() && !result.valid()) {
        auto next = node->uct_select_child(white_turn);

        if (next != nullptr) {
            auto move = next->get_move();

			currstate.play_move(move);
            result = play_simulation(currstate, next);
        }
    }

    if (result.valid()) {
        node->update(result.eval());
    }
    node->virtual_loss_undo();
    TTable::get_TT()->update(hash, prev_boards, node);

    return result;
}

void UCTSearch::dump_stats(Board & state, UCTNode & parent) {
    if (cfg_quiet || !parent.has_children()) {
        return;
    }

    const int white_turn = state.get_to_move();

    // sort children, put best move on top
    m_root.sort_root_children(white_turn);

    UCTNode * bestnode = parent.get_first_child();

    if (bestnode->first_visit()) {
        return;
    }

    int movecount = 0;
    UCTNode * node = bestnode;

    while (node != nullptr) {
        if (++movecount > 2 && !node->get_visits()) break;

        std::string tmp = state.move_to_text(node->get_move());
        std::string pvstring(tmp);

        printf("%4s -> %7d (V: %5.2f%%) (N: %5.2f%%) PV: ",
            tmp.c_str(),
            node->get_visits(),
            node->get_visits() > 0 ? node->get_eval(white_turn)*100.0f : 0.0f,
            node->get_score() * 100.0f);

        Board tmpstate = state;

        tmpstate.play_move(node->get_move());
        pvstring += " " + get_pv(tmpstate, *node);

        printf("%s\n", pvstring.c_str());

        node = node->get_sibling();
    }
}

int UCTSearch::get_best_move() {
    int white_turn = m_rootstate.board.get_to_move();

    // Make sure best is first
    m_root.sort_root_children(white_turn);

    // Check whether to randomize the best move proportional
    // to the playout counts, early game only.
    auto movenum = int(m_rootstate.get_movenum());
    if (movenum < cfg_random_cnt) {
        m_root.randomize_first_proportionally();
    }

    int bestmove = m_root.get_first_child()->get_move();

    // do we have statistics on the moves?
    if (m_root.get_first_child() != nullptr) {
        if (m_root.get_first_child()->first_visit()) {
            return bestmove;
        }
    }

    float bestscore = m_root.get_first_child()->get_eval(white_turn);

    int visits = m_root.get_visits();

    //If lost end game
    size_t movetresh = (m_rootstate.SIZE * m_rootstate.SIZE) * 4;
    // bad score and visited enough
    if (bestscore < ((float)cfg_resignpct / 100.0f)
        && visits > 500
        && m_rootstate.m_movenum > movetresh) {
        printf("Score looks bad. Resigning.\n");
        bestmove = -1;
    }

    return bestmove;
}

std::string UCTSearch::get_pv(Board & state, UCTNode & parent) {
    if (!parent.has_children()) {
        return std::string();
    }

    auto best_child = parent.get_best_root_child(state.get_to_move());
    auto best_move = best_child->get_move();
    auto res = state.move_to_text(best_move);

    state.play_move(best_move);

    auto next = get_pv(state, *best_child);
    if (!next.empty()) {
        res.append(" ").append(next);
    }
    return res;
}

void UCTSearch::dump_analysis(int playouts) {
    if (cfg_quiet) {
        return;
    }

    Board tempstate = m_rootstate;
    int color = tempstate.board.get_to_move();

    std::string pvstring = get_pv(tempstate, m_root);
    float winrate = 100.0f * m_root.get_eval(color);
    printf("Playouts: %d, Win: %5.2f%%, PV: %s\n",
             playouts, winrate, pvstring.c_str());
}

bool UCTSearch::is_running() const {
    return m_run;
}

bool UCTSearch::playout_limit_reached() const {
    return m_playouts >= m_maxplayouts;
}

void UCTWorker::operator()() {
    do {
        auto currstate = std::make_unique<Game>(m_rootstate);
        auto result = m_search->play_simulation(*currstate, m_root);
        if (result.valid()) {
            m_search->increment_playouts();
        }
    } while(m_search->is_running() && !m_search->playout_limit_reached());
}

void UCTSearch::increment_playouts() {
    m_playouts++;
}

int UCTSearch::think(bool white_turn) {
    assert(m_playouts == 0);
    assert(m_nodes == 0);

    // Start counting time for us
    m_rootstate.start_clock(white_turn);

    // set side to move
    m_rootstate.board.set_to_move(white_turn);

    // set up timing info
    Time start;

    m_rootstate.get_timecontrol().set_boardsize(m_rootstate.board.get_boardsize());
    auto time_for_move = m_rootstate.get_timecontrol().max_time_for_move(white_turn);

    printf("Thinking at most %.1f seconds...\n", time_for_move/100.0f);

    // create a sorted list off legal moves (make sure we
    // play something legal and decent even in time trouble)
    float root_eval;
    m_root.create_children(m_nodes, m_rootstate, root_eval);
    m_root.remove_sucide_moves(m_rootstate);
    if (cfg_noise) {
        m_root.dirichlet_noise(0.25f, 0.03f);
    }

    printf("NN eval=%f\n",
             (!white_turn ? root_eval : 1.0f - root_eval));

    m_run = true;
    int cpus = cfg_num_threads;
    Utils::ThreadGroup tg(thread_pool);
    for (int i = 1; i < cpus; i++) {
        tg.add_task(UCTWorker(m_rootstate, this, &m_root));
    }

    bool keeprunning = true;
    int last_update = 0;
    do {
        auto currstate = std::make_unique<Board>(m_rootstate);

        auto result = play_simulation(*currstate, &m_root);
        if (result.valid()) {
            increment_playouts();
        }

        Time elapsed;
        int centiseconds_elapsed = Time::timediff(start, elapsed);

        // output some stats every few seconds
        // check if we should still search
        if (centiseconds_elapsed - last_update > 250) {
            last_update = centiseconds_elapsed;
            dump_analysis(static_cast<int>(m_playouts));
        }
        keeprunning  = is_running();
        keeprunning &= (centiseconds_elapsed < time_for_move);
        keeprunning &= !playout_limit_reached();
    } while(keeprunning);

    // stop the search
    m_run = false;
    tg.wait_all();
    m_rootstate.stop_clock(white_turn);
    if (!m_root.has_children()) {
        return FastBoard::PASS;
    }

    // display search info
    printf("\n");

    dump_stats(m_rootstate, m_root);
    Training::record(m_rootstate, m_root);

    Time elapsed;
    int centiseconds_elapsed = Time::timediff(start, elapsed);
    if (centiseconds_elapsed > 0) {
        printf("%d visits, %d nodes, %d playouts, %d n/s\n\n",
                 m_root.get_visits(),
                 static_cast<int>(m_nodes),
                 static_cast<int>(m_playouts),
                 (m_playouts * 100) / (centiseconds_elapsed+1));
    }
    int bestmove = get_best_move();
    return bestmove;
}

void UCTSearch::ponder() {
    assert(m_playouts == 0);
    assert(m_nodes == 0);

    m_run = true;
    int cpus = cfg_num_threads;
    Utils::ThreadGroup tg(thread_pool);
    for (int i = 1; i < cpus; i++) {
        tg.add_task(UCTWorker(m_rootstate, this, &m_root));
    }
    do {
        auto currstate = std::make_unique<Board>(m_rootstate);
        auto result = play_simulation(*currstate, &m_root);
        if (result.valid()) {
            increment_playouts();
        }
    } while(!Utils::input_pending() && is_running());

    // stop the search
    m_run = false;
    tg.wait_all();
    // display search info
    printf("\n");
    dump_stats(m_rootstate, m_root);

    printf("\n%d visits, %d nodes\n\n", m_root.get_visits(), (int)m_nodes);
}

void UCTSearch::set_playout_limit(int playouts) {
    static_assert(std::is_convertible<decltype(playouts),
                                      decltype(m_maxplayouts)>::value,
                  "Inconsistent types for playout amount.");
    if (playouts == 0) {
        m_maxplayouts = std::numeric_limits<decltype(m_maxplayouts)>::max();
    } else {
        m_maxplayouts = playouts;
    }
}
