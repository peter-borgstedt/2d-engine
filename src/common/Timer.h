#ifndef TIMER_H
#define TIMER_H

#include <SDL.h>

class Timer
{
private:
  long last_ticks = -1;

public:
  Timer();
  ~Timer();

  const bool is_obselete(const long &duration);
};

#endif
