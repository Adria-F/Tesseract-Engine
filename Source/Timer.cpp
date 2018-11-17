#include "Timer.h"

Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	running = true;
	if (isPaused)
	{
		started_at = SDL_GetTicks()-stopped_at;
	}
	else
	{
		started_at = SDL_GetTicks();
	}
}

void Timer::Stop()
{
	running = false;
	isPaused = false;
	stopped_at = SDL_GetTicks();
}

Uint32 Timer::ReadTime()
{
	if(running == true)
	{
		return SDL_GetTicks() - started_at;
	}
	else
	{
		return stopped_at;
	}
}

void Timer::PauseTimer()
{
	stopped_at = SDL_GetTicks() - started_at;
	isPaused = true;
	running = false;
}
