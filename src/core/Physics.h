#ifndef PHYSICS_H
#define PHYSICS_H

#include <SDL.h>
#include <algorithm>
#include <memory>

#include "CollisionUtils.h"
#include <iostream>

namespace physics
{
  struct Physics
  {
    bool in_air = false;

    int gravity = 1;
    float friction = 0.98f; // TODO: implement on objects?

    float elastics_strength = 0.8f;

    float velocity_x_strength = 24.0f;
    int velocity_y_strength = -24;

    int max_velocity_y = -12;
    int max_velocity_x = -12;

    float velocity_x = 0;
    int velocity_y = 0; // Intentionally int for clipping.
  };

  inline void apply(SDL_Rect &position, std::shared_ptr<Physics> physics)
  {
    if (physics->velocity_x != 0) {
      physics->velocity_x = physics->velocity_x * physics->friction;
      position.x += static_cast<int>(physics->velocity_x);
    }

    physics->velocity_y += physics->gravity;
    position.y += static_cast<int>(physics->velocity_y);

    if (!physics->in_air && physics->velocity_y != physics->gravity) {
      physics->in_air = true;
    }
  }

  inline void determine(std::shared_ptr<Physics> physics, const uint8_t &horizontal_state,
                        const uint8_t &vertical_state)
  {
    if (horizontal_state & (COLLIDE_LEFT | COLLIDE_RIGHT)) {
      // Hit left or right side.
      physics->velocity_x *= -1;
      return;
    } else if (vertical_state & COLLIDE_TOP) {
      // Hit roof.
      physics->velocity_y *= -1;
      return;
    } else if (vertical_state & COLLIDE_BOTTOM) {
      // Hit floor
      if (physics->elastics_strength > 0) // Apply elastics.
      {
        float elastics = -physics->velocity_y * physics->elastics_strength;
        physics->velocity_y = static_cast<int>(elastics);
      } else {
        physics->velocity_y = 0;
        physics->in_air = false;
      }
    }
  }
} // namespace physics

#endif
