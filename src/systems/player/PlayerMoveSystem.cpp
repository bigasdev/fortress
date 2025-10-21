#include "PlayerMoveSystem.hpp"

#include "SDL_keycode.h"
#include "components/PlayerMoveComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/GameManager.hpp"
#include "core/InputManager.hpp"
#include "core/Timer.hpp"
#include "core/global.hpp"
#include "entity/Types.hpp"
#include "tools/Logger.hpp"
#include "tools/Math.hpp"

void PlayerMoveSystem::update() {
  auto player = g_game_manager->main_world->get_component<TransformComponent>(
      PLAYER_ENTITY_ID);
  auto player_move =
      g_game_manager->main_world->get_component<PlayerMoveComponent>(
          PLAYER_ENTITY_ID);

  if (player == nullptr || player_move == nullptr)
    return;

  auto dt = Timer::get_dt();

  player->pos.x +=
      (player_move->speed * dt) * g_input_manager->get_raw_axis().x;
  player->pos.y +=
      (player_move->speed * dt) * g_input_manager->get_raw_axis().y;

  // light system, this will need a grid system to get distance between ALL
  // lights later
  // g_light_falloff = Math::prox_falloff(player->pos.x, 120, .006f);

  // start of the dash
  if (g_input_manager->get_key_press(SDLK_LSHIFT)) {

    player->pos.x += (50) * g_input_manager->get_raw_axis().x;
    player->pos.y += (50) * g_input_manager->get_raw_axis().y;
  }
}
