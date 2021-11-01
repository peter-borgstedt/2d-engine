#include "ExampleGame.h"

#include "CustomPanel.h"
#include "Panel.h"

using namespace std;

void ExampleGame::start()
{
  ExampleGame example_game;

  shared_ptr<Context> context = example_game.build_context();
  example_game.build_panels(context);

  // TODO: make engine a singleton.
  shared_ptr<Engine> engine = Engine::get_instance(context);
  engine->run();
}

shared_ptr<Context> ExampleGame::build_context()
{
  auto window = Window::get_instance(TITLE, WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
  auto context = Context::get_instance(window);
  build_map_content(context);
  setup_key_mapping(context);
  return context;
}

void ExampleGame::build_map_content(const shared_ptr<Context> context)
{
  // Use the TMX-format and parse content; sprite sheets, sprites, objects and
  // backgrounds e.t.c. NOTE: a map and all logic can be written manually as
  // well, but the TMXParser is more convenient when designing a adding logic to
  // a game. The TMX-parser is a bit specific to, this game though -- with its
  // specific kind of sprites.
  // TODO: relative path needs to be fixed so images can be located correctly
  // TODO: from the game root directory
  TMX::parse(context, "./maps/level01.tmx"); // Level 1.
  TMX::parse(context, "./maps/level02.tmx"); // Level 2.

  // Initialize resources.
  context->get_resource_manager()->initalize(context->get_window()->get_renderer_ptr());
}

// NOTE: all event variables will be erased when the event manager is
// deconstructed.
void ExampleGame::setup_key_mapping(const shared_ptr<Context> context)
{
  // Map ESC to exit dialog.
  context->get_event_manager()->map_key(SDLK_n,
                                        [](shared_ptr<Context> context, const SDL_Event &e) {
                                          if (e.type == SDL_KEYUP) {
                                            return;
                                          }
                                          context->get_map_manager()->change_map(1);
                                        });

  // Map ESC to exit dialog.
  context->get_event_manager()->map_key(SDLK_ESCAPE, [](shared_ptr<Context> context,
                                                        const SDL_Event &e) {
    if (e.type == SDL_KEYUP) {
      return;
    }

    auto player = context->get_map_manager()->current_map()->get_sprite_manager()->get_player();
    // TODO: do this properly. this is only a issue to the player sprite.
    player->set_movement(false); // Stop movement on player.

    auto action_exit = [](const SDL_Event &, const unordered_map<string, string> &ed) {
      // TODO: now we use the event loop in the engine, but its not that
      // obvious, maybe
      // TODO: use internal events instead for exit/cancel e.t.c.
      SDL_Event quit_event;
      quit_event.type = SDL_QUIT;

      // Send quit to event queue.
      SDL_PushEvent(&quit_event); // Exit dialog.
      SDL_PushEvent(&quit_event); // Exit program.
    };

    auto action_cancel = [](const SDL_Event &, const unordered_map<string, string> &ed) {
      // Send quit to event queue.
      SDL_Event quit_event;
      quit_event.type = SDL_QUIT;
      SDL_PushEvent(&quit_event); // Exit dialog.
    };

    auto dialog = GUIFactory::create_simple_option_dialog(
        context, 350, 200, "Do you want to exist?", "Exit", "Cancel", action_exit, action_cancel);
    GUIFactory::show_shaded_dialog(context, dialog, {25, 25, 50, 128});
  });

  // Map CTRL-F key for full screen toggle.
  context->get_event_manager()->map_key(
      SDLK_f, [](shared_ptr<Context> context, const SDL_Event &e) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
          auto action_cancel = [](const SDL_Event &e, const unordered_map<string, string> &ed) {
            // Send quit to event queue.
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event); // Exit dialog.
          };

          shared_ptr<Window> window = context->get_window();
          auto action_ok = [window, context](const SDL_Event &e,
                                             const unordered_map<string, string> &ed) {
            int in_full_screen_mode = static_cast<int>(window->toggle_full_screen());
            context->set_int_data("FULL_SCREEN_MODE", in_full_screen_mode);
            auto map = context->get_map_manager()->current_map();
            map->update_map_dimension();

            // Send quit to event queue.
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event); // Exit dialog.
          };

          auto dialog = GUIFactory::create_simple_option_dialog(
              context, 350, 200,
              window->is_full_screen() ? "Exit full screen mode?" : "Set full screen mode?", "Ok",
              "Cancel", action_ok, action_cancel);
          GUIFactory::show_shaded_dialog(context, dialog, {25, 25, 50, 128});
        }
      });

  // Map W key for input dialog.
  context->get_event_manager()->map_key(SDLK_w, [](shared_ptr<Context> context,
                                                   const SDL_Event &e) {
    if (e.type == SDL_KEYUP) {
      return;
    }

    auto action_cancel = [](const SDL_Event &e, const unordered_map<string, string> &ed) {
      SDL_Event quit_event;
      quit_event.type = SDL_QUIT;

      // Send quit to event queue.
      SDL_PushEvent(&quit_event); // Exit dialog.
    };

    auto action_ok = [context](const SDL_Event &e, const unordered_map<string, string> &ed) {
      SDL_Event quit_event;
      quit_event.type = SDL_QUIT;
      if (ed.count("input_text")) { // Se TextBox event.
        context->set_string_data("NAME", ed.at("input_text"));
      }
      // Send quit to event queue.
      SDL_PushEvent(&quit_event); // Exit dialog.
    };

    auto dialog = GUIFactory::create_simple_input_dialog(context, 350, 200, "What is your name?",
                                                         "Ok", "Cancel", action_ok, action_cancel,
                                                         context->get_string_data("NAME", ""));

    GUIFactory::show_shaded_dialog(context, dialog, {25, 25, 50, 128});
  });

  // Map LEFT key to left movement.
  context->get_event_manager()->map_key(
      SDLK_LEFT, [](shared_ptr<Context> context, const SDL_Event &e) {
        auto player = context->get_map_manager()->current_map()->get_sprite_manager()->get_player();
        if (e.type == SDL_KEYDOWN) {
          player->set_direction(Direction::kLEFT); // Move player to the left.
          player->set_movement(true);
        } else // Type is SDL_KEYUP
        {
          player->stop_if_direction_set(kLEFT); // Stop left movement on player.
        }
      });

  // Map RIGHT key to right movement.
  context->get_event_manager()->map_key(
      SDLK_RIGHT, [](shared_ptr<Context> context, const SDL_Event &e) {
        auto player = context->get_map_manager()->current_map()->get_sprite_manager()->get_player();
        if (e.type == SDL_KEYDOWN) {
          player->set_direction(Direction::kRIGHT); // Move player to the right.
          player->set_movement(true);
        } else // Type is SDL_KEYUP
        {
          player->stop_if_direction_set(kRIGHT); // Stop right movement on player.
        }
      });

  // Map UP key to jump.
  context->get_event_manager()->map_key(
      SDLK_UP, [](shared_ptr<Context> context, const SDL_Event &e) {
        auto player = context->get_map_manager()->current_map()->get_sprite_manager()->get_player();
        if (e.type == SDL_KEYDOWN) {
          // Initiate a jump in physics for player.
          player->jump();
        } else {
          // Abrupt current jump in physics on player (if jump has not already
          // is fulfilled).
          player->cancel_jump();
        }
      });

  // Map A key to shooting a bullet.
  auto timer = make_shared<Timer>();     // How often a bullet may be shoot.
  auto shoot = make_shared<bool>(false); // Boolean wrapper.
  context->get_event_manager()->map_key(
      SDLK_a, [timer, shoot](shared_ptr<Context> context, const SDL_Event &e) {
        auto player = context->get_map_manager()->current_map()->get_sprite_manager()->get_player();
        if (e.type == SDL_KEYDOWN && !*shoot) {
          if (timer->is_obselete(350)) {
            Direction direction = player->get_direction();
            const SDL_Rect &position = player->get_position();

            int x, y;
            // Ugly hard coded position.
            if (direction == kRIGHT) {
              x = position.x + 32;
              y = position.y - 8;
            } else {
              x = position.x;
              y = position.y - 8;
            }

            // Ugly hard coded stump. TODO: add some gid mapping in TMX to
            // types.
            int gid = 81;
            auto sm = context->get_map_manager()->current_map()->get_sprite_manager();
            auto sprite =
                TMX::create_particle_sprite(gid, x, y, player->get_direction(), sm->get_tilesets());
            sm->add_dynamic_sprite(sprite);
          }
          *shoot = true;
        } else if (e.type == SDL_KEYUP) {
          *shoot = false;
        }
      });
}

void ExampleGame::build_panels(const std::shared_ptr<Context> context)
{
  std::shared_ptr<Panel> my_custom_panel = CustomPanel::get_instance();
  context->get_engine_core()->add_view(dynamic_pointer_cast<Panel>(my_custom_panel));
}
