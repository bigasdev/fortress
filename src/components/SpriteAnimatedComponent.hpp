#pragma once
#include "../renderer/Sprite.hpp"
#include "../res/Res.hpp"
#include "../tools/Common.hpp"
#include "components/IComponent.hpp"

#include <string>

class SpriteAnimatedComponent : public Flag::IComponent {
public:
  ~SpriteAnimatedComponent() = default;

  EntityID owner;
  std::map<std::string, SpriteAnim> anims;
  SpriteAnim current_anim;
  float tick_timer = 0.13f;
  float tick = 0.0f;
  SpriteAnim default_anim;
  std::string finished_anim = "";
  std::string change_to_anim = "";
  vec2 offset = {0, 0};

private:
};
