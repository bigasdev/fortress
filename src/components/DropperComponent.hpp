#pragma once
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"
#include <string>

class DropperComponent : public Flag::IComponent {
public:
  ~DropperComponent() = default;

  int amount = 1;
  std::string item_name = "coin";
};
