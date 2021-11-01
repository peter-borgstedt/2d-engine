#ifndef MOVING_SPRITE_H
#define MOVING_SPRITE_H

#include "Common.h"
#include "Debug.h"
#include "DynamicSprite.h"
#include "Timer.h"

class MovingSprite : public DynamicSprite
{
public:
  static std::shared_ptr<MovingSprite>
  MovingSprite::get_instance(const std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                             const SDL_Rect &position, const SDL_Rect &collision_offset,
                             int movement_amount, int movement_distance);
  ~MovingSprite();

  void tick() override;
  void determine(std::shared_ptr<Context> context,
                 std::vector<std::shared_ptr<Sprite>> &sprites) override;
  void on_collision(const uint8_t &state, std::shared_ptr<Sprite> other) override;

protected:
  MovingSprite::MovingSprite(const std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                             const SDL_Rect &position, const SDL_Rect &collision_offset,
                             int movement_amount, int movement_distance);

private:
  const SDL_Rect original_position;
  Direction direction;
  int movement_amount;
  int movement_distance;
};

#endif
