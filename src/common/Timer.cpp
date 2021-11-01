#include "Timer.h"
#include "Debug.h"

Timer::Timer()
{
  debug::analyser.type_constructed(this);
}

Timer::~Timer()
{
  debug::analyser.type_deconstructed(this);
}

const bool Timer::is_obselete(const long &duration)
{
  long ticks = SDL_GetTicks();
  if (last_ticks == -1) // Not started.
  {
    last_ticks = ticks;
    // TODO: should default be false, maybe set this in instantiation?
    return true;
  } else if (ticks - last_ticks > duration) {
    last_ticks = ticks;
    return true;
  }
  return false;
}
