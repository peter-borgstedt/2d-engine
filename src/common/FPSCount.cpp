#include "FPSCount.h"
#include <iostream>

using namespace std;

FPSCount::FPSCount()
    : timer(), started(false), start_tick(0), counted_frames(0), max_fps(-1), previous_tick(-1)
{
}

void FPSCount::start(int max_fps)
{

  // TODO: add possibility to stop the counter.
  if (!started) {
    FPSCount::max_fps = max_fps;
    // This give a start FPS (base tick to calculate rate from), else the rate
    // would start from 0 and then gradely grow to the actual rate.
    start_tick = SDL_GetTicks();
    started = true;
  }
}

Uint32 FPSCount::get_adjusted_ticks()
{
  return SDL_GetTicks() - start_tick;
}

void FPSCount::tick()
{
  ++counted_frames;
  if (max_fps > 0) {
    adjust();
  }
}

int FPSCount::get_avarage_fps()
{
  return static_cast<int>(counted_frames / (get_adjusted_ticks() / 1000.0f));
}

void FPSCount::adjust()
{
  int diff = SDL_GetTicks() - previous_tick;
  if (diff < (1000 / max_fps)) {
    SDL_Delay((1000 / max_fps) - diff); // Sleep remaining time.
  }
  previous_tick = SDL_GetTicks();
}

void FPSCount::print()
{
  if (timer.is_obselete(500)) {
    cout << "FPS:" << get_avarage_fps() << endl;
  }
}
