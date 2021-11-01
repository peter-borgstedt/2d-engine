#ifndef GAME_H
#define GAME_H

#define WINDOW_X      SDL_WINDOWPOS_CENTERED
#define WINDOW_Y      SDL_WINDOWPOS_CENTERED
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 500
#define TITLE         "A game example"

#include "Debug.h"
#include "Engine.h"
#include "GUIFactory.h"
#include "Panel.h"
#include "SpriteManager.h"
#include "TMXParser.h"

/*
 * We do not need much code to make things work. Most game data have been parsed
 * through the TMX format, for details see the TMX-parser. The game engine
 * contain all the logic, the TMX-parser just initialize some of the custom
 * sprites written. The TMX-parser is just a demonstration of the engine
 * flexibility, also it makes the game development a lot faster when using a
 * editor like Tiled instead of hard coding map content in code or a
 * configuration.
 */
class ExampleGame
{
private:
  std::shared_ptr<Context> build_context();
  void build_map_content(const std::shared_ptr<Context> context);
  void setup_key_mapping(const std::shared_ptr<Context> context);
  void build_panels(const std::shared_ptr<Context> context);

  ExampleGame(const ExampleGame &) = delete;
  const ExampleGame &operator=(const ExampleGame &) = delete;

private:
  ExampleGame()
  {
    // Do nothing
  }
  ~ExampleGame()
  {
    // Do nothing
  };

public:
  static void start();
};

#endif
