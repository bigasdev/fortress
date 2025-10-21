#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"

class HealthComponent : public Flag::IComponent {
public:
  ~HealthComponent() = default;

  int max_health = 100;
  int current_health = 100;
  bool is_dead = false;
};
