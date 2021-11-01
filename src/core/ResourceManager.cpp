#include "ResourceManager.h"
#include "Debug.h"

using namespace std;

TTF_Font *ResourceManager::load_font(string font_path, int size)
{
  TTF_Font *font = TTF_OpenFont(font_path.c_str(), size);
  if (font == nullptr) {
    debug::runtime_error("TTF_OpenFont: could not open font" + font_path);
  }
  return font;
}

ResourceManager::ResourceManager()
{
  debug::analyser.type_constructed(this);
}

ResourceManager::~ResourceManager()
{
  for (auto kv : fonts) {
    TTF_CloseFont(kv.second);
    kv.second = nullptr;
  }
  debug::analyser.type_deconstructed(this);
}

shared_ptr<Image> ResourceManager::get_image(string path)
{
  return images.count(path) ? images.at(path) : nullptr;
}

shared_ptr<Image> ResourceManager::get_or_load_image(string path)
{
  if (images.count(path)) {
    return images.at(path);
  } else {
    auto image = Image::get_instance(path);
    images[image->get_path()] = image;
    return image;
  }
}

TTF_Font *ResourceManager::get_or_load_font(string font_path, int size)
{
  string key = font_path + to_string(size);
  if (fonts.count(key)) {
    return fonts.at(key);
  }
  return fonts[key] = load_font(font_path, size);
}

void ResourceManager::initalize(SDL_Renderer *renderer)
{
  for (auto kv : images)
    kv.second->init(renderer);
}

void ResourceManager::cleanse()
{
  for (auto it = images.cbegin(); it != images.cend(); ++it)
    if ((*it).second.unique()) {
      it = images.erase(it);
    }
}
