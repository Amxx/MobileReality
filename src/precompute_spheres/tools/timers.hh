#ifndef TIMERS_HH
#define TIMERS_HH

#include <ctime>
#include <ratio>
#include <chrono>

#include <string>

typedef			std::chrono::steady_clock::time_point													timer;
typedef			std::chrono::duration<long int, std::ratio<1l, 1000000000l>>	duration;

timer now();
std::string formatTimer(duration);

#endif