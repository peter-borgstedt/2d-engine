#include "PhysicsSprite.h"
#include "LogicExtension.h"

using namespace std;
using namespace collision_utils;

PhysicsSprite::PhysicsSprite(const shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                             const SDL_Rect &position, const SDL_Rect &collision_offset)
    : DynamicSprite(image, src_rectangle, position, collision_offset),
      physics(std::shared_ptr<physics::Physics>(new physics::Physics()))
{
  debug::analyser.type_constructed(this);
}

PhysicsSprite::~PhysicsSprite()
{
  debug::analyser.type_deconstructed(this);
}

shared_ptr<PhysicsSprite> PhysicsSprite::get_instance(const shared_ptr<Image> image,
                                                      const SDL_Rect &src_rectangle,
                                                      const SDL_Rect &position,
                                                      const SDL_Rect &collision_offset)
{
  return shared_ptr<PhysicsSprite>(
      new PhysicsSprite(image, src_rectangle, position, collision_offset));
}

void PhysicsSprite::tick()
{
  previous_x = position.x;
  previous_y = position.y;
  physics::apply(position, physics);
}

void PhysicsSprite::determine(std::shared_ptr<Context> context,
                              std::vector<std::shared_ptr<Sprite>> &sprites)
{
  // First start with previous, it may be adjusted.
  int x_adjustment = previous_x;

  SDL_Rect &a = position;
  for (auto other : sprites) {
    if (other.get() == this) {
      continue;
    }

    // Do an eventual logic on sprite, if this has the correct criteria.
    // Collision is not a necessary to trigger this.
    if (auto logic = dynamic_pointer_cast<LogicExtension>(other)) {
      logic->do_logic(context, Sprite::shared_from_this());
    }

    // Validate collision on given rectangles, this is done in two step:
    // 1, check if rectangles are aligned both horizontally and vertically to
    // previous y position. If there is a horizontal collide, adjust the x
    // position. 2, check if rectangles are aligned both horizontally and
    // vertically to previous x position. If there is a horizontal collide,
    // adjust the x position.
    if (other->is_collidable()) // Collision rectangle has been set.
    {
      const SDL_Rect b = other->get_collision_position();

      uint8_t h_state = horizontal(a.x, previous_y, a.w, a.h, b.x, b.y, b.w, b.h);
      if (h_state) {
        on_x_collisions(h_state, other, x_adjustment);
      }

      uint8_t v_state = vertical(x_adjustment, a.y, a.w, a.h, b.x, b.y, b.w, b.h);
      if (v_state) {
        on_y_collision(v_state, other);
      }

      if (h_state || v_state) {
        physics::determine(physics, h_state, v_state); // Adjust physics.

        // Send collision event to sprite.
        other->on_collision(h_state | v_state, Sprite::shared_from_this());
      }
    }
  }
}

void PhysicsSprite::adjust_x_axis(const uint8_t &h_state, SDL_Rect &a, SDL_Rect &b)
{
  a.x = h_state & COLLIDE_LEFT ? b.x - a.w : b.x + b.w;
}

void PhysicsSprite::adjust_y_axis(const uint8_t &v_state, SDL_Rect &a, SDL_Rect &b)
{
  a.y = v_state & COLLIDE_BOTTOM ? b.y - a.h : b.y + b.h;
}

void PhysicsSprite::on_x_collisions(const uint8_t &h_state, const std::shared_ptr<Sprite> other,
                                    int &x_adjustment)
{
  if (dynamic_pointer_cast<PhysicsSprite>(other)) {
    return;
  }

  adjust_x_axis(h_state, position, other->get_position());
  // X position has been adjusted, we want to use this for now on when
  // validating y collision, instead of previous X position.
  x_adjustment = position.x;
}

void PhysicsSprite::on_y_collision(const uint8_t &v_state, const std::shared_ptr<Sprite> other)
{
  if (dynamic_pointer_cast<PhysicsSprite>(other)) {
    return;
  }
  adjust_y_axis(v_state, position, other->get_position());
}

void PhysicsSprite::on_collision(const uint8_t &state, std::shared_ptr<Sprite> other)
{
  uint8_t horizontal_state = retrieve_horizontal_state(state);
  uint8_t vertical_state = retrieve_vertical_state(state);
  if (horizontal_state & COLLIDE_RIGHT) {
    physics->velocity_x = -physics->velocity_x_strength;
  }
  if (horizontal_state & COLLIDE_LEFT) {
    physics->velocity_x = physics->velocity_x_strength;
  }
  if (vertical_state & COLLIDE_BOTTOM) {
    physics->velocity_y = -physics->velocity_y_strength;
  }
  if (vertical_state & COLLIDE_TOP) {
    physics->velocity_y = physics->velocity_y_strength;
  }
}
