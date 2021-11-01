#ifndef SPRITE_BASE_H
#define SPRITE_BASE_H

#include <memory>

#include "Image.h"

class Sprite : public std::hash<std::shared_ptr<Sprite>>,
               public std::enable_shared_from_this<Sprite>
{
public:
  enum State
  {
    kIDLE,     // No post validation will be done.
    kUPDATE,   // Post validation will be done, collision e.t.c. TODO: implement
               // or remove ...
    kOBSOLETED // This will remove and deconstruct this sprite.
  };

  // Current sprite state, see Sprite::State for details.
  State state = kIDLE;

  // Iteration weight. This is used when iterate over sprites, process
  // prioritized sprites first.
  int order = 10; // Low for high priority, high for less.

  virtual ~Sprite();

  // Paint sprite graphics on screen.
  virtual void paint(SDL_Renderer *renderer, const int &offset_x = 0, const int &offset_y = 0);

  // Get sprite destination rectangle, i.e. sprite position in map.
  virtual SDL_Rect &get_position();

  // Get sprite source rectangle, i.e. sprite position in tile sheet.
  virtual SDL_Rect &get_source_rectangle();

  // Get sprite collision rectangle, i.e. a rectangle that should be considered
  // as a collision boundary for this sprite. This rectangle can be zeroed out,
  // which mean it is not a colliding sprite.
  virtual SDL_Rect get_collision_position();

  bool is_collidable();

  // Get the image of this Sprite, need to retrieve pixels when doing perfect
  // pixel collision.
  const std::shared_ptr<Image> get_image() const;

  virtual void on_collision(const uint8_t &state, std::shared_ptr<Sprite> other){};

protected:
  // Texture and Surface.
  std::shared_ptr<Image> image;

  // The source area in the image to paint from.
  SDL_Rect src_rectangle;

  // Sprite position, the destination area in the renderer to paint to.
  SDL_Rect position;

  // Collision offset, if zeroed out no collision will be made.
  SDL_Rect collision_offset;

  // Only subclasses may create instances of this class.
  Sprite(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle, const SDL_Rect &position,
         const SDL_Rect &collision_offset);

private:
  Sprite(const Sprite &) = delete;
  const Sprite &operator=(const Sprite &) = delete;
};

#endif
