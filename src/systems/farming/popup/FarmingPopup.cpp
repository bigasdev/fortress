#include "FarmingPopup.hpp"
#include "core/GameManager.hpp"
#include "core/InputManager.hpp"
#include "core/global.hpp"
#include "entity/EntityFactory.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "res/Res.hpp"
#include "res/autogen_res.hpp"
#include "tools/Common.hpp"
#include "tools/Logger.hpp"
#include "tools/Math.hpp"
#include "tools/Mouse.hpp"

void FarmingPopup::render() {
  Rect rect;
  rect.w = 200 * g_camera->get_game_scale();
  rect.h = 200 * g_camera->get_game_scale();
  rect.x = (g_camera->get_size().x - rect.w - 50) / 2;
  rect.y = (g_camera->get_size().y - rect.h - 50) / 2;

  Col col = {50, 50, 20, 255};

  g_renderer->draw_rect(rect, col, true);

  // TODO: change this magic number to a grid size
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      Rect rect;
      rect.w = 16 * g_camera->get_game_scale();
      rect.h = 16 * g_camera->get_game_scale();
      rect.x = 50 * j;
      rect.y = 50;

      Col col = {0, 0, 0, 20};

      g_renderer->draw_rect(rect, col, true);
    }
  }
}

void FarmingPopup::update() {
  if (g_input_manager->get_key_press(SDLK_p)) {
    g_game_manager->main_world->resume();
  }
}

void FarmingPopup::popup() { Logger::log("Hello popup"); }
