#include "Utility.h"

namespace utility
{
  // TODO: we probably want to add logic to att movable extension for sprites.
  void move(SDL_Rect &position, const Direction &direction, const int &amount)
  {
    switch (direction) {
      case kUP:
        position.y -= amount;
        break; // Not used. TODO: support 3/4 view? now only side view.
      case kDOWN:
        position.y += amount;
        break; // Not used. TODO: support 3/4 view? now only side view.
      case kRIGHT:
        position.x += amount;
        break;
      case kLEFT:
        position.x -= amount;
        break;
      case kNONE:
        break;
    }
  }
} // namespace utility
