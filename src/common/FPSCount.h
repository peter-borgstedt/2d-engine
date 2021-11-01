#ifndef FPS_COUNTER_H
#  define FPS_COUNTER_h

#  include "Timer.h"
#  include <SDL.h>

class FPSCount
{
public:
  FPSCount();

  void start(int maximum_fps = -1);
  Uint32 get_adjusted_ticks();
  void tick();
  void print();
  int get_avarage_fps();

private:
  void adjust();

  Timer timer;
  bool started;

  Uint32 start_tick;
  int counted_frames;

  int max_fps;
  int previous_tick;
};

#endif
