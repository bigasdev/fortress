#include "Game.hpp"
#include "core/ecs_commons.hpp"
#include "core/engine_commons.hpp"

#include "core/global.hpp"
#include "entity/Types.hpp"
#include "res/autogen_res.hpp"
#include "tools/Cooldown.hpp"
#include "tools/RecordSystem.hpp"

// SDL
#include "SDL.h"

#include "../entity/EntityFactory.hpp"

std::unique_ptr<RecordSystem> m_record_system =
    std::make_unique<RecordSystem>();

std::unique_ptr<SpriteSystem> m_sprite_system =
    std::make_unique<SpriteSystem>();
std::unique_ptr<PhysicsSystem> m_physics_system =
    std::make_unique<PhysicsSystem>();
std::unique_ptr<AnimatedSpriteSystem> m_animated_sprite_system =
    std::make_unique<AnimatedSpriteSystem>();
std::unique_ptr<PlayerMoveSystem> m_player_move_system =
    std::make_unique<PlayerMoveSystem>();
std::unique_ptr<SpriteComponent> orc_spr;
std::unique_ptr<WeaponSystem> m_weapon_system =
    std::make_unique<WeaponSystem>();
std::unique_ptr<BulletSystem> m_bullet_system =
    std::make_unique<BulletSystem>();
std::unique_ptr<CollisionSystem> m_collision_system =
    std::make_unique<CollisionSystem>();
std::unique_ptr<PlayerAttackSystem> m_player_attack_system =
    std::make_unique<PlayerAttackSystem>();
std::unique_ptr<HealthManagementSystem> m_health_system =
    std::make_unique<HealthManagementSystem>();

std::unique_ptr<EntityFactory> m_entity_factory =
    std::make_unique<EntityFactory>();

// flag implementation
std::unique_ptr<Flag::World> m_world;

Game::Game() {}

Game::~Game() {}

void Game::init() {
  m_camera = new Camera(g_engine->get_window_size());
  m_cooldown = new Cooldown();

  // TODO: load system start
  // REMEMBER to add this to the engine later so it can await to load
  auto save_data = g_res->load_save();

  g_cooldown = m_cooldown;
  g_camera = m_camera;

  // entity factory is where all the entities skeletons shuld be defined
  // makes the systems/games cleaner and easier to manage different types of
  // instantatiations
  g_entity_factory = m_entity_factory.get();

  // the game manager holds the world
  g_game_manager = new GameManager();

  // registering all the systems, order matters
  std::vector<Flag::ISystem *> m_systems = {};
  m_systems.push_back(m_sprite_system.get());
  m_systems.push_back(m_animated_sprite_system.get());
  m_systems.push_back(m_physics_system.get());
  m_systems.push_back(m_player_move_system.get());
  m_systems.push_back(m_player_attack_system.get());

  m_systems.push_back(m_collision_system.get());
  m_systems.push_back(m_weapon_system.get());
  m_systems.push_back(m_bullet_system.get());
  m_systems.push_back(m_health_system.get());

  m_world = std::make_unique<Flag::World>(m_systems);

  g_game_manager->main_world = m_world.get();
#if _DEBUG
  m_world->DIAGNOSTIC_MODE = true;
#endif
  m_world->start();

  // TODO: hero spawn
  g_entity_factory->spawn_hero();

  // spawning enemies
  for (int i = 2; i < 4; i++) {
    // rnd engine
    auto rnd = Random::get<int>(0, 1);
    auto entity_type =
        rnd == 0 ? res::data::entity_orc_rogue : res::data::entity_silver_vein;

    auto pos = vec2{Random::get<float>(-50, 600), Random::get<float>(-96, 600)};
    m_entity_factory->spawn_entity(entity_type, pos);
  }

  // TODO: camera positioning/setting to hero
  g_camera->track_pos(&m_world->get_component<TransformComponent>(0)->pos);
}

void Game::fixed_update(double tmod) { m_world->fixed_update(); }

void Game::update(double dt) {
  m_cooldown->update(dt);

  m_world->update();
  m_world->destroy_pending_entities();

  // debug pause + recording
  // control + d its used to get a profiler in a .txt file
#if _DEBUG
  if (g_input_manager->get_key_press(SDL_KeyCode::SDLK_TAB)) {
    if (g_game_manager->main_world->is_paused) {
      g_game_manager->main_world->resume();
    } else {
      g_game_manager->main_world->pause();
      SaveData data;
      data.hero_pos =
          m_world->get_component<TransformComponent>(PLAYER_ENTITY_ID)->pos;
      g_res->save(data);
    }
  }
  m_record_system->capture_frame(dt);
  if (g_input_manager->get_key_press(SDL_KeyCode::SDLK_s, SDLK_LCTRL)) {
    m_record_system->take_screenshot();
  }
  if (g_input_manager->get_key_press(SDL_KeyCode::SDLK_f, SDLK_LCTRL)) {
    m_record_system->start_recording();
  }
  if (g_input_manager->get_key_press(SDL_KeyCode::SDLK_d, SDLK_LCTRL)) {
    // get all the diagnostic info and write it to the report.txt file
    auto report = m_world->diagnostic.get_diagnostics();
    std::ofstream file("report.txt");
    for (auto &line : report) {
      if (file.is_open()) {
        file << line << "\n";
      }
    }
    file.close();
  }
#endif
}

void Game::post_update(double dt) {
  m_camera->move();
  m_camera->update();
}

void Game::draw_root() {
  auto tileset = g_res->get_asset_texture("tileset");

  g_renderer->draw_asset(*tileset, {-192, -96});
  g_renderer->draw_asset(*tileset, {-192, 96});
}

void Game::draw_ent() { m_world->render(); }
void Game::draw_ui() {
  if (g_res->is_saving) {
    g_renderer->draw_text({0, 0}, "Saving...",
                          g_res->get_font(res::fonts::blueterm));
  }
}

void Game::clean() {}
