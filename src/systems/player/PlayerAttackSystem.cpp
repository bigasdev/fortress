#include "PlayerAttackSystem.hpp"

#include "components/PlayerMoveComponent.hpp"
#include "components/SpriteAnimatedComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/WeaponComponent.hpp"
#include "core/GameManager.hpp"
#include "core/InputManager.hpp"
#include "core/global.hpp"
#include "entity/EntityFactory.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "tools/Logger.hpp"
#include "tools/Math.hpp"
#include "tools/Mouse.hpp"
#include <cmath>

void PlayerAttackSystem::update() {
  auto player_transform =
      g_game_manager->main_world->get_component<TransformComponent>(
          PLAYER_ENTITY_ID);

  auto player_spr =
      g_game_manager->main_world->get_component<SpriteAnimatedComponent>(
          PLAYER_ENTITY_ID);

  auto weapon_transform =
      g_game_manager->main_world->get_component<TransformComponent>(
          WEAPON_ENTITY_ID);

  auto weapon_component =
      g_game_manager->main_world->get_component<WeaponComponent>(
          WEAPON_ENTITY_ID);

  if (player_transform == nullptr || weapon_transform == nullptr ||
      player_spr == nullptr || weapon_component == nullptr)
    return;

  m_last_mouse_pos = Mouse::get_mouse_coordinates() +
                     vec2(g_camera->get_pos().x, g_camera->get_pos().y);

  float angle =
      Math::angle(player_transform->pos + player_spr->offset, m_last_mouse_pos);
  float angle_deg = angle * (180.0 / 3.14159265);

  weapon_transform->rotation = angle_deg;

  if (g_input_manager->get_key_press(SDL_KeyCode::SDLK_SPACE)) {
    float dx = cos(angle);
    float dy = sin(angle);
    vec2 spawn_pos = {
        player_transform->pos.x + (dx),
        player_transform->pos.y + (dy * 20),
    };
    g_entity_factory->spawn_bullet(spawn_pos, dx, dy, true);
  }
}

void PlayerAttackSystem::render() {
#if _DEBUG
  g_renderer->draw_rect(Rect{m_last_mouse_pos.x, m_last_mouse_pos.y, 16, 16},
                        Col{255, 0, 0, 255});
#endif
}
