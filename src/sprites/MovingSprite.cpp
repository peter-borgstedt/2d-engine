#include "MovingSprite.h"
#include "CollisionUtils.h"

std::shared_ptr<MovingSprite> MovingSprite::get_instance(const std::shared_ptr<Image> image,
                                                         const SDL_Rect &src_rectangle,
                                                         const SDL_Rect &position,
                                                         const SDL_Rect &collision_offset,
                                                         int movement_amount, int movement_distance)
{
  return std::shared_ptr<MovingSprite>(new MovingSprite(
      image, src_rectangle, position, collision_offset, movement_amount, movement_distance));
}

MovingSprite::MovingSprite(const std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                           const SDL_Rect &position, const SDL_Rect &collision_offset,
                           int movement_amount, int movement_distance)
    : DynamicSprite(image, src_rectangle, position, collision_offset),
      original_position(position),
      movement_amount(movement_amount),
      movement_distance(movement_distance)
{
  debug::analyser.type_constructed(this);
}

MovingSprite::~MovingSprite()
{
  debug::analyser.type_deconstructed(this);
}

void MovingSprite::tick()
{
  // The dstLocation is original position;
  if (position.x > original_position.x + movement_distance ||
      position.x < original_position.x - movement_distance) {
    movement_amount *= -1;
  }
  position.x += movement_amount;
}

void MovingSprite::determine(std::shared_ptr<Context> context,
                             std::vector<std::shared_ptr<Sprite>> &sprites)
{
  // Do nothing?
}

void MovingSprite::on_collision(const uint8_t &state, std::shared_ptr<Sprite> other)
{
  if (state == COLLIDE_BOTTOM) {
    SDL_Rect &other_position = other->get_position();
    other_position.x += movement_amount;
  }
}
