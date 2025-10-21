#pragma once
#include "../renderer/Sprite.hpp"
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"

#include <string>

class SpriteComponent : public Flag::IComponent {
public:
  ~SpriteComponent() = default;

  std::string name = "bigas";
  vec2 offset = {0, 0};
  [[NotSerializable]] Sprite spr;

private:
};
