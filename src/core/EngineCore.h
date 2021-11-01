#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include <memory>
#include <vector>

class Context;

#include "Map.h"
#include "Panel.h"

class EngineCore
{
public:
  static std::shared_ptr<EngineCore> get_instance();
  ~EngineCore();

  void paint(std::shared_ptr<Context> context);
  void paint_copy(SDL_Texture *texture, std::shared_ptr<Context> context);
  void add_view(const std::shared_ptr<Panel> &view);

private:
  EngineCore();
  std::vector<std::shared_ptr<Panel>> views;

  EngineCore(const EngineCore &) = delete;
  const EngineCore &operator=(const EngineCore &) = delete;
};

#endif
