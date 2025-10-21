#pragma once
#include "systems/IRenderSystem.hpp"
#include "systems/IStartSystem.hpp"
#include "systems/IUpdateSystem.hpp"
#include "tools/Common.hpp"
#include <vector>

class Item;

class FarmingSystem : public Flag::IStartSystem,
                      public Flag::IUpdateSystem,
                      public Flag::IRenderSystem {
public:
  FarmingSystem() = default;
  ~FarmingSystem() = default;
  void start() override;
  void update() override;
  void render() override;

private:
  void get_mouse_cell();

  bool building_mode = false;
  bool valid_pos = true;
  vec2 current_cell = {0, 0};
  std::vector<Item> seeds;

  std::vector<Item *> fruits;
  std::vector<Item *> plants;
  std::vector<Item *> herbs;

  // player interaction
  int current_crop = -1;
};
