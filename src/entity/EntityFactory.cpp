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
#include "components/PlayerMoveComponent.hpp"
#include "components/SpriteAnimatedComponent.hpp"
#include "components/SpriteComponent.hpp"
#include "components/WeaponComponent.hpp"
#include "res/Res.hpp"
#include "res/autogen_res.hpp"
#include "tools/Logger.hpp"

EntityFactory::EntityFactory() {}

int EntityFactory::spawn_hero(vec2 pos) {
  auto player_components = std::vector<std::shared_ptr<Flag::IComponent>>{};

  auto hero_transform = std::make_shared<TransformComponent>();
  auto save_data = g_res->load_save();
  hero_transform->pos = {100, 100};
  if (save_data) {
    hero_transform->pos = save_data->hero_pos;
  }
  hero_transform->scale = vec2{1, 1};

  auto hero_collider = std::make_shared<CollisionBoxComponent>();
  hero_collider->layer = ColLayers::PLAYER;
  hero_collider->size = vec2{8, 10};
  hero_collider->gizmo_color = {0, 255, 0, 100};

  auto hero_player_move = std::make_shared<PlayerMoveComponent>();
  hero_player_move->speed = 100.0f;

  auto hero_anim_spr = std::make_shared<SpriteAnimatedComponent>();
  auto hero_idle = g_res->get_animation("Bigas_Idle");
  hero_anim_spr->offset = vec2{-12, -16};
  hero_anim_spr->anims.insert(std::make_pair("idle", hero_idle));
  hero_anim_spr->default_anim = hero_idle;
  hero_anim_spr->current_anim = hero_idle;

  player_components.push_back(hero_transform);
  player_components.push_back(hero_collider);
  player_components.push_back(hero_player_move);
  player_components.push_back(hero_anim_spr);

  auto player = new Flag::Entity(g_game_manager->main_world, player_components);

  auto id = g_game_manager->main_world->create_entity(player);
  g_game_manager->main_world->add_component(id, hero_transform);
  g_game_manager->main_world->add_component(id, hero_collider);
  g_game_manager->main_world->add_component(id, hero_player_move);
  g_game_manager->main_world->add_component(id, hero_anim_spr);

  auto weapon_data = g_res->get_item(res::data::items_wooden_dagger);
  auto sword_components = std::vector<std::shared_ptr<Flag::IComponent>>{};
  auto sword_transform = std::make_shared<TransformComponent>();
  sword_transform->pos = vec2{150, 100};
  sword_transform->scale = vec2{1, 1};
  sword_transform->pivot_x = 0;
  sword_transform->pivot_y = 13;
  auto sword_spr = std::make_shared<SpriteComponent>();
  sword_spr->name = weapon_data.sprite;
  auto sword_component = std::make_shared<WeaponComponent>();
  sword_component->following = id;

  sword_components.push_back(sword_transform);
  sword_components.push_back(sword_spr);
  sword_components.push_back(sword_component);
  auto sword = new Flag::Entity(g_game_manager->main_world, sword_components);

  auto sword_id = g_game_manager->main_world->create_entity(sword);
  g_game_manager->main_world->add_component(sword_id, sword_transform);
  g_game_manager->main_world->add_component(sword_id, sword_spr);
  g_game_manager->main_world->add_component(sword_id, sword_component);
  return id;
}

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

  auto collider = std::make_shared<CollisionBoxComponent>();
  collider->layer = ColLayers::ENEMY;
  collider->size = vec2{19, 16};
  collider->gizmo_color = {55, 0, 255, 100};
  // orc->add_component(orc_collider, 15);
  g_game_manager->main_world->add_component(entity_id, collider);

  g_game_manager->main_world->add_component(entity_id, transform);
  g_game_manager->main_world->add_component(entity_id, health);
  g_game_manager->main_world->add_component(entity_id, sprite);
  if (drop != nullptr)
    g_game_manager->main_world->add_component(entity_id, dropper);
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
