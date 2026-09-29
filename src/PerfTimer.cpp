// ----------------------------------------------------
// Fast timer with microsecond precision
// ----------------------------------------------------

#include "PerfTimer.h"

Uint64 PerfTimer::frequency = SDL_GetPerformanceFrequency();

// L02: TODO 2: Fill Constructor, Start(), ReadMs() and ReadTicks() methods
// they are simple, one line each!

PerfTimer::PerfTimer()
{
	//...
}

void PerfTimer::Start()
{
	//...
}

double PerfTimer::ReadMs() const
{
	return 0.0f;
}

Uint64 PerfTimer::ReadTicks() const
{
	return 0;
}
