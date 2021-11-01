#ifndef DATA_CONTAINER_H
#define DATA_CONTAINER_H

#include <SDL.h>
#include <algorithm>
#include <iostream>
#include <memory>

#include "CollisionUtils.h"

struct MapMetrics
{
  const int map_grid_width;
  const int map_grid_height;
  const int map_tile_width;
  const int map_tile_height;
  const int map_width;
  const int map_height;
  SDL_Rect map_dimension;

  MapMetrics(const int &map_grid_width, const int &map_grid_height, const int &map_tile_width,
             const int &map_tile_height)
      : map_grid_width(map_grid_width),
        map_grid_height(map_grid_height),
        map_tile_width(map_tile_width),
        map_tile_height(map_tile_height),
        map_width(map_grid_width * map_tile_width),
        map_height(map_grid_height * map_tile_height),
        map_dimension({0, 0, map_width, map_height})
  {
  }
};

// NOTE: this is undone and unused.
// TODO: finish it and implement in all code -- replace the use of SDL_Rect, it
// should contain a function that returns a wrapped SDL_Rect for usage in SDL
// content.
struct Rectangle
{
  int top_left;
  int top_right;
  int bottom_left;
  int bottom_right;

  void merge(const Rectangle &other)
  {
    top_left = std::min(top_left, other.top_left);
    top_right = std::max(top_right, other.top_right);
    bottom_left = std::min(bottom_left, other.bottom_left);
    bottom_right = std::max(bottom_left, other.bottom_right);
  }
};

#endif
