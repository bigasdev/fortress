#pragma once

#include "entity/types.hpp"
#include "tools/Common.hpp"
#include <string>

class EntityFactory {
public:
  EntityFactory();
  ~EntityFactory() = default;

  int spawn_hero(vec2 pos = {0, 0});
  int spawn_entity(std::string name, vec2 pos = {0, 0});
  void spawn_bullet(vec2 pos, double dx, double dy, bool rotates = false);
  int spawn_drop(std::string drop, vec2 pos = {0, 0}, float dx = 0,
                 float dy = 0);
};
