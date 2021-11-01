#ifndef PORTAL_SPRITE_H
#define PORTAL_SPRITE_H

#include "LogicExtension.h"
#include "StaticSprite.h"

class PortalSprite : public StaticSprite, public LogicExtension
{
private:
  Direction direction;
  int map_index; // Which map index the portal should change into.
protected:
  PortalSprite(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
               const SDL_Rect &position, const SDL_Rect &collision_offset, int grid_layer,
               int grid_row, int grid_column, const int &map_index, const Direction &direction);

public:
  ~PortalSprite();
  static std::shared_ptr<PortalSprite>
  get_instance(std::shared_ptr<Image> image, const SDL_Rect &src_rectangle,
               const SDL_Rect &position, const SDL_Rect &collision_offset, int grid_layer,
               int grid_row, int grid_column, const int &map_index, const Direction &direction);
  void do_logic(std::shared_ptr<Context> context, std::shared_ptr<Sprite> trigger) override;
};

#endif
