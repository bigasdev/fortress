#include "SpriteSystem.hpp"

#include "World.hpp"
#include "components/IComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/GameManager.hpp"
#include "core/global.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "res/Res.hpp"
#include "tools/Logger.hpp"

void SpriteSystem::start() {
  auto sprite_components =
      g_game_manager->main_world->get_all_components<SpriteComponent>();
  for (auto &[id, spr] : sprite_components) {
    spr->spr = g_res->get_sprite(spr->name);
    Logger::log("SpriteSystem: Loaded sprite for entity:  with name: " +
                spr->name);
  }
}

void SpriteSystem::render() {
  auto sprite_components =
      g_game_manager->main_world->get_all_components<SpriteComponent>();

  for (auto &[id, spr] : sprite_components) {
    auto transform =
        g_game_manager->main_world->get_component<TransformComponent>(id);

    if (!spr->is_enabled || !transform)
      continue;

    if (spr->spr.sheet == "") {
      spr->spr = g_res->get_sprite(spr->name);
    }

    if (!g_camera->is_on_screen(transform->pos, 32)) {
      continue;
    }

    spr->spr.dir = transform->flip_x ? -1 : 1;
    spr->spr.angle = transform->rotation;
    spr->spr.pivot_x = transform->pivot_x;
    spr->spr.pivot_y = transform->pivot_y;
    g_renderer->draw(*g_res->get_texture(spr->spr.sheet), spr->spr,
                     transform->pos + spr->offset);
  }
}

void SpriteSystem::pause() {}
