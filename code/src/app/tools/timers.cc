#include "timers.hh"

timer now()
{
	return std::chrono::steady_clock::now();
}
std::string formatTimer(duration dt)
{
	char buffer[1024];
	sprintf(buffer, "%3d ms %03d μs %03d ns",
					std::chrono::duration_cast<std::chrono::milliseconds>(dt).count(),
					std::chrono::duration_cast<std::chrono::microseconds>(dt).count()%1000,
					std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count()%1000);
	return std::string(buffer);
}
