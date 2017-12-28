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
#include "Training.h"
#include <random>

#include <cassert>

extern Utils::ThreadPool thread_pool;

UCTSearch::UCTSearch(Board & g)
	: m_rootstate(g) {
	set_playout_limit(ConfigStore::get().ints.at("cfg_max_playouts"));
	this->m_root = new UCTNode{ -1, 0.0f, 0.5f };
}
UCTSearch::~UCTSearch() {
	delete m_root;
}

SearchResult UCTSearch::play_simulation(Board & currstate, UCTNode* const node) {
	Player turn;
	if (currstate.white_turn) {
		turn = White;
	}
	else {
		turn = Black;
	}
	auto hash = currstate.get_hash();

	TTable::get_TT()->sync(hash, node);

	auto result = SearchResult{};

	node->virtual_loss();

	//If not root_node or visited node
	if (!node->has_children()) {
		//Check if Game has ended
		if (currstate.black_win && currstate.white_win) {
			this->tie_win++;
			result = SearchResult::from_score(3.0);
		}
		else if (currstate.black_win) {
			this->black_win++;
			result = SearchResult::from_score((float)Black);
		}
		else if (currstate.white_win) {
			this->white_win++;
			result = SearchResult::from_score((float)White);
		}
		else if (m_nodes < MAX_TREE_SIZE) {
			float eval;

			//Create All children 
			auto success = node->create_children(m_nodes, currstate, eval);
			if (success) {
				result = SearchResult::from_eval(eval);
			}
		}
		else {
			auto eval = node->eval_state(currstate);
			result = SearchResult::from_eval(eval);
		}
	}
	
	uint move;

	if (node->has_children() && !result.valid()) {
		//Select part of Monte Carlo
		if(node->possoble_moves.size() == 0){
			currstate.print_board();
		}
		auto next = node->uct_select_child(turn);


		if (next != nullptr) {
			move = next->get_move();

			currstate.PlayIndex(move);
			result = play_simulation(currstate, next);
		}
	}

	if (result.valid()) {
		//currstate.print_board();
		node->update(result.eval());
	}
	node->virtual_loss_undo();
	TTable::get_TT()->update(hash, node);

	return result;
}

void UCTSearch::dump_stats(Board & state, UCTNode & parent) {
	if (ConfigStore::get().bools.at("cfg_quiet") || !parent.has_children()) {
		return;
	}
	Player turn;
	if (state.white_turn) {
		turn = White;
	}
	else {
		turn = Black;
	}

	// sort moves, put best move on top
	m_root->sort_moves(turn);

	UCTNode * bestnode = parent.get_best_root_child(turn);

	if (bestnode->first_visit()) {
		return;
	}

	uint movecount = 0;

	for (uint i = 0; i < 5 && i< parent.possoble_moves.size(); i++) {
		if (++movecount > 2 && !parent.possoble_moves[i]->get_visits()) break;

		std::string tmp = std::to_string(parent.possoble_moves[i]->get_move());
		std::string pvstring(tmp);

		Utils::myprintf("%4s -> %7d (Win: %5.2f%%) (Score: %5.2f%%) Moves: ",
			tmp.c_str(),
			parent.possoble_moves[i]->get_visits(),
			parent.possoble_moves[i]->get_visits() > 0 ? parent.possoble_moves[i]->get_eval(turn)*100.0f : 0.0f,
			parent.possoble_moves[i]->get_score() * 100.0f);

		Board tmpstate = state;

		tmpstate.PlayIndex(parent.possoble_moves[i]->get_move());
		pvstring += " " + get_pv(tmpstate, *parent.possoble_moves[i], 0);

		Utils::myprintf("%s\n", pvstring.c_str());
	}
}

int UCTSearch::get_best_move(Player turn) {
	// Make sure best is first
	m_root->sort_moves(turn);

	auto node = m_root->get_best_root_child(turn);

	// do we have statistics on the moves?
	if (node != nullptr) {
		if (node->first_visit()) {
			return node->get_move();
		}
	}

	float bestscore = node->get_eval(turn);

	int visits = m_root->get_visits();

	//If lost end game
	size_t movetresh = (m_rootstate.SIZE * m_rootstate.SIZE) * 4;
	// bad score and visited enough
	if (bestscore < ((float)ConfigStore::get().doubles.at("cfg_resignpct") / 100.0f)
		&& visits > 500
		&& m_rootstate.move_number > movetresh) {
		Utils::myprintf("Score looks bad. Resigning.\n");
		return -1;
	}

	return node->get_move();
}

std::string UCTSearch::get_pv(Board & state, UCTNode & parent, uint8_t depth) {
	if (!parent.has_children()) {
		return std::string();
	}
	Player turn;
	if (state.white_turn) {
		turn = White;
	}
	else {
		turn = Black;
	}

	auto best_child = parent.get_best_root_child(turn);
	auto best_move = best_child->get_move();
	auto res = std::to_string(best_move);

	state.PlayIndex(best_move);

	if (depth< 3) {
		auto next = get_pv(state, *best_child, depth + 1);
		res.append(" ").append(next);
	}
	return res;
}

void UCTSearch::dump_analysis(uint playouts) {
	if (ConfigStore::get().bools.at("cfg_quiet")) {
		return;
	}
	Player turn;
	Board tempstate = m_rootstate;
	if (tempstate.white_turn) {
		turn = White;
	}
	else {
		turn = Black;
	}

	std::string pvstring = get_pv(tempstate, *m_root, 0);
	float winrate = 100.0f * m_root->get_eval(turn);
	Utils::myprintf("Playouts: %d, Win: %5.2f%%, PV: %s\n",
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
		auto currstate = std::make_unique<Board>(m_rootstate);
		auto result = m_search->play_simulation(*currstate, m_root);
		if (result.valid()) {
			m_search->increment_playouts();
		}
	} while(m_search->is_running() && !m_search->playout_limit_reached());
}

void UCTSearch::increment_playouts() {
	m_playouts++;
}

int UCTSearch::think(Player turn, Training training) {
	// Start counting time for us
	m_rootstate.start_clock(turn);

	// set side to move
	//m_rootstate.white_turn = color;

	// set up timing info
	Time start;

	m_rootstate.get_timecontrol().set_boardsize(m_rootstate.SIZE);
	uint time_for_move = m_rootstate.get_timecontrol().max_time_for_move(turn);

	if (turn == White) {
		Utils::myprintf("White Turn\n");
	}
	else {
		Utils::myprintf("Black Turn\n");
	}
	
	Utils::myprintf("Thinking at most %.1f seconds...\n", time_for_move/100.0f);

	// Select & Expand
	float root_eval = 0.5;

	m_root->create_children(m_nodes, m_rootstate, root_eval);
	
 
	//Expand with noise
	if (ConfigStore::get().bools.at("cfg_noise")) {
		m_root->dirichlet_noise(0.03f, 1.0f);
	}

	Utils::myprintf("NN winrate_on_board=%f\n",
			 (root_eval));

	//Rollout & back prop
	m_run = true;
	uint cpus = ConfigStore::get().ints.at("cfg_num_threads");
	Utils::ThreadGroup tg(thread_pool);
	for (uint i = 1; i < cpus; i++) {
		tg.add_task(UCTWorker(m_rootstate, this, m_root));
	}

	bool keeprunning = true;
	uint last_update = 0;
	do {
		auto currstate = std::make_unique<Board>(m_rootstate);

		auto result = play_simulation(*currstate, m_root);
		if (result.valid()) {
			increment_playouts();
		}

		Time elapsed;
		uint centiseconds_elapsed = Time::timediff(start, elapsed);

		// output some stats every few seconds
		// check if we should still search
		if (centiseconds_elapsed - last_update > 250) {
			last_update = centiseconds_elapsed;
			dump_analysis(static_cast<uint>(m_playouts));
		}
		keeprunning  = is_running();
		keeprunning &= (centiseconds_elapsed < time_for_move);
		keeprunning &= !playout_limit_reached();
	} while(keeprunning);

	// stop the search
	m_run = false;
	tg.wait_all();
	m_rootstate.stop_clock(turn);

	// display search info
	Utils::myprintf("\n");

	dump_stats(m_rootstate, *m_root);
	training.record(m_rootstate, *m_root);

	Time elapsed;
	uint centiseconds_elapsed = Time::timediff(start, elapsed);
	if (centiseconds_elapsed > 0) {
		Utils::myprintf("%d visits, %d nodes, %d playouts, %d n/s\n\n",
				 m_root->get_visits(),
				 static_cast<uint>(m_nodes),
				 static_cast<uint>(m_playouts),
				 (m_playouts * 100) / (centiseconds_elapsed+1));
	}
	int bestmove = get_best_move(turn);

	return bestmove;
}

void UCTSearch::swap_root(int move_number) {
	//Delete other than the move_number

	UCTNode* new_root;
	UCTNode* old_root;
	uint64_t size = 0;
	//best node is the first node so delete others
	for (size_t i = 0; i < this->m_root->possoble_moves.size(); i++)
	{
		
		//It is most likely index 0 but make sure
		//if (this->m_root->possoble_moves[i]->get_move() != move_number) {
		//size += this->m_root->possoble_moves[i]->get_visits();
		delete this->m_root->possoble_moves[i];
			//this->m_root->possoble_moves.erase(this->m_root->possoble_moves.begin() + i);
	}

	//Change rootnode
	old_root = this->m_root;
	this->m_root = new_root;

	old_root->possoble_moves.clear();
	delete old_root;
	

	//New Count new nodes
	this->m_nodes = 0;
	this->m_playouts = 0;
}

void UCTSearch::ponder() {
	assert(m_playouts == 0);
	assert(m_nodes == 0);

	m_run = true;
	uint cpus = ConfigStore::get().ints.at("cfg_num_threads") ;
	Utils::ThreadGroup tg(thread_pool);
	for (uint i = 1; i < cpus; i++) {
		tg.add_task(UCTWorker(m_rootstate, this, m_root));
	}
	do {
		//Search Part of Monte Carlo
		auto currstate = std::make_unique<Board>(m_rootstate);
		auto result = play_simulation(*currstate, m_root);
		if (result.valid()) {
			increment_playouts();
		}
	} while(!Utils::input_pending() && is_running());

	// stop the search
	m_run = false;
	tg.wait_all();
	// display search info
	Utils::myprintf("\n");
	dump_stats(m_rootstate, *m_root);

	Utils::myprintf("\n%d visits, %d nodes\n\n", m_root->get_visits(), (int)m_nodes);
}

void UCTSearch::set_playout_limit(uint playouts) {
	static_assert(std::is_convertible<decltype(playouts),
									  decltype(m_maxplayouts)>::value,
				  "Inconsistent types for playout amount.");
	if (playouts == 0) {
		m_maxplayouts = std::numeric_limits<decltype(m_maxplayouts)>::max();
	} else {
		m_maxplayouts = playouts;
	}
}
