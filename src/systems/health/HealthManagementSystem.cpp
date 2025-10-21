#include "HealthManagementSystem.hpp"
#include "components/DropperComponent.hpp"
#include "components/HealthComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/GameManager.hpp"
#include "core/global.hpp"
#include "entity/EntityFactory.hpp"
#include "messages/DamageMessage.hpp"
#include "random.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "tools/Logger.hpp"

using Random = effolkronium::random_static;

void HealthManagementSystem::render() {

  for (const auto &[id, health_comp] :
       g_game_manager->main_world->get_all_components<HealthComponent>()) {

    if (health_comp->current_health < health_comp->max_health) {
      auto pos =
          g_game_manager->main_world->get_component<TransformComponent>(id)
              ->pos;
      auto sprite =
          g_game_manager->main_world->get_component<SpriteComponent>(id);

      if (!pos.zero()) {
        Rect rect;
        rect.x = pos.x + 8;
        rect.y = pos.y - sprite->offset.y + 12;
        rect.w = 8 * g_camera->get_game_scale();
        rect.h = 2 * g_camera->get_game_scale();

        g_renderer->draw_rect(rect, {0, 0, 0, 200}, true);

        float health_perc = static_cast<float>(health_comp->current_health) /
                            static_cast<float>(health_comp->max_health);
        rect.w = rect.w * health_perc;

        g_renderer->draw_rect(rect, {225, 0, 0, 255}, true);
      }
    }
  }
}

void HealthManagementSystem::on_message(const Flag::IMessage &message) {
  if (auto health = dynamic_cast<const DamageMessage *>(&message)) {
    auto health_comp =
        g_game_manager->main_world->get_component<HealthComponent>(health->id);
    if (health_comp) {
      health_comp->current_health -= health->amount;
      Logger::log("Entity " + std::to_string(health->id) + " took " +
                  std::to_string(health->amount) + " damage. " +
                  std::to_string(health_comp->current_health) + "/" +
                  std::to_string(health_comp->max_health) + " HP left.");
      if (health_comp->current_health <= 0) {
        health_comp->is_dead = true;
        health_comp->current_health = 0;
      }
    }
  }
}

void HealthManagementSystem::update() {
  for (const auto &[id, health_comp] :
       g_game_manager->main_world->get_all_components<HealthComponent>()) {
    if (health_comp->is_dead) {
      auto dropper =
          g_game_manager->main_world->get_component<DropperComponent>(id);
      auto transform =
          g_game_manager->main_world->get_component<TransformComponent>(id);

      if (dropper && transform) {
        for (int i = 0; i < dropper->amount; i++) {
          auto pos = vec2{
              Random::get<float>(transform->pos.x - 5, transform->pos.x + 5),
              Random::get<float>(transform->pos.y - 5, transform->pos.y + 5)};

          auto dx = Random::get<float>(-420.0f, 420.0f);
          auto dy = Random::get<float>(-420.0f, 420.0f);

          g_entity_factory->spawn_drop(dropper->item_name, pos, dx, dy);
        }
      }

      g_game_manager->main_world->destroy_entity(id);
      // FIX: improve a way to remove it from the grid
      // g_game_manager->main_world->spatial_grid.remove_entity(id);
    }
  }
}
