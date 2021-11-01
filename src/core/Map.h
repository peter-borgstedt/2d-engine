#ifndef MAP_H
#define MAP_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Camera.h"
#include "Common.h"
#include "DataContainer.h"
#include "Image.h"
#include "Window.h"

#include "PlayerSprite.h"
#include "SpriteManager.h"

class Context;

class Map
{
private:
  SDL_Rect map_dimension;
  std::shared_ptr<Window> window;
  MapMetrics map_metrics;
  Camera camera;

  std::shared_ptr<SpriteManager> sprite_manager;
  std::vector<std::shared_ptr<Sprite>> retriever;

  Map(const Map &) = delete;
  const Map &operator=(const Map &) = delete;

protected:
  Map(const SDL_Rect &map_view_dimension, std::shared_ptr<Window> window,
      const MapMetrics &metrics);

public:
  static std::shared_ptr<Map> get_instance(const SDL_Rect &map_dimension,
                                           std::shared_ptr<Window> window,
                                           const MapMetrics &map_metrics);

  ~Map();

  std::shared_ptr<SpriteManager> get_sprite_manager();

  void tick(std::shared_ptr<Context> context);

  void paint(SDL_Renderer *renderer);

  const SDL_Rect &get_map_dimension();

  const MapMetrics &get_map_metrics() const;

  const void update_map_dimension();
};

#endif
