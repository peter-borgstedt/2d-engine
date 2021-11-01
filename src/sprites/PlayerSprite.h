#ifndef PLAYER_SPRITE_H
#define PLAYER_SPRITE_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "Common.h"
#include "DataContainer.h"
#include "Debug.h"
#include "PhysicsSprite.h"

#include "CollisionUtils.h"
#include "Timer.h"
#include "Utility.h"

class PlayerSprite : public PhysicsSprite
{
  typedef std::unordered_map<Direction, std::vector<SDL_Rect>> animation_map;

public:
  static std::shared_ptr<PlayerSprite>
  PlayerSprite::get_instance(const std::shared_ptr<Image> image, const SDL_Rect &dst_area,
                             const SDL_Rect &collision_offset, const animation_map &animation,
                             const Direction &direction, const int &movement_amount);

  ~PlayerSprite();

  void paint(SDL_Renderer *renderer, const int &offset_x = 0, const int &offset_y = 0) override;

  void tick() override;
  void on_collision(const uint8_t &state, std::shared_ptr<Sprite> other) override;

  void set_movement(bool move);
  void set_direction(const Direction &direction);
  void stop_if_direction_set(const Direction &direction);
  void jump();
  void cancel_jump();
  Direction &get_direction();

  // TODO: we want to get size of the amount of indexes and iterate through that
  // size instead of using hard coded values.
  SDL_Rect &get_source_rectangle() override;

protected:
  PlayerSprite(const std::shared_ptr<Image> image, const SDL_Rect &position,
               const SDL_Rect &collision_offset, const animation_map &animation,
               const Direction &direction, const int &movement_amount);

private:
  Direction direction;
  int movement_amount;

  Timer timer;
  int animation_index = 0;
  animation_map animation;
};

#endif
