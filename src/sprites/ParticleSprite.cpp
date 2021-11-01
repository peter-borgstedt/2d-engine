#include "ParticleSprite.h"
#include "CollisionUtils.h"
#include "Debug.h"

using namespace std;

ParticleSprite::ParticleSprite(shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                               const SDL_Rect &position, const Direction &direction,
                               const std::shared_ptr<FrameAnimation> &animation)
    : DynamicSprite(image, src_rectangle, position, {0, 0, 0, 0}),
      direction(direction),
      animation(animation)

{
  debug::analyser.type_constructed(this);
}

ParticleSprite::~ParticleSprite()
{
  debug::analyser.type_deconstructed(this);
}

std::shared_ptr<ParticleSprite>
ParticleSprite::get_instance(shared_ptr<Image> image, const SDL_Rect &src_rectangle,
                             const SDL_Rect &position, const Direction &direction,
                             const std::shared_ptr<FrameAnimation> &animation)
{
  return shared_ptr<ParticleSprite>(
      new ParticleSprite(image, src_rectangle, position, direction, animation));
}

void ParticleSprite::tick()
{
  // TODO: set amount in constructor
  if (direction == Direction::kLEFT) {
    position.x -= 4;
  } else {
    position.x += 4;
  }
}

void ParticleSprite::determine(std::shared_ptr<Context> context,
                               std::vector<std::shared_ptr<Sprite>> &sprites)
{
  for (auto s : sprites) {
    if (s.get() == this || collision_utils::no_collision(position, s->get_position())) {
      continue;
    }

    if (collision_utils::pixel_collision(image->get_surface(), s->get_image()->get_surface(),
                                         position, s->get_position(), src_rectangle,
                                         s->get_source_rectangle())) {
      state = kOBSOLETED;
      return;
    }
  }
}

SDL_Rect &ParticleSprite::get_source_rectangle()
{
  return src_rectangle = animation->get_next_source();
}
