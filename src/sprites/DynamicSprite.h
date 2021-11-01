#ifndef DYNAMIC_SPRITE_H
#define DYNAMIC_SPRITE_H

#include "Sprite.h"
#include <vector>

class Context;

class DynamicSprite : public Sprite
{
protected:
  DynamicSprite(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                const SDL_Rect &position, const SDL_Rect &collision_offset);

public:
  ~DynamicSprite();

  virtual void tick() = 0;
  virtual void determine(std::shared_ptr<Context> context,
                         std::vector<std::shared_ptr<Sprite>> &sprites) = 0;
};

#endif
