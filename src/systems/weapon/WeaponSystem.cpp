#include "WeaponSystem.hpp"

#include "components/IComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/WeaponComponent.hpp"
#include "core/GameManager.hpp"
#include "core/global.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "res/Res.hpp"
#include "tools/Logger.hpp"

void WeaponSystem::update() {
  auto weapon_components =
      g_game_manager->main_world->get_all_components<WeaponComponent>();
  auto transform_components =
      g_game_manager->main_world->get_all_components<TransformComponent>();

  for (auto &[id, weapon] : weapon_components) {
    auto weapon_transform =
        g_game_manager->main_world->get_component<TransformComponent>(id);
    auto follow_transform =
        g_game_manager->main_world->get_component<TransformComponent>(
            weapon->following);
    if (weapon_transform == nullptr || follow_transform == nullptr)
      continue;

    // position the weapon relative to the follow entity
    if (weapon->left) {
      // FIX: THIS MAGIC NUMBER IS TOTALLY WRONG BUT IT WORKS TO SET THE PLAYER
      // CORRECT POSITION
      weapon_transform->pos.x = (follow_transform->pos.x - 12) - 25;
      weapon_transform->pos.y = (follow_transform->pos.y - 16) + 5;
      weapon_transform->flip_x = true;
    } else {
      weapon_transform->pos.x = (follow_transform->pos.x - 12) + 20;
      weapon_transform->pos.y = (follow_transform->pos.y - 16) + 5;
      weapon_transform->flip_x = false;
    }
  }
}
