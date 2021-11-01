#ifndef PARTICLE_SPRITE_H
#define PARTICLE_SPRITE_H

#include "Common.h"
#include "DynamicSprite.h"
#include "FrameAnimation.h"

class ParticleSprite : public DynamicSprite
{
public:
  ~ParticleSprite();

  static std::shared_ptr<ParticleSprite>
  ParticleSprite::get_instance(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                               const SDL_Rect &position, const Direction &direction,
                               const std::shared_ptr<FrameAnimation> &animation);

  SDL_Rect &get_source_rectangle() override;

  void tick() override;
  void determine(std::shared_ptr<Context> context,
                 std::vector<std::shared_ptr<Sprite>> &sprites) override;

protected:
  ParticleSprite(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                 const SDL_Rect &position, const Direction &direction,
                 const std::shared_ptr<FrameAnimation> &animation);

private:
  Direction direction;
  const std::shared_ptr<FrameAnimation> animation;
};

#endif
