#ifndef TMX_PARSER_H
#define TMX_PARSER_H

#include <memory>
#include <string>

#include "Context.h"

namespace TMX
{
  void parse(std::shared_ptr<Context> window, std::string file_path);
  std::shared_ptr<ParticleSprite>
  create_particle_sprite(int gid, int x, int y, const Direction &direction,
                         std::vector<std::shared_ptr<Tileset>> &tilesets);
} // namespace TMX

#endif
