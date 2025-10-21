#include "EntityFactory.hpp"
#include "../components/TransformComponent.hpp"
#include "../core/GameManager.hpp"
#include "../core/global.hpp"
#include "Types.hpp"
#include "World.hpp"
#include "components/BodyComponent.hpp"
#include "components/BulletComponent.hpp"
#include "components/CollisionBoxComponent.hpp"
#include "components/CropComponent.hpp"
#include "components/DropperComponent.hpp"
#include "components/HealthComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "res/Res.hpp"
#include "res/autogen_res.hpp"
#include "tools/Logger.hpp"

EntityFactory::EntityFactory() {}

int EntityFactory::spawn_entity(std::string name, vec2 pos) {
  auto data = g_res->get_item(name);
  if (data.name.empty()) {
    Logger::error("EntityFactory failed to spawn entity: " + name);
    return -1;
  }

  auto components = std::vector<std::shared_ptr<Flag::IComponent>>{};
  auto transform = std::make_shared<TransformComponent>();
  auto sprite = std::make_shared<SpriteComponent>();
  auto health = std::make_shared<HealthComponent>();

  auto drop = data.get_param<ItemParamString>("drop");
  auto dropper = std::make_shared<DropperComponent>();
  if (drop != nullptr) {
    dropper->item_name = drop->value;
    dropper->amount = data.get_param<ItemParamInt>("drop_amount")->value;
    components.push_back(dropper);
  }

  if (pos.zero()) {
    transform->pos = {data.get_param<ItemParamFloat>("pos_x")->value,
                      data.get_param<ItemParamFloat>("pos_y")->value};
  } else {
    transform->pos = pos;
  }

  health->max_health = data.get_param<ItemParamInt>("health")->value;
  health->current_health = health->max_health;
  sprite->name = data.sprite;
  sprite->offset =
      vec2{static_cast<float>(data.get_param<ItemParamInt>("offset_x")->value),
           static_cast<float>(data.get_param<ItemParamInt>("offset_y")->value)};

  components.push_back(transform);
  components.push_back(sprite);

  auto entity = new Flag::Entity(g_game_manager->main_world, components);
  entity->is_destroyed = false;
  auto entity_id = g_game_manager->main_world->create_entity(entity);

  g_game_manager->main_world->add_component(entity_id, transform);
  g_game_manager->main_world->add_component(entity_id, health);
  g_game_manager->main_world->add_component(entity_id, sprite);
  if (drop != nullptr)
    g_game_manager->main_world->add_component(entity_id, dropper);
  return entity_id;
}

int EntityFactory::spawn_crop(vec2 pos) {
  auto components = std::vector<std::shared_ptr<Flag::IComponent>>{};
  auto transform = std::make_shared<TransformComponent>();
  auto sprite = std::make_shared<SpriteComponent>();
  auto crop = std::make_shared<CropComponent>();

  transform->pos = pos;
  sprite->name = "crop";

  components.push_back(transform);
  components.push_back(sprite);
  components.push_back(crop);

  auto entity = new Flag::Entity(g_game_manager->main_world, components);
  entity->is_destroyed = false;
  auto entity_id = g_game_manager->main_world->create_entity(entity);

  g_game_manager->main_world->add_component(entity_id, transform);
  g_game_manager->main_world->add_component(entity_id, sprite);
  g_game_manager->main_world->add_component(entity_id, crop);
  return entity_id;
}

void EntityFactory::spawn_bullet(vec2 pos, double dx, double dy, bool rotates) {
  auto bullet_data = g_res->get_item(res::data::bullets_axe);
  if (bullet_data.name.empty()) {
    return;
  }

  auto component_list = std::vector<std::shared_ptr<Flag::IComponent>>{};
  auto transform = std::make_shared<TransformComponent>();
  auto sprite = std::make_shared<SpriteComponent>();
  auto bullet = std::make_shared<BulletComponent>();
  auto collider = std::make_shared<CollisionBoxComponent>();

  transform->pos = pos;
  transform->pivot_x = 8;
  transform->pivot_y = 8;
  sprite->name = bullet_data.sprite;
  sprite->offset = vec2{-8, -8};
  bullet->speed = bullet_data.get_param<ItemParamFloat>("speed")->value;
  bullet->rotation_speed = 780;
  bullet->dx = dx;
  bullet->dy = dy;
  bullet->life_time = 2.0f;
  bullet->rotates = rotates;
  collider->layer = ColLayers::PLAYER_PROJECTILE;
  collider->size = vec2{8, 8};
  collider->gizmo_color = {0, 255, 0, 100};

  auto bullet_entity =
      new Flag::Entity(g_game_manager->main_world, component_list);
  auto entity_id = g_game_manager->main_world->create_entity(bullet_entity);
  g_game_manager->main_world->get_entity(entity_id)->is_destroyed = false;

  g_game_manager->main_world->add_component(entity_id, transform);
  g_game_manager->main_world->add_component(entity_id, sprite);
  g_game_manager->main_world->add_component(entity_id, bullet);
  g_game_manager->main_world->add_component(entity_id, collider);
}

int EntityFactory::spawn_drop(std::string drop, vec2 pos, float dx, float dy) {
  auto data = g_res->get_item(drop);
  if (data.name.empty()) {
    Logger::error("EntityFactory failed to spawn drop: " + drop);
    return -1;
  }

  auto components = std::vector<std::shared_ptr<Flag::IComponent>>{};
  auto transform = std::make_shared<TransformComponent>();
  auto body = std::make_shared<BodyComponent>();
  auto sprite = std::make_shared<SpriteComponent>();

  transform->pos = pos;
  sprite->name = data.sprite;
  body->dx = dx;
  body->dy = dy;
  // TODO: deal with this magic number later
  body->weight = 10.0f;
  /*sprite->offset =
      vec2{static_cast<float>(data.get_param<ItemParamInt>("offset_x")->value),
           static_cast<float>(data.get_param<ItemParamInt>("offset_y")->value)};*/

  components.push_back(transform);
  components.push_back(body);
  components.push_back(sprite);

  auto entity = new Flag::Entity(g_game_manager->main_world, components);
  entity->is_destroyed = false;
  auto entity_id = g_game_manager->main_world->create_entity(entity);

  g_game_manager->main_world->add_component(entity_id, transform);
  g_game_manager->main_world->add_component(entity_id, sprite);
  g_game_manager->main_world->add_component(entity_id, body);
  return entity_id;
}
