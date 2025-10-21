#include "Sprite.hpp"
#include "../tools/Logger.hpp"
#include <utility>

SpriteAnimator::SpriteAnimator(Sprite *spr) { m_spr = spr; }

SpriteAnimator::~SpriteAnimator() {}

void SpriteAnimator::update(double dt) {}

void SpriteAnimator::register_anim() {}
