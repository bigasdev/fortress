#include "BulletSystem.hpp"

#include "components/BulletComponent.hpp"
#include "components/CollisionBoxComponent.hpp"
#include "components/IComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/GameManager.hpp"
#include "core/Timer.hpp"
#include "core/global.hpp"
#include "entity/Types.hpp"
#include "messages/CollisionMessage.hpp"
#include "messages/DamageMessage.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "res/Res.hpp"
#include "tools/Logger.hpp"
#include "tools/Math.hpp"

void BulletSystem::update() {
  auto bullets =
      g_game_manager->main_world->get_all_components<BulletComponent>();

  for (auto &[id, bullet] : bullets) {
    if (!bullet->is_enabled)
      continue;

    auto transform =
        g_game_manager->main_world->get_component<TransformComponent>(id);
    if (!transform) {
      continue;
    }

    auto dt = Timer::get_dt();

    transform->pos.x += static_cast<float>(bullet->dx * bullet->speed * dt);
    transform->pos.y += static_cast<float>(bullet->dy * bullet->speed * dt);
    bullet->alive_for += dt;

    if (bullet->alive_for >= bullet->life_time) {
      g_game_manager->main_world->destroy_entity(id);
    }

    if (bullet->rotates) {
      transform->rotation += bullet->rotation_speed * static_cast<float>(dt);
      if (transform->rotation >= 360.0f) {
        transform->rotation -= 360.0f;
      }
    }

    auto colliders =
        g_game_manager->main_world->get_all_components<CollisionBoxComponent>();

    for (auto &[col_id, collider] : colliders) {
      if (col_id == PLAYER_ENTITY_ID)
        continue;
      if (col_id == id)
        continue;
      if (!collider->is_enabled)
        continue;

      auto col_transform =
          g_game_manager->main_world->get_component<TransformComponent>(col_id);
      auto bullet_collider =
          g_game_manager->main_world->get_component<CollisionBoxComponent>(id);

      if (col_transform == nullptr || bullet_collider == nullptr) {
        continue;
      }

      Rect bullet_rect;
      bullet_rect.x = static_cast<int>(transform->pos.x) +
                      static_cast<int>(bullet_collider->offset.x);
      bullet_rect.y = static_cast<int>(transform->pos.y) +
                      static_cast<int>(bullet_collider->offset.y);
      bullet_rect.w = static_cast<int>(bullet_collider->size.x) *
                      g_camera->get_game_scale();
      bullet_rect.h = static_cast<int>(bullet_collider->size.y) *
                      g_camera->get_game_scale();

      Rect ent_rect;
      ent_rect.x = static_cast<int>(col_transform->pos.x) +
                   static_cast<int>(collider->offset.x);
      ent_rect.y = static_cast<int>(col_transform->pos.y) +
                   static_cast<int>(collider->offset.y);
      ent_rect.w =
          static_cast<int>(collider->size.x) * g_camera->get_game_scale();
      ent_rect.h =
          static_cast<int>(collider->size.y) * g_camera->get_game_scale();

      if (!bullet_rect.intersects(ent_rect)) {
        continue;
      }

      CollisionMessage msg;
      DamageMessage dmg_msg;
      msg.dir = 0;
      msg.id = col_id;
      msg.sender_id = id;
      dmg_msg.amount = 10;
      dmg_msg.id = col_id;
      dmg_msg.sender_id = id;
      g_game_manager->main_world->notify_watchers(&dmg_msg);
      g_game_manager->main_world->notify_watchers(&msg);
      // g_game_manager->main_world->destroy_entity(id);
      break;
    }

    /* testdebug for spational grids
    auto e = g_game_manager->main_world->spatial_grid.get_entities_in_position(
        Math::round(transform->pos.x), Math::round(transform->pos.y));

    Logger::log("Checking for collision on : " +
                std::to_string(Math::round(transform->pos.x) /
                               (24 * g_camera->get_game_scale())) +
                ", " +
                std::to_string(Math::round(transform->pos.y) /
                               (24 * g_camera->get_game_scale())) +
                " found " + std::to_string(e.size()) + " entities.");

    for (auto &ent : e) {
      if (ent == id)
        continue;

      m_curr_spatial_entities.push_back(ent);
      Logger::log("BulletSystem: spatial grid collision test with entity " +
                  std::to_string(ent));

      auto ent_collider =
          g_game_manager->main_world->get_component<CollisionBoxComponent>(ent);
      auto ent_transform =
          g_game_manager->main_world->get_component<TransformComponent>(ent);
      auto bullet_collider =
          g_game_manager->main_world->get_component<CollisionBoxComponent>(id);

      if (ent_collider && ent_collider->is_enabled && bullet_collider &&
          bullet_collider->is_enabled) {

        if (ent_transform == nullptr || transform == nullptr) {
          continue;
        }
      }
    }
  m_curr_spatial_entities.clear();*/
  }
}

void BulletSystem::render() {
  for (auto id : m_curr_spatial_entities) {
    auto transform =
        g_game_manager->main_world->get_component<TransformComponent>(id);
    if (!transform) {
      continue;
    }

    Rect rect;
    rect.x = static_cast<int>(transform->pos.x);
    rect.y = static_cast<int>(transform->pos.y);
    rect.w = 4 * g_camera->get_game_scale();
    rect.h = 4 * g_camera->get_game_scale();
    Col col = {255, 0, 0, 100};

    g_renderer->draw_rect(rect, col);
  }
}
