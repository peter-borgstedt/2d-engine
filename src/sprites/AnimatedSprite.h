#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include "FrameAnimation.h"
#include "StaticSprite.h"

class AnimatedSprite : public StaticSprite
{
public:
  int animated_index = 0;
  struct AnimationFrame
  {
    SDL_Rect rectangle;
    int duration;
  };

  static std::shared_ptr<AnimatedSprite>
  get_instance(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
               const SDL_Rect &position, const SDL_Rect &collision_offset, int grid_layer,
               int grid_row, int grid_column, const std::shared_ptr<FrameAnimation> &animation);

  ~AnimatedSprite();

  SDL_Rect &get_source_rectangle() override;

protected:
  AnimatedSprite(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                 const SDL_Rect &position, const SDL_Rect &collision_offset, int grid_layer,
                 int grid_row, int grid_column, const std::shared_ptr<FrameAnimation> &animation);

private:
  const std::shared_ptr<FrameAnimation> animation;
};

#endif
