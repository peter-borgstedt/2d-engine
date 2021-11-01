#ifndef PHYSICS_SPRITE_H
#define PHYSICS_SPRITE_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "CollisionUtils.h"
#include "Common.h"
#include "Debug.h"
#include "DynamicSprite.h"
#include "Physics.h"
#include "Utility.h"

#include "Timer.h"

class PhysicsSprite : public DynamicSprite
{
protected:
  int previous_x = -1;
  int previous_y = -1;
  std::shared_ptr<physics::Physics> physics;

  bool is_moving = false;
  bool in_air = false;

  PhysicsSprite(const std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                const SDL_Rect &position, const SDL_Rect &collision_offset);

public:
  ~PhysicsSprite();
  static std::shared_ptr<PhysicsSprite>
  PhysicsSprite::get_instance(const std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                              const SDL_Rect &position, const SDL_Rect &collision_offset);

  void tick() override;

  void determine(std::shared_ptr<Context> context,
                 std::vector<std::shared_ptr<Sprite>> &sprites) override;

  void adjust_x_axis(const uint8_t &h_state, SDL_Rect &a, SDL_Rect &b);
  void adjust_y_axis(const uint8_t &v_state, SDL_Rect &a, SDL_Rect &b);

  virtual void on_x_collisions(const uint8_t &h_state, const std::shared_ptr<Sprite> other,
                               int &x_adjustment);
  virtual void on_y_collision(const uint8_t &v_state, const std::shared_ptr<Sprite> other);

  void on_collision(const uint8_t &state, std::shared_ptr<Sprite> other) override;
};

#endif
