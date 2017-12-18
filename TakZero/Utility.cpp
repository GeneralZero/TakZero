#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <thread>
#include <mutex>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/select.h>
#endif

#include "config.h"
#include "Utility.h"

Utils::ThreadPool thread_pool;

bool Utils::input_pending(void) {
#ifdef HAVE_SELECT
	fd_set read_fds;
	struct timeval timeout;
	FD_ZERO(&read_fds);
	FD_SET(0, &read_fds);
	timeout.tv_sec = timeout.tv_usec = 0;
	select(1, &read_fds, NULL, NULL, &timeout);
	if (FD_ISSET(0, &read_fds)) {
		return true;
	}
	else {
		return false;
	}
#else
	static int init = 0, pipe;
	static HANDLE inh;
	DWORD dw;

	if (!init) {
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);
		if (!pipe) {
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
		}
	}

	if (pipe) {
		if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) {
			myprintf("Nothing at other end - exiting\n");
			exit(EXIT_FAILURE);
		}

		if (dw) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		if (!GetNumberOfConsoleInputEvents(inh, &dw)) {
			myprintf("Nothing at other end - exiting\n");
			exit(EXIT_FAILURE);
		}

		if (dw <= 1) {
			return false;
		}
		else {
			return true;
		}
	}
	return false;
#endif
}

static std::mutex IOmutex;

void Utils::myprintf(const char *fmt, ...) {
	if (ConfigStore::get().bools.at("cfg_quiet")) return;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (!cfg_logfile_handle.is_open()) {

		cfg_logfile_handle.open("Tak.log", std::fstream::out);
	}
	
	std::lock_guard<std::mutex> lock(IOmutex);
	va_start(ap, fmt);
	cfg_logfile_handle << fmt << ap;
	va_end(ap);
}

void Utils::log_input(std::string input) {
	if (!cfg_logfile_handle.is_open()) {

		cfg_logfile_handle.open("Tak.log", std::fstream::out);
	}

	std::lock_guard<std::mutex> lock(IOmutex);
	cfg_logfile_handle << ">>%s\n" << input.c_str();

}