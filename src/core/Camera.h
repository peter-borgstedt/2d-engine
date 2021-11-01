#ifndef CAMERA_H
#define CAMERA_H

#include "DataContainer.h"
#include <SDL.h>

class Camera
{
private:
  int camera_y, camera_x, camera_w, camera_h;
  const MapMetrics map_metrics;

public:
  struct Scope
  {
    int x_min, x_max, y_min, y_max;
  };

  Camera(MapMetrics map_metrics);
  ~Camera();

  SDL_Rect get_camera_position();

  inline Scope Camera::get_scope(const SDL_Rect &position)
  {
    // If in positioned perfected in one grid x + width will generate an extra grid,
    // adding -1 will not generate one, if position is offset 1px then the extra grid is taken i
    // consideration.
    int position_left = position.x, position_right = position.x + position.w - 1;
    int position_top = position.y, position_bottom = position.y + position.h - 1;

    int x_min = position_left / map_metrics.map_tile_width,
        x_max = position_right / map_metrics.map_tile_width;
    int y_min = position_top / map_metrics.map_tile_height,
        y_max = position_bottom / map_metrics.map_tile_height;
    return {x_min, x_max, y_min, y_max};
  }

  inline Scope Camera::focus_camera(const SDL_Rect &position, const SDL_Rect &view_bounds)
  {
    // Center the camera over player.
    camera_x = position.x - ((view_bounds.w - position.w) / 2);
    camera_y = position.y - ((view_bounds.h - position.h) / 2);

    // Keep the camera oriented in the view bound.
    int map_x_diff = map_metrics.map_width - view_bounds.w;
    int map_y_diff = map_metrics.map_height - view_bounds.h;

    if (camera_x > map_x_diff) {
      camera_x = map_x_diff;
    }
    if (camera_y > map_y_diff) {
      camera_y = map_y_diff;
    }
    if (camera_x < 0) {
      camera_x = 0;
    }
    if (camera_y < 0) {
      camera_y = 0;
    }

    camera_w = view_bounds.w;
    camera_h = view_bounds.h;

    int camera_left = camera_x;
    int camera_right = camera_x + camera_w - 1;
    int camera_top = camera_y;
    int camera_bottom = camera_y + camera_h - 1;

    Scope scope;
    scope.x_min = camera_left / map_metrics.map_tile_width;
    scope.x_max = camera_right / map_metrics.map_tile_width;
    scope.y_min = camera_top / map_metrics.map_tile_height;
    scope.y_max = camera_bottom / map_metrics.map_tile_height;
    return scope;
  }
};

#endif
