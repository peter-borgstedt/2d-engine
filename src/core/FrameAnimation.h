#ifndef INDEX_ANIMATION_EXTENSION_H
#define INDEX_ANIMATION_EXTENSION_H

#include "Debug.h"
#include "Timer.h"
#include <vector>

class FrameAnimation
{
public:
  struct Frame
  {
    SDL_Rect rectangle;
    int duration;
  };

  FrameAnimation(std::vector<Frame> animation_frames)
      : animation_frames(animation_frames), timer(Timer())
  {
    debug::analyser.type_constructed(this);
  }

  ~FrameAnimation()
  {
    debug::analyser.type_deconstructed(this);
  }

  inline SDL_Rect get_next_source()
  {
    Frame &frame = animation_frames[index = index % animation_frames.size()];
    if (timer.is_obselete(frame.duration)) {
      ++index;
    }
    return frame.rectangle;
  }

private:
  Timer timer;
  int index = 0;
  std::vector<Frame> animation_frames;
};

#endif
