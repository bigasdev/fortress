#include "FarmingSystem.hpp"

#include "SDL_keycode.h"
#include "World.hpp"
#include "components/CollisionBoxComponent.hpp"
#include "components/CropComponent.hpp"
#include "components/IComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/GameManager.hpp"
#include "core/InputManager.hpp"
#include "core/global.hpp"
#include "entity/EntityFactory.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "res/Res.hpp"
#include "res/autogen_res.hpp"
#include "systems/farming/popup/FarmingPopup.hpp"
#include "tools/Common.hpp"
#include "tools/Logger.hpp"
#include "tools/Math.hpp"
#include "tools/Mouse.hpp"
#include <string>

void FarmingSystem::start() {
  current_crop = -1;
  seeds = g_res->get_items_from_folder("seeds");
  for (auto &seed : seeds) {
    auto type = seed.get_param<ItemParamString>("type");
    Logger::log("Found seed: " + seed.name);
    if (type) {
      if (type->value == "fruit") {
        fruits.push_back(&seed);
      } else if (type->value == "plant") {
        plants.push_back(&seed);
      } else if (type->value == "herb") {
        herbs.push_back(&seed);
      }
      Logger::log("Type: " + type->value);
    }
  }
}

void FarmingSystem::render() {
  if (!building_mode) {
    // player interaction stuff
    if (current_crop != -1) {
      auto crop_component =
          g_game_manager->main_world->get_component<CropComponent>(
              current_crop);
      auto player_pos =
          g_game_manager->main_world->get_component<TransformComponent>(
              PLAYER_ENTITY_ID);

      if (crop_component) {
        // player UI
        switch (crop_component->type) {
        case FRUIT:
          for (int i = 0; i < fruits.size(); i++) {
            auto fruit = fruits[i];
            Rect bg;
            bg.h = 64;
            bg.w = 64;
            bg.x = player_pos->pos.x;
            bg.y = player_pos->pos.y - 70 - (i * 70);
            Col color = {0, 0, 0, 150};

            auto icon = g_res->get_sprite(fruit->sprite);

            g_renderer->draw_rect(bg, color, true);
            g_renderer->draw(*g_res->get_texture(icon.sheet), icon,
                             {bg.x + 16, bg.y + 16});
          }
          break;
        case PLANT:
          for (int i = 0; i < plants.size(); i++) {
            auto plant = plants[i];
            Rect bg;
            bg.h = 64;
            bg.w = 64;
            bg.x = player_pos->pos.x;
            bg.y = player_pos->pos.y - 70 - (i * 70);
            Col color = {0, 0, 0, 150};

            auto icon = g_res->get_sprite(plant->sprite);

            g_renderer->draw_rect(bg, color, true);
            g_renderer->draw(*g_res->get_texture(icon.sheet), icon,
                             {bg.x, bg.y});
            g_renderer->draw_text({bg.x + 32, bg.y + 50}, "999",
                                  g_res->get_font(res::fonts::pixolleta),
                                  {255, 255, 255, 255}, 2);
          }
          break;
        case HERB:
          break;
        }
      }
    }

    return;
  }

  auto cells = g_game_manager->main_world->spatial_grid.get_all_entities();

  auto e = g_game_manager->main_world->spatial_grid.get_entities_in_position(
      current_cell.x, current_cell.y);

  if (e.size() > 0) {
    valid_pos = false;
  } else {
    valid_pos = true;
  }

  Col col = valid_pos ? Col(0, 255, 0, 100) : Col(255, 0, 0, 100);

  g_renderer->draw_rect(
      Rect{current_cell.x, current_cell.y,
           g_game_manager->main_world->spatial_grid.cell_size,
           g_game_manager->main_world->spatial_grid.cell_size},
      col, true);
}
void FarmingSystem::get_mouse_cell() {
  auto mouse_pos = Mouse::get_mouse_coordinates() +
                   vec2(g_camera->get_pos().x, g_camera->get_pos().y);

  int cell_size = g_game_manager->main_world->spatial_grid.cell_size;

  current_cell.x = (int)(mouse_pos.x / cell_size) * cell_size;
  current_cell.y = (int)(mouse_pos.y / cell_size) * cell_size;
}

void FarmingSystem::update() {
  if (g_input_manager->get_key_press(SDL_KeyCode::SDLK_b)) {
    building_mode = !building_mode;
  }

  // check if the player is close to some crop
  auto player_pos =
      g_game_manager->main_world->get_component<TransformComponent>(
          PLAYER_ENTITY_ID);

  if (player_pos) {

    auto crops =
        g_game_manager->main_world->get_all_components<CropComponent>();

    if (current_crop != -1) {
      if (g_input_manager->get_key_press(SDLK_e)) {
        g_game_manager->main_world->popup<FarmingPopup>();
      }
    }

    // TODO: LATER REWORK THE GRID SYSTEM TO BE ABLE TO FEED HIGHER GRID SIZES
    // TO GET

    for (auto &[entity_id, crop] : crops) {
      auto crop_transform =
          g_game_manager->main_world->get_component<TransformComponent>(
              entity_id);
      if (crop_transform) {
        float dist = Math::dist_vec(player_pos->pos, crop_transform->pos);
        if (dist > 5.0f && dist < 80.0f) {
          current_crop = entity_id;
          Logger::log("Player is near a crop!" + std::to_string(entity_id));
          continue;
        }
      }
      current_crop = -1;
    }
  }

  if (!building_mode) {
    return;
  }

  if (g_input_manager->get_key_press(SDL_KeyCode::SDLK_p) && valid_pos) {
    g_game_manager->main_world->spatial_grid.add_entity(
        g_entity_factory->spawn_crop(current_cell), current_cell.x,
        current_cell.y);
  }

  get_mouse_cell();
}
