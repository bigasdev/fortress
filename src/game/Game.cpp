#include "Game.hpp"
#include "../core/Engine.hpp"
#include "../core/InputManager.hpp"
#include "../core/Timer.hpp"
#include "../core/global.hpp"
#include "../renderer/Camera.hpp"
#include "../renderer/Renderer.hpp"
#include "../renderer/Sprite.hpp"
#include "../res/Res.hpp"
#include "../tools/Cooldown.hpp"
#include "../tools/Logger.hpp"
#include "../tools/Math.hpp"
#include "SDL.h"
#include <algorithm>
#include <memory>
#include <string>

#include "../entity/Entity.hpp"

// FIX:: start of the ecs, this will be better created in the future
#include "../components/IComponent.hpp"
#include "../components/PlayerMoveComponent.hpp"
#include "../components/SpriteComponent.hpp"
#include "../components/TransformComponent.hpp"
#include "../core/GameManager.hpp"
#include "../entity/GameAsset.hpp"
#include "../systems/PlayerMoveSystem.hpp"
#include "../systems/SpriteSystem.hpp"

std::unique_ptr<GameAsset> g_game_asset = std::make_unique<GameAsset>();
std::unique_ptr<SpriteSystem> m_sprite_system =
    std::make_unique<SpriteSystem>();
std::unique_ptr<PlayerMoveSystem> m_player_move_system =
    std::make_unique<PlayerMoveSystem>();
std::unique_ptr<SpriteComponent> orc_spr;

bool moving_right = false;
bool moving_left = false;
bool slow_mo = false;

float timer = 0;

Game::Game() {}

Game::~Game() {}

void Game::init() {
  m_camera = new Camera(g_engine->get_window_size());
  m_cooldown = new Cooldown();

  g_cooldown = m_cooldown;
  g_camera = m_camera;

  g_input_manager->bind_keyboard(SDLK_e, &moving_right);
  g_input_manager->bind_keyboard(SDLK_q, &moving_left);
  g_input_manager->bind_keyboard(SDLK_SPACE, &slow_mo);

  // FIX:: ECS TEST
  g_game_manager = new GameManager();

  EntityID hero_entity_id = g_game_manager->create_entity();
  auto hero_transform = std::make_shared<TransformComponent>();
  hero_transform->pos = vec2{100, 100};
  hero_transform->scale = vec2{1, 1};
  g_game_manager->add_component<TransformComponent>(hero_entity_id,
                                                    *hero_transform);
  auto hero_spr = std::make_shared<SpriteComponent>();
  hero_spr->name = "sword";
  g_game_manager->add_dynamic_component(hero_entity_id, hero_spr);
  auto hero_player_move = std::make_shared<PlayerMoveComponent>();
  hero_player_move->speed = 100.0f;
  g_game_manager->add_dynamic_component(hero_entity_id, hero_player_move);
  // limit test
  /*auto bigas = g_res->get_prefab("bigas");
  for (int i = 0; i < 10000; i++) {
    auto entity = g_game_manager->create_entity();

    auto spr_component = bigas.get_component("SpriteComponent");
    if(!spr_component){
      Logger::error("SpriteComponent not found in prefab!");
      continue;
    }else{
      auto spr = g_game_manager->component_factory->create(spr_component->name);
      for(auto& variable : spr_component->variables){
        spr->set_variable(variable.name, variable.type, variable.val);
      }
      g_game_manager->add_dynamic_component(entity, spr);
    }

    auto transform_component = std::make_shared<TransformComponent>();
    transform_component->pos = vec2{static_cast<float>(i * 26),
  static_cast<float>(80)}; transform_component->scale = vec2{1, 1};

    g_game_manager->add_component<TransformComponent>(i + 1,
  *transform_component);
  }*/

  g_game_asset->init();
  g_game_manager->add_system(std::move(m_sprite_system));
  g_game_manager->add_system(std::move(m_player_move_system));

  g_game_manager->init();
  g_game_manager->start();
}

void Game::fixed_update(double tmod) { g_game_manager->fixed_update(tmod); }

void Game::update(double dt) {
  m_cooldown->update(dt);

  g_game_manager->update(dt);

  if (slow_mo) {
    Timer::apply_slow_mo(.1f * dt);
  }
}

void Game::post_update(double dt) {
  m_camera->move();
  m_camera->update();
}

void Game::draw_root() {}

void Game::draw_ent() { g_game_manager->render(); }
void Game::draw_ui() {}

void Game::clean() {}
