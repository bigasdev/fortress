#include "PhysicsSystem.hpp"
#include "components/BodyComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/GameManager.hpp"
#include "core/Timer.hpp"
#include "core/global.hpp"
#include "tools/Common.hpp"
#include "tools/Logger.hpp"
#include "tools/Math.hpp"
#include <cstdlib>

void PhysicsSystem::update() {
  auto body = g_game_manager->main_world->get_all_components<BodyComponent>();

  for (auto &[id, body] : body) {
    auto transform =
        g_game_manager->main_world->get_component<TransformComponent>(id);

    if (!body->is_enabled || !transform)
      continue;

    body->dx = Math::lerp(body->dx, 0.0f, body->weight * Timer::get_dt());
    body->dy = Math::lerp(body->dy, 0.0f, body->weight * Timer::get_dt());

    if (std::abs(body->dx) < 0.01f)
      body->dx = 0;
    if (std::abs(body->dy) < 0.01f)
      body->dy = 0;

    transform->pos.x += body->dx * Timer::get_dt();
    transform->pos.y += body->dy * Timer::get_dt();
  }
}

void PhysicsSystem::fixed_update() {}
