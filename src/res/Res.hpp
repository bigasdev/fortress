#pragma once
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include <memory>
#include <unordered_map>
#ifndef RES_HPP
#define RES_HPP
#include "../tools/Common.hpp"
#include "SDL_gpu.h"
#include "SDL_ttf.h"

#include "../entity/Types.hpp"
#include "autogen_res.hpp"
#include "tools/Logger.hpp"
#include <iostream>
#include <map>
#include <vector>

class AssetData;
class Sprite;
class SpriteFrame;

struct AsepriteHelper {
  std::string file;
  std::string last_edited_time;
};

struct AssetHelper {
  std::string file;
  std::string last_edited_time;
};

struct SpriteFrame {
  GPU_Image *texture = nullptr;
};

struct SpriteAnim {
  std::string name = "";
  int m_start_frame = 0;
  int m_curr_frame = 0;
  int m_end_frame = 0;
  int priority = 0;
  bool loop = true;
  std::vector<SpriteFrame> frames;
};

struct ItemParam {
  virtual ~ItemParam() = default;

  std::string name = "Param";
};

struct ItemParamInt : public ItemParam {
  int value;
};

struct ItemParamFloat : public ItemParam {
  float value;
};

struct ItemParamString : public ItemParam {
  std::string value;
};

struct ItemParamBool : public ItemParam {
  bool value;
};

struct ItemParamVec2 : public ItemParam {
  vec2 value;
};

struct Item {
  std::string name = "";
  std::string folder = "";
  std::string sprite = "";

  template <typename T> T *get_param(const std::string &param_name) {
    try {
      auto &param = params.at(param_name); // unique_ptr<ItemParam>&
      if (auto casted = dynamic_cast<T *>(param.get())) {
        return casted;
      } else {
        Logger::error("Param " + param_name + " in item " + name +
                      " is of the wrong type");
        return nullptr;
      }
    } catch (const std::out_of_range &e) {
      Logger::error("Param " + param_name + " not found in item " + name);
      return nullptr;
    }
  }

  std::unordered_map<std::string, std::shared_ptr<ItemParam>> params;
};

// save system
struct SaveData {
  vec2 hero_pos;
};

class Res {
public:
  Res(SDL_Renderer *renderer);
  ~Res();

  void init();
  void update();

  void load_folder(std::string path);

  // save system
  void start_save();
  void test_func();
  std::unique_ptr<SaveData> load_save();
  void save(SaveData data);

  // loaders
  void load_fonts();
  void load_sounds();
  void load_aseprites();
  void load_assets();
  void load_sprites();
  void load_animations();
  void load_shaders();
  void load_data();

  // getters
  const Item &get_item(std::string name);
  std::vector<Item> get_items_from_folder(std::string folder);
  TTF_Font *get_font(std::string name) { return m_fonts[name]; }
  GPU_Image **get_texture(std::string name);
  GPU_Image **get_asset_texture(std::string name);
  std::vector<std::string> get_shaders() { return m_shaders; }
  Uint32 get_shader_id();
  GPU_ShaderBlock get_shader_block();
  Sprite get_sprite(std::string name);
  SpriteAnim get_animation(std::string name);

  void create_asset_data(std::string file, std::string name,
                         std::string folder);

  // TODO: temp variable for multithread saving
  bool is_saving = false;

private:
  std::map<std::string, GPU_Image *> m_textures;
  std::map<std::string, AssetData *> m_assets;
  std::map<std::string, GPU_Image *> m_aseprite_textures;
  std::map<std::string, GPU_Image *> m_asset_textures;
  std::map<std::string, TTF_Font *> m_fonts;
  std::vector<std::string> m_shaders;
  std::vector<Uint32> m_shaders_id;
  std::vector<GPU_ShaderBlock> m_shader_blocks;
  std::vector<AsepriteHelper> m_aseprite_files;
  std::vector<AssetHelper> m_asset_files;
  std::map<std::string, Sprite> m_sprites;

  // data
  std::unordered_map<std::string, Item> m_items;

  // animations
  std::map<std::string, SpriteAnim> m_sprite_anims;

  // save system
  std::string m_save_path = "";

  SDL_Renderer *m_renderer;
};
#endif
