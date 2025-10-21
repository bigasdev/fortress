#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"

class WeaponComponent : public Flag::IComponent {
public:
  ~WeaponComponent() = default;

  int sprite_id = -1;
  int following = -1; // entity id of the entity this weapon follows
  bool left = false;
};
