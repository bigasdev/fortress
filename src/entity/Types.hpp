#pragma once

#include <cstdint>
#include <string>
#include <vector>

using EntityID = uint32_t;
const uint32_t PLAYER_ENTITY_ID = 0;
const uint32_t WEAPON_ENTITY_ID = 1;

enum class ColLayers {
  DEFAULT = 0,
  PLAYER = 1,
  ENEMY = 2,
  PLAYER_PROJECTILE = 3,
  ENEMY_PROJECTILE = 4,
  SCENERY = 5,
  TRIGGER = 6,
  NONE = 7
};

