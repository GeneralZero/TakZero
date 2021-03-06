#pragma once

#include <array>
#include "Timing.h"
#include "Move.h"

class TimeControl {
public:
	/*
	Initialize time control. Timing info is per GTP and in centiseconds
	*/
	TimeControl(int boardsize = 19,
		int maintime = 60 * 60 * 100,
		int byotime = 0, int byostones = 25,
		int byoperiods = 0);

	void set_boardsize(int boardsize);
	int get_remaining_time(Player color);
	void display_times();
	int max_time_for_move(Player color);
	void adjust_time(Player color, int time, int stones);
	void reset_clocks();
	void start(Player color);
	void stop(Player color);

private:
	int m_maintime;
	int m_byotime;
	int m_byostones;
	int m_byoperiods;
	int m_moves_expected;

	std::array<int, 2> m_remaining_time;    /* main time per player */
	std::array<int, 2> m_stones_left;       /* stones to play in byo period */
	std::array<int, 2> m_periods_left;      /* byo periods */
	std::array<bool, 2> m_inbyo;             /* player is in byo yomi */

	std::array<Time, 2> m_times;             /* storage for player times */
};