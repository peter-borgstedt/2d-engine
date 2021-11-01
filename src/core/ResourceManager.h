#ifndef TILESET_MANAGER_H
#define TILESET_MANAGER_H

#define DEFAULT_FONT      "clacon.ttf";
#define DEFAULT_FONT_SIZE 24

#include <SDL.h>
#include <SDL_ttf.h>
#include <functional>
#include <memory>
#include <unordered_map>

#include "Image.h"
#include "Tileset.h"

class Context; // Forward declaration

class ResourceManager
{
private:
  std::unordered_map<std::string, TTF_Font *> fonts;
  std::unordered_map<std::string, std::shared_ptr<Image>> images;

  TTF_Font *load_font(std::string path, int size);

public:
  ResourceManager();
  ~ResourceManager();

  // Get image resource.
  std::shared_ptr<Image> get_image(std::string path);
  std::shared_ptr<Image> get_or_load_image(std::string path);

  // Get font resource
  TTF_Font *get_or_load_font(std::string font, int size);

  // Initialize resources.
  void initalize(SDL_Renderer *renderer);

  // Remove unused resources.
  void cleanse();
};

#endif
