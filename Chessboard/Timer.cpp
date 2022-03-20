
#include "Timer.h"
#include <time.h>
#include <stdexcept>

void delay(int ns)
{
	timespec ts = {};
	ts.tv_sec = 0;
	ts.tv_nsec = ns;
	int result = nanosleep(&ts, nullptr);
	if (result < 0)
		throw std::runtime_error("nanosleep failed");
}
