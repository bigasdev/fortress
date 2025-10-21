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
std::unique_ptr<FarmingSystem> m_farming_system =
    std::make_unique<FarmingSystem>();
std::unique_ptr<FarmingPopup> m_farming_popup =
    std::make_unique<FarmingPopup>();
std::unique_ptr<HealthManagementSystem> m_health_system =
    std::make_unique<HealthManagementSystem>();

// Test spawning stuff
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
  g_entity_factory = m_entity_factory.get();

  // FIX:: ECS TEST
  g_game_manager = new GameManager();

  std::vector<Flag::ISystem *> m_systems = {};
  m_systems.push_back(m_farming_system.get());
  m_systems.push_back(m_farming_popup.get());
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
  // FIX: just disabling the farming popup for now,
  //  change World later to disable it automatically on start
  m_world->disable_system(1);

  g_game_manager->main_world = m_world.get();
#if _DEBUG
  m_world->DIAGNOSTIC_MODE = true;
#endif
  m_world->start();

  // starting partitional grid
  // TODO: later change to a separated system
  m_world->spatial_grid.reset(50, 24 * g_camera->get_game_scale());

  auto player_components = std::vector<std::shared_ptr<Flag::IComponent>>{};

  auto hero_transform = std::make_shared<TransformComponent>();
  hero_transform->pos = {100, 100};
  if (save_data) {
    hero_transform->pos = save_data->hero_pos;
  }
  hero_transform->scale = vec2{1, 1};

  auto hero_collider = std::make_shared<CollisionBoxComponent>();
  hero_collider->layer = ColLayers::PLAYER;
  hero_collider->size = vec2{8, 10};
  hero_collider->gizmo_color = {0, 255, 0, 100};

  auto hero_player_move = std::make_shared<PlayerMoveComponent>();
  hero_player_move->speed = 100.0f;

  auto hero_anim_spr = std::make_shared<SpriteAnimatedComponent>();
  auto hero_idle = g_res->get_animation("Bigas_Idle");
  hero_anim_spr->offset = vec2{-12, -16};
  hero_anim_spr->anims.insert(std::make_pair("idle", hero_idle));
  hero_anim_spr->default_anim = hero_idle;
  hero_anim_spr->current_anim = hero_idle;

  player_components.push_back(hero_transform);
  player_components.push_back(hero_collider);
  player_components.push_back(hero_player_move);
  player_components.push_back(hero_anim_spr);

  auto player = new Flag::Entity(m_world.get(), player_components);

  auto id = m_world->create_entity(player);
  m_world->add_component(id, hero_transform);
  m_world->add_component(id, hero_collider);
  m_world->add_component(id, hero_player_move);
  m_world->add_component(id, hero_anim_spr);

  auto weapon_data = g_res->get_item(res::data::items_wooden_dagger);
  auto sword_components = std::vector<std::shared_ptr<Flag::IComponent>>{};
  auto sword_transform = std::make_shared<TransformComponent>();
  sword_transform->pos = vec2{150, 100};
  sword_transform->scale = vec2{1, 1};
  sword_transform->pivot_x = 0;
  sword_transform->pivot_y = 13;
  auto sword_spr = std::make_shared<SpriteComponent>();
  sword_spr->name = weapon_data.sprite;
  auto sword_component = std::make_shared<WeaponComponent>();
  sword_component->following = id;

  sword_components.push_back(sword_transform);
  sword_components.push_back(sword_spr);
  sword_components.push_back(sword_component);
  auto sword = new Flag::Entity(m_world.get(), sword_components);

  auto sword_id = m_world->create_entity(sword);
  m_world->add_component(sword_id, sword_transform);
  m_world->add_component(sword_id, sword_spr);
  m_world->add_component(sword_id, sword_component);

  // spawning enemies
  for (int i = 2; i < 2000; i++) {
    auto rnd = Random::get<int>(0, 1);
    auto entity_type =
        rnd == 0 ? res::data::entity_orc_rogue : res::data::entity_silver_vein;

    auto pos = vec2{Random::get<float>(-50, 600), Random::get<float>(-96, 600)};
    m_entity_factory->spawn_entity(entity_type, pos);

    auto orc_collider = std::make_shared<CollisionBoxComponent>();
    orc_collider->layer = ColLayers::ENEMY;
    orc_collider->size = vec2{19, 16};
    orc_collider->gizmo_color = {55, 0, 255, 100};
    // orc->add_component(orc_collider, 15);

    // TODO: remember all of this logic for spatial grid will need to be
    //  reworked + changed to a different spot/system later
    auto grid_pos = pos;

    g_game_manager->main_world->add_component(i, orc_collider);
    g_game_manager->main_world->spatial_grid.add_entity(
        i, Math::round(grid_pos.x), Math::round(grid_pos.y));

    auto orc_test = g_game_manager->main_world->get_entity(i);
  }
  g_camera->track_pos(&m_world->get_component<TransformComponent>(0)->pos);
}

void Game::fixed_update(double tmod) { m_world->fixed_update(); }

void Game::update(double dt) {
  m_cooldown->update(dt);

  m_world->update();
  m_world->destroy_pending_entities();

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
