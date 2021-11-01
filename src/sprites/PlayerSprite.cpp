#include "PlayerSprite.h"
#include "LogicExtension.h"

using namespace std;

PlayerSprite::PlayerSprite(const shared_ptr<Image> image, const SDL_Rect &position,
                           const SDL_Rect &collision_offset, const animation_map &animation,
                           const Direction &direction, const int &movement_amount)
    : PhysicsSprite(image, {0, 0, 0, 0}, position, collision_offset),
      animation(animation),
      direction(direction),
      movement_amount(movement_amount)
{
  order = 1;
  debug::analyser.type_constructed(this);
  physics->velocity_x_strength = 0;
  physics->max_velocity_x = 0;

  physics->friction = 0;
  physics->elastics_strength = 0;
}

PlayerSprite::~PlayerSprite()
{
  debug::analyser.type_deconstructed(this);
}

shared_ptr<PlayerSprite>
PlayerSprite::get_instance(const shared_ptr<Image> image, const SDL_Rect &position,
                           const SDL_Rect &collision_area, const animation_map &animation,
                           const Direction &direction, const int &movement_amount)
{
  return shared_ptr<PlayerSprite>(
      new PlayerSprite(image, position, collision_area, animation, direction, movement_amount));
}

void PlayerSprite::paint(SDL_Renderer *renderer, const int &offset_x, const int &offset_y)
{
  SDL_Rect animated_area = get_source_rectangle();
  SDL_Rect offset_area = position;
  offset_area.x -= offset_x;
  offset_area.y -= offset_y;
  SDL_RenderCopy(renderer, image->get_texture(), &animated_area, &offset_area);
}

void PlayerSprite::tick()
{
  previous_x = position.x;
  previous_y = position.y;
  PhysicsSprite::tick();
  if (is_moving) {
    utility::move(position, direction, 4);
  }
}

void PlayerSprite::on_collision(const uint8_t &state, std::shared_ptr<Sprite> other)
{
  // Do nothing for now.
}

void PlayerSprite::set_movement(bool move)
{
  is_moving = move;
}

void PlayerSprite::set_direction(const Direction &direction)
{
  PlayerSprite::direction = direction;
}

void PlayerSprite::stop_if_direction_set(const Direction &direction)
{
  if (PlayerSprite::direction == direction) {
    is_moving = false;
  }
}

void PlayerSprite::jump()
{
  if (!physics->in_air) {
    physics->velocity_y = physics->velocity_y_strength;
  }
}

void PlayerSprite::cancel_jump()
{
  if (physics->velocity_y < physics->max_velocity_y) {
    physics->velocity_y = physics->max_velocity_y;
  }
}

Direction &PlayerSprite::get_direction()
{
  return direction;
}

SDL_Rect &PlayerSprite::get_source_rectangle()
{
  if (animation.count(direction)) {
    if (!is_moving) {
      animation_index = 0;
    }
    if (physics->in_air) {
      animation_index = 3;
    }

    src_rectangle = animation.at(direction)[animation_index];

    if (is_moving && !physics->in_air) {
      if (timer.is_obselete(50)) {
        animation_index = ++animation_index % 8;
      }
    }
  } else {
    src_rectangle = animation.at(kRIGHT)[0];
  }
  return src_rectangle;
}
