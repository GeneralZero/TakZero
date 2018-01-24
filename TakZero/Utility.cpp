#include "config.h"

#include <iostream>
#include <fstream>
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

#include "Utility.h"

Utils::ThreadPool thread_pool;

bool Utils::input_pending(void) {
	return false;
}

static std::mutex IOmutex;

void Utils::myprintf(const char *fmt, ...) {
	if (ConfigStore::get().bools.at("cfg_quiet")) return;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	/*
	if (cfg_logfile_handle) {
		std::lock_guard<std::mutex> lock(IOmutex);
		va_start(ap, fmt);
		vfprintf(cfg_logfile_handle, fmt, ap);
		va_end(ap);
	}
	*/
}

void Utils::gtp_printf(int id, const char *fmt, ...) {
	va_list ap;

	if (id != -1) {
		fprintf(stdout, "=%d ", id);
	}
	else {
		fprintf(stdout, "= ");
	}

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
	printf("\n\n");
	/*
	if (cfg_logfile_handle) {
		std::lock_guard<std::mutex> lock(IOmutex);
		if (id != -1) {
			fprintf(cfg_logfile_handle, "=%d ", id);
		}
		else {
			fprintf(cfg_logfile_handle, "= ");
		}
		va_start(ap, fmt);
		vfprintf(cfg_logfile_handle, fmt, ap);
		va_end(ap);
		fprintf(cfg_logfile_handle, "\n\n");
	}
	*/
}

void Utils::gtp_fail_printf(int id, const char *fmt, ...) {
	va_list ap;

	if (id != -1) {
		fprintf(stdout, "?%d ", id);
	}
	else {
		fprintf(stdout, "? ");
	}

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
	printf("\n\n");
	/*
	if (cfg_logfile_handle) {
		std::lock_guard<std::mutex> lock(IOmutex);
		if (id != -1) {
			fprintf(cfg_logfile_handle, "?%d ", id);
		}
		else {
			fprintf(cfg_logfile_handle, "? ");
		}
		va_start(ap, fmt);
		vfprintf(cfg_logfile_handle, fmt, ap);
		va_end(ap);
		fprintf(cfg_logfile_handle, "\n\n");
	}
	*/
}

void Utils::log_input(std::string input) {
	/*
	if (cfg_logfile_handle) {
		std::lock_guard<std::mutex> lock(IOmutex);
		fprintf(cfg_logfile_handle, ">>%s\n", input.c_str());
	}
	*/
}