#include "Camera.h"
#include "Debug.h"

Camera::Camera(MapMetrics map_metrics) : map_metrics(map_metrics)
{
  debug::analyser.type_constructed(this);
}

Camera::~Camera()
{
  debug::analyser.type_deconstructed(this);
}

SDL_Rect Camera::get_camera_position()
{
  return {camera_x, camera_y, camera_w, camera_h};
}
