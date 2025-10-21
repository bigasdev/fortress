#include "CollisionSystem.hpp"

#include "components/CollisionBoxComponent.hpp"
#include "components/IComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/GameManager.hpp"
#include "core/global.hpp"
#include "entity/Types.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "res/Res.hpp"
#include "res/autogen_res.hpp"
#include "tools/Logger.hpp"

#include "messages/CollisionMessage.hpp"

void CollisionSystem::update() {}

void CollisionSystem::render() {
  auto collision_boxes =
      g_game_manager->main_world->get_all_components<CollisionBoxComponent>();
  for (auto &[id, box] : collision_boxes) {
    auto transform =
        g_game_manager->main_world->get_component<TransformComponent>(id);
    if (transform == nullptr || !box->is_enabled)
      continue;

    if (!g_camera->is_on_screen(transform->pos, 32)) {
      continue;
    }

    Rect rect;
    rect.x = (transform->pos.x);
    rect.y = (transform->pos.y);
    rect.w = box->size.x * g_camera->get_game_scale();
    rect.h = box->size.y * g_camera->get_game_scale();

    if (box->touching_something) {
      Col c = box->gizmo_color;
      c.r = 255;
      c.g = 0;
      c.b = 0;
      g_renderer->draw_rect(rect, c, false);
    } else {
      g_renderer->draw_rect(rect, box->gizmo_color, false);
    }
  }

  /*auto spatial_grid_all =
      g_game_manager->main_world->spatial_grid.get_all_entities();
  for (auto &[cell, entities] : spatial_grid_all) {
    for (auto &entity_id :
         entities) { // draw a small rect for each entity in the cell
      auto transform =
          g_game_manager->main_world->get_component<TransformComponent>(
              entity_id);
      if (transform == nullptr)
        continue;

      if (!g_camera->is_on_screen(transform->pos, 32)) {
        continue;
      }

      Rect rect;
      rect.x = (transform->pos.x);
      rect.y = (transform->pos.y);
      rect.w = 24 * g_camera->get_game_scale();
      rect.h = 24 * g_camera->get_game_scale();

      auto text =
          "Cell (" + std::to_string(cell / (24 * g_camera->get_game_scale())) +
          ", " + std::to_string(cell / (24 * g_camera->get_game_scale())) + ")";

      g_renderer->draw_rect(rect, {255, 255, 0, 20}, true);
      g_renderer->draw_text({rect.x, rect.y - 25}, text.c_str(),
                            g_res->get_font(res::fonts::pixolleta));
    }
}*/
#if _DEBUG
#endif
}

void CollisionSystem::on_message(const Flag::IMessage &message) {
  Logger::log("CollisionSystem received a message");

  if (auto col = dynamic_cast<const CollisionMessage *>(&message)) {
    Logger::log("Entity " + std::to_string(col->id) + " collided with entity " +
                std::to_string(col->sender_id));
  }
}
