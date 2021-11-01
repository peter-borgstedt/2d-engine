#ifndef ITEM_SPRITE_H
#define ITEM_SPRITE_H

#include "AnimatedSprite.h"
#include "LogicExtension.h"

class ItemSprite : public AnimatedSprite, public LogicExtension
{
public:
  ~ItemSprite();
  static std::shared_ptr<ItemSprite>
  get_instance(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
               const SDL_Rect &position, const SDL_Rect &collision_offset, int grid_layer,
               int grid_row, int grid_column, const std::shared_ptr<FrameAnimation> &animation);

  void do_logic(std::shared_ptr<Context> context, std::shared_ptr<Sprite> trigger) override;

  ItemSprite(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle, const SDL_Rect &position,
             const SDL_Rect &collision_offset, int grid_layer, int grid_row, int grid_column,
             const std::shared_ptr<FrameAnimation> &animation);
};

#endif
