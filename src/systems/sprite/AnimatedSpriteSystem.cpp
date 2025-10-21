#include "AnimatedSpriteSystem.hpp"

#include "components/IComponent.hpp"
#include "components/SpriteAnimatedComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/GameManager.hpp"
#include "core/Timer.hpp"
#include "core/global.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "res/Res.hpp"
#include "tools/Logger.hpp"

void AnimatedSpriteSystem::update() {

  auto sprite_components =
      g_game_manager->main_world->get_all_components<SpriteAnimatedComponent>();

  for (auto &[id, spr] : sprite_components) {

    if (!spr->is_enabled)
      continue;

    auto dt = Timer::get_dt();

    spr->tick += dt;

    if (spr->tick >= spr->tick_timer) {
      spr->tick = 0.0f;
      spr->current_anim.m_curr_frame++;
      if (spr->current_anim.m_curr_frame > spr->current_anim.m_end_frame) {
        if (spr->current_anim.loop) {
          spr->current_anim.m_curr_frame = spr->current_anim.m_start_frame;
        } else {
          spr->current_anim.m_curr_frame = spr->current_anim.m_end_frame;
          spr->finished_anim = spr->current_anim.name;
          if (spr->change_to_anim != "") {
            if (spr->anims.find(spr->change_to_anim) != spr->anims.end()) {
              spr->current_anim = spr->anims[spr->change_to_anim];
              spr->change_to_anim = "";
            } else {
              Logger::error("Animation not found: " + spr->change_to_anim);
            }
          } else {
            if (spr->default_anim.name != "") {
              spr->current_anim = spr->default_anim;
            }
          }
        }
      }
    }
  }
}

void AnimatedSpriteSystem::render() {
  auto sprite_components =
      g_game_manager->main_world->get_all_components<SpriteAnimatedComponent>();

  for (auto &[id, spr] : sprite_components) {
    auto transform =
        g_game_manager->main_world->get_component<TransformComponent>(id);

    if (!spr->is_enabled || transform == nullptr) {
      continue;
    }

    auto dir = transform->flip_x ? -1 : 1;
    auto angle = transform->rotation;
    auto pivot_x = transform->pivot_x;
    auto pivot_y = transform->pivot_y;

    g_renderer->draw_asset(
        spr->current_anim.frames[spr->current_anim.m_curr_frame].texture,
        transform->pos + spr->offset, angle, pivot_x, pivot_y, dir);
  }
}

void AnimatedSpriteSystem::pause() {}
