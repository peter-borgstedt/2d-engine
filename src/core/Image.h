#ifndef IMAGE_H
#define IMAGE_H

#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include <string>

class Image
{
private:
  const std::string path;
  SDL_Texture *texture = nullptr;
  SDL_Surface *surface = nullptr;

  int width;
  int height;
  SDL_Rect boundary;

  Image(const Image &) = delete;
  const Image &operator=(const Image &) = delete;

  Image(std::string image_path);

public:
  ~Image();

  static std::shared_ptr<Image> get_instance(std::string image_path);

  void init(SDL_Renderer *renderer);
  int get_width() const;
  int get_height() const;
  const SDL_Rect &get_boundary();

  std::string get_path();

  SDL_Surface *get_surface();
  SDL_Texture *get_texture();
};

#endif
