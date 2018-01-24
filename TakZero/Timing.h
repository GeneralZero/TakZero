#pragma once

#include "config.h"

#include <time.h>

class Time {
public:
	/*
	sets to current time
	*/
	Time(void);

	/*
	time difference in centiseconds
	*/
	static int timediff(Time start, Time end);

private:
	time_t m_time;
};
