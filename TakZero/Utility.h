#include "Move.h"

#include "config.h"
#include <string>
#include <atomic>
#include "ThreadPool.h"

extern Utils::ThreadPool thread_pool;

namespace Utils {
	void myprintf(const char *fmt, ...);
	void gtp_printf(int id, const char *fmt, ...);
	void gtp_fail_printf(int id, const char *fmt, ...);
	void log_input(std::string input);
	bool input_pending();

	template<class T>
	void atomic_add(std::atomic<T> &f, T d) {
		T old = f.load();
		while (!f.compare_exchange_weak(old, old + d));
	}

	template<typename T>
	T rotl(const T x, const int k) {
		return (x << k) | (x >> (std::numeric_limits<T>::digits - k));
	}

	inline bool is7bit(int c) {
		return c >= 0 && c <= 127;
	}
}