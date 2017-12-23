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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits>
#include <cmath>
#include <random>
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>

#include "UCTNode.h"
#include "mutex.h"
#include "UCTSearch.h"
#include "Utility.h"
#include "Random.h"


UCTNode::UCTNode(int move_index, float score, float prev_win_rate)
    : m_move(move_index), m_score(score), m_prev_win_rate(prev_win_rate) {
}

UCTNode::~UCTNode() {
    //LOCK(get_mutex(), lock);

	int i = this->possoble_moves.size() - 1;

	for (; i >= 0; i--) {
		delete this->possoble_moves[i];
	}
}

bool UCTNode::first_visit() const {
    return m_visits == 0;
}

SMP::Mutex& UCTNode::get_mutex() {
    return m_nodemutex;
}

float UCTNode::get_cur_score() const
{
	return m_curent_score;
}

void UCTNode::set_cur_score(float score)
{
	m_curent_score = score;
}

bool UCTNode::create_children(std::atomic<int> & nodecount,
                              Board & state,
                              float & win_rate) {
    // check whether somebody beat us to it (atomic)
    if (has_children()) {
        return false;
    }
    // acquire the lock
    LOCK(get_mutex(), lock);
    // check whether somebody beat us to it (after taking the lock)
	if (state.black_win || state.white_win) {
		return false;
	}
    if (has_children()) {
        return false;
    }
    // Someone else is running the expansion
    if (m_is_expanding) {
        return false;
    }
    // We'll be the one queueing this node for expansion, stop others
    m_is_expanding = true;
    lock.unlock();

    auto raw_netlist = FakeNetwork::get_scored_moves(
        &state);

	//if (raw_netlist.first.size() < 20) {
	//	state.print_board();
	//}

    // DCNN returns winrate as side to move
	win_rate = raw_netlist.second;
	bool white_turn = state.white_turn;

	//if (raw_netlist.first.size() < 20) {
	//	state.print_board();
	//}

	// our search functions evaluate from black's point of view
	if (white_turn) {
		win_rate = 1.0f - win_rate;
	}

    std::vector<scored_node> nodelist;

    auto legal_sum = 0.0f;
    for (auto& node : raw_netlist.first) {
        auto move_index = node.second;
        nodelist.emplace_back(node);
        legal_sum += node.first;
    }

    // If the sum is 0 or a denormal, then don't try to normalize.
    if (legal_sum > std::numeric_limits<float>::min()) {
        // re-normalize after removing illegal moves.
        for (auto& node : nodelist) {
            node.first /= legal_sum;
        }
    }

	nodecount += this->add_move_nodes(nodelist, win_rate);

    return true;
}

int UCTNode::add_move_nodes(std::vector<scored_node> nodelist, float win_rate) {
	if (!nodelist.size()) {
		return false;
	}

	//Sort list?
	//std::sort(begin(nodelist), end(nodelist));

	auto childrenadded = 0;

	LOCK(get_mutex(), lock);

	//Add nodes
	for (const auto& node : nodelist) {
		auto vtx = new UCTNode(node.second, node.first, win_rate);
		this->possoble_moves.push_back(vtx);
		childrenadded++;
	}

	this->m_has_children = true;

	//Expand with noise
	if (ConfigStore::get().bools.at("cfg_noise")) {
		this->dirichlet_noise(0.25f, 1.0f);
	}

	return childrenadded;
}

float UCTNode::eval_state(Board& state) {
    auto raw_netlist = FakeNetwork::get_scored_moves(
        &state);

    // DCNN returns winrate as side to move
    auto net_eval = raw_netlist.second;

    // But we score from black's point of view
    if (state.white_turn) {
        net_eval = 1.0f - net_eval;
    }

    return net_eval;
}

void UCTNode::dirichlet_noise(float epsilon, float alpha) {
    auto dirichlet_vector = std::vector<float>{};

	std::gamma_distribution<float> gamma(alpha, 1.0f);
	std::default_random_engine generator(std::random_device{}());

    for (size_t i = 0; i < this->possoble_moves.size(); i++) {
        dirichlet_vector.emplace_back(gamma(generator));
    }

    auto sample_sum = std::accumulate(begin(dirichlet_vector),
                                      end(dirichlet_vector), 0.0f);

    // If the noise vector sums to 0 or a denormal, then don't try to
    // normalize.
    if (sample_sum < std::numeric_limits<float>::min()) {
        return;
    }

    for (auto& v: dirichlet_vector) {
        v /= sample_sum;
    }

	//Randomize the noise to the nodes
	std::random_shuffle(dirichlet_vector.begin(), dirichlet_vector.end());

	for (size_t i = 0; i < this->possoble_moves.size(); i++){
		auto score = this->possoble_moves[i]->get_score();
		auto eta_a = dirichlet_vector[i];
		score = score * (1 - epsilon) + epsilon * eta_a;
		this->possoble_moves[i]->set_score(score);
	}
}

int UCTNode::get_move() const {
    return m_move;
}

void UCTNode::virtual_loss() {
    m_virtual_loss += VIRTUAL_LOSS_COUNT;
}

void UCTNode::virtual_loss_undo() {
    m_virtual_loss -= VIRTUAL_LOSS_COUNT;
}

void UCTNode::update(float eval) {
    m_visits++;
    accumulate_eval(eval);
}

bool UCTNode::has_children() const {
    return m_has_children;
}

void UCTNode::set_visits(uint64_t visits) {
    m_visits = visits;
}

float UCTNode::get_score() const {
    return m_score;
}

float UCTNode::get_prev_score() const
{
	return this->m_prev_win_rate;
}

void UCTNode::set_score(float score) {
    m_score = score;
}

int UCTNode::get_visits() const {
    return m_visits;
}

float UCTNode::get_eval(Player turn) const {
    // Due to the use of atomic updates and virtual losses, it is
    // possible for the visit count to change underneath us. Make sure
    // to return a consistent result to the caller by caching the values.
    auto virtual_loss = uint64_t{m_virtual_loss};
    auto visits = get_visits() + virtual_loss;
    if (visits > 0) {
        auto black_wins = get_black_wins();
        if (turn == White) {
			black_wins += virtual_loss;
        }
        auto score = static_cast<float>(black_wins / (double)visits);
        if (turn == White) {
            score = 1.0f - score;
        }
        return score;
    } else {
        // If a node has not been visited yet,
        // the eval is that of the parent.
        auto eval = m_prev_win_rate;
        if (turn == White) {
            eval = 1.0f - eval;
        }
        return eval;
    }
}

double UCTNode::get_black_wins() const {
	return m_black_win;
}

double UCTNode::get_percent_black_wins() const {
	return m_black_win / m_visits;
}

void UCTNode::set_black_wins(double black_wins) {
	m_black_win = black_wins;
}

void UCTNode::accumulate_eval(double win) {
	Utils::atomic_add(m_black_win, win);
}

UCTNode* UCTNode::uct_select_child(Player turn) {
    UCTNode * best = nullptr;
    float best_value = -1000.0f;

    LOCK(get_mutex(), lock);

    // Count parentvisits.
    // We do this manually to avoid issues with transpositions.
    int parentvisits = 0;

    // Make sure we are at a valid successor.
	for (size_t i = 0; i < this->possoble_moves.size(); i++)
	{
		parentvisits += this->possoble_moves[i]->get_visits();
	}
 
    float numerator = std::sqrt((double)parentvisits);

	//update values
	for (size_t i = 0; i < this->possoble_moves.size(); i++){
		float winrate = this->possoble_moves[i]->get_eval(turn);
		float psa = this->possoble_moves[i]->get_score();
		float denom = 1.0f + this->possoble_moves[i]->get_visits();
		float puct = ConfigStore::get().doubles.at("cfg_puct") * psa * (numerator / denom);
		float value = winrate + puct;
		this->possoble_moves[i]->set_cur_score(value);

		if (value > best_value) {
			best_value = value;
			best = this->possoble_moves[i];
		}
	}

    assert(best != nullptr);

    return best;
}

void UCTNode::sort_moves(Player turn) {
    LOCK(get_mutex(), lock);

	//Is White
	if (turn == White) {
		std::sort(this->possoble_moves.begin(), this->possoble_moves.end(), [](UCTNode const* a, UCTNode const* b) {
			if (a->get_visits() > b->get_visits()) {
				return true;
			}
			else if (a->get_visits() < b->get_visits()) {
				return false;
			}
			else {
				if (a->get_visits() == 0) {
					if (a->get_prev_score() > b->get_prev_score()) {
						return true;
					}
					else if (a->get_prev_score() < b->get_prev_score()) {
						return false;
					}
					else {
						return a->get_score() > b->get_score();
					}
				}
				else {
					return a->get_black_wins() < b->get_black_wins();
				}
			}

		});
	}
	else {
		std::sort(this->possoble_moves.begin(), this->possoble_moves.end(), [](UCTNode const* a, UCTNode const* b) {
			if (a->get_visits() > b->get_visits()) {
				return true;
			}
			else if (a->get_visits() < b->get_visits()) {
				return false;
			}
			else {
				if (a->get_visits() == 0) {
					if (a->get_prev_score() > b->get_prev_score()) {
						return true;
					}
					else if (a->get_prev_score() < b->get_prev_score()) {
						return false;
					}
					else {
						return a->get_score() > b->get_score();
					}
				}
				else {
					return a->get_black_wins() > b->get_black_wins();
				}
			}

		});
	}

}


UCTNode* UCTNode::get_best_root_child(Player turn) {
 	assert(this->possoble_moves.size() != 0);
	sort_moves(turn);
	
	return this->possoble_moves.at(0);
}
