#include "Image.h"
#include "Debug.h"

using namespace std;

Image::Image(string path) : path(path), width(-1), height(-1)
{
  debug::analyser.type_constructed(this);
}

Image::~Image()
{
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
  debug::analyser.type_deconstructed(this);
}

shared_ptr<Image> Image::get_instance(std::string image_path)
{
  return shared_ptr<Image>(new Image(image_path));
}

void Image::init(SDL_Renderer *renderer)
{
  if (surface && texture) {
    return; // Already initialized.
  }
  if (!renderer) {
    debug::log("Image: error wile initializing image " + path);
    debug::runtime_error("Image: no renderer found");
  }

  // Keep surface in primary memory, we want quick access to pixel data.
  surface = IMG_Load(path.c_str());
  if (!surface) {
    debug::log("Image: error wile initializing image " + path);
    debug::runtime_error(string("IMG_Load:").append(SDL_GetError()));
  }

  // Maybe use a SDL_TEXTUREACCESS_STREAMING Texture instead, then we have better access
  // to pixels. However keeping the pixels (SDL_Surface) in the primary memory like now is
  // probably a lot faster.
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    debug::log("Image: error wile initializing image " + path);
    debug::runtime_error(string("SDL_CreateTextureFromSurface: ").append(SDL_GetError()));
  }

  width = surface->w;
  height = surface->h;
  boundary = {0, 0, width, height};
}

int Image::get_width() const
{
  return width;
}

int Image::get_height() const
{
  return height;
}

const SDL_Rect &Image::get_boundary()
{
  return boundary;
}

std::string Image::get_path()
{
  return path;
}

SDL_Surface *Image::get_surface()
{
  return surface;
}

SDL_Texture *Image::get_texture()
{
  return texture;
}
