#include "SDL_filesystem.h"
#include "SDL_gpu.h"
#include "SDL_pixels.h"
#include <array>
#include <fstream>
#include <string>
#include <thread>
#define CUTE_ASEPRITE_IMPLEMENTATION
#include "../core/Engine.hpp"
#include "../core/SoundManager.hpp"
#include "../core/global.hpp"
#include "../renderer/Sprite.hpp"
#include "../tools/Reader.hpp"
#include "Res.hpp"
#include "SDL_render.h"
#include "cute_aseprite.h"
#include "json.hpp"

#include "../tools/Logger.hpp"
#include <filesystem>

namespace fs = std::filesystem;

std::string get_aseprite_edited_time(const char *file) {
  auto ftime = fs::last_write_time(file);
  auto ftime_since_epoch = ftime.time_since_epoch();

  auto now = std::chrono::system_clock::now();
  auto now_since_epoch = now.time_since_epoch();

  auto duration = ftime_since_epoch - now_since_epoch;
  auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
      now + duration);

  auto cftime = std::chrono::system_clock::to_time_t(sctp);

  return std::to_string(cftime);
}

GPU_Image *CreateTextureFromRGBA(SDL_Renderer *renderer, const uint8_t *rgba,
                                 int width, int height) {
  SDL_Surface *surface =
      SDL_CreateRGBSurfaceFrom((void *)rgba, width, height, 32, width * 4,
                               0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

  if (!surface) {
    std::cerr << "Failed to create SDL surface: " << SDL_GetError()
              << std::endl;
    return nullptr;
  }

  GPU_Image *texture = GPU_CopyImageFromSurface(surface);
  GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);

  if (!texture) {
    std::cerr << "Failed to create SDL texture: " << SDL_GetError()
              << std::endl;
  }

  return texture;
}

Res::Res(SDL_Renderer *renderer) { m_renderer = renderer; }

Res::~Res() {}

void Res::init() {
  load_fonts();
  load_sounds();
  load_aseprites();
  load_animations();
  load_sprites();
  load_assets();
  load_shaders();
  load_data();

  start_save();
}

void Res::start_save() {
  // TODO: use the game name for the folder
  auto path = SDL_GetPrefPath("bigasdev", "_mygame");
  if (path) {
    m_save_path = std::string(path);
    SDL_free(path);
  } else {
    Logger::error("Failed to get save path!");
  }
}

// TODO: Multithread test + learning
std::unique_ptr<SaveData> Res::load_save() {
  auto data = std::make_unique<SaveData>();

  std::ifstream save_file(m_save_path + "savegame.json");
  if (!save_file.is_open()) {
    Logger::log("No save file found!");
    return nullptr;
  }

  nlohmann::json save_json;
  save_file >> save_json;
  save_file.close();

  data->hero_pos.x = save_json["hero"]["x"].get<float>();
  data->hero_pos.y = save_json["hero"]["y"].get<float>();

  Logger::log("Save loaded!");

  return data;
}

void Res::save(SaveData data) {
  if (is_saving) {
    Logger::log("Already saving!");
    return;
  }

  std::thread save_thread([this, data]() {
    is_saving = true;
    nlohmann::json save_json;
    save_json["hero"]["x"] = data.hero_pos.x;
    save_json["hero"]["y"] = data.hero_pos.y;

    std::ofstream save_file(m_save_path + "savegame.json");
    save_file << save_json.dump(4);
    save_file.close();

    SDL_Delay(1000);

    is_saving = false;
  });

  save_thread.detach();
}

void Res::load_fonts() {
  auto files = Reader::read_file("res/font");

  for (auto file : files) {
    std::string path = file;
    std::string file_name = path.substr(path.find_last_of("/\\") + 1);

    if (file_name.find("-") == std::string::npos) {
      Logger::error("Font file name must contain size, e.g. fontname-16.ttf: " +
                    file_name);
      continue;
    }

    std::string font_size = file_name.substr(file_name.find_last_of("-") + 1);
    font_size = font_size.substr(0, font_size.find_last_of("."));
    Logger::log_group("Font size: ", font_size);
    file_name = file_name.substr(0, file_name.find_last_of("-"));
    Logger::log("Loading font: " + file_name);
    TTF_Font *font = TTF_OpenFont(path.c_str(), std::stoi(font_size));
    if (font == nullptr) {
      Logger::error("Failed to load font: " + path);
    } else {
      Logger::log("Loaded font: " + path);
      m_fonts.insert(std::make_pair(file_name, font));
    }
  }
}

void Res::load_sounds() {
  auto sounds = Reader::get_extension_files("res/sounds", ".wav");

  for (auto sound : sounds) {
    std::string path = sound;
    std::string file_name = path.substr(path.find_last_of("/\\") + 1);
    file_name = file_name.substr(0, file_name.find_last_of("."));
    Logger::log("Loading sound: " + file_name);
    g_sound_manager->load_sound(path.c_str(), file_name);
  }
}

void Res::load_aseprites() {
  auto files = Reader::get_extension_files("res/", ".aseprite");
  m_aseprite_files.clear();
  m_sprite_anims.clear();

  if (m_aseprite_textures.size() > 0) {
    Logger::log("Detected changes!! Reloading...");
    for (auto const &file : files) {
      std::string fileName = file.substr(file.find_last_of("/") + 1);
      fileName = fileName.substr(0, fileName.find_last_of("."));

      GPU_Image **text = get_texture(fileName.c_str());

      if (text != nullptr) {
        ase_t *ase = cute_aseprite_load_from_file(file.c_str(), NULL);

        auto tags = ase->tag_count;

        if (tags > 0) {
          for (int i = 0; i < tags; i++) {
            ase_tag_t tag = ase->tags[i];
            Logger::log_group("Aseprite Tag: ", tag.name);
            Logger::log_group("From frame: ", std::to_string(tag.from_frame));
            Logger::log_group("To frame: ", std::to_string(tag.to_frame));

            SpriteAnim anim;
            anim.name = tag.name;
            anim.m_start_frame = tag.from_frame;
            anim.m_end_frame = tag.to_frame;
            anim.loop = tag.repeat;

            for (int f = tag.from_frame; f <= tag.to_frame; f++) {
              ase_frame_t *frame = &ase->frames[f];
              const uint8_t *rgba = reinterpret_cast<const uint8_t *>(
                  frame->ase->frames[f].pixels);
              GPU_Image *texture = CreateTextureFromRGBA(
                  m_renderer, rgba, frame->ase->w, frame->ase->h);

              SpriteFrame spriteFrame;
              spriteFrame.texture = texture;
              anim.frames.push_back(spriteFrame);
            }

            m_sprite_anims.insert(
                std::pair<std::string, SpriteAnim>(anim.name, anim));
          }
        }

        ase_frame_t *frame = &ase->frames[0];
        const uint8_t *rgba =
            reinterpret_cast<const uint8_t *>(frame->ase->frames[0].pixels);
        GPU_Image *texture = CreateTextureFromRGBA(
            m_renderer, rgba, frame->ase->w, frame->ase->h);

        AsepriteHelper asepriteHelper;
        asepriteHelper.file = file;
        asepriteHelper.last_edited_time =
            get_aseprite_edited_time(file.c_str());
        m_aseprite_files.push_back(asepriteHelper);

        auto it = m_aseprite_textures.find(fileName.c_str());
        it->second = texture;

        Logger::log("Aseprite file reloaded: " + file);
      }
    }

    return;
  }

  for (auto const &file : files) {
    // first we get just the file name without the aseprite extension
    std::string fileName = file.substr(file.find_last_of("/") + 1);
    fileName = fileName.substr(0, fileName.find_last_of("."));

    Logger::log(file.c_str());

    ase_t *ase = cute_aseprite_load_from_file(file.c_str(), NULL);
    auto tags = ase->tag_count;

    if (tags > 0) {
      for (int i = 0; i < tags; i++) {
        ase_tag_t tag = ase->tags[i];
        Logger::log_group("Aseprite Tag: ", tag.name);
        Logger::log_group("From frame: ", std::to_string(tag.from_frame));
        Logger::log_group("To frame: ", std::to_string(tag.to_frame));

        SpriteAnim anim;
        anim.name = tag.name;
        anim.m_start_frame = tag.from_frame;
        anim.m_end_frame = tag.to_frame;
        anim.loop = tag.repeat;

        for (int f = tag.from_frame; f <= tag.to_frame; f++) {
          std::cout << f << std::endl;
          ase_frame_t *frame = &ase->frames[f];
          const uint8_t *rgba =
              reinterpret_cast<const uint8_t *>(frame->ase->frames[f].pixels);
          GPU_Image *texture = CreateTextureFromRGBA(
              m_renderer, rgba, frame->ase->w, frame->ase->h);

          SpriteFrame spriteFrame;
          spriteFrame.texture = texture;
          anim.frames.push_back(spriteFrame);
        }

        m_sprite_anims.insert(
            std::pair<std::string, SpriteAnim>(anim.name, anim));
      }
    }

    ase_frame_t *frame = &ase->frames[0];
    const uint8_t *rgba =
        reinterpret_cast<const uint8_t *>(frame->ase->frames[0].pixels);
    GPU_Image *texture =
        CreateTextureFromRGBA(m_renderer, rgba, frame->ase->w, frame->ase->h);

    AsepriteHelper asepriteHelper;
    asepriteHelper.file = file;
    asepriteHelper.last_edited_time = get_aseprite_edited_time(file.c_str());
    m_aseprite_files.push_back(asepriteHelper);

    auto pair = std::make_pair(fileName.c_str(), texture);

    m_aseprite_textures.insert(pair);
  }
  Logger::log("Finished aseprite load!");
  Logger::log("------------");
  for (auto const &aseprite : m_aseprite_textures) {
    Logger::log_group("Aseprite", aseprite.first);
  }
}

void Res::load_sprites() {
  auto files = Reader::get_extension_files("res/sprites", ".json");

  for (auto file : files) {
    std::string path = file;
    std::string file_name = path.substr(path.find_last_of("/\\") + 1);
    file_name = file_name.substr(0, file_name.find_last_of("."));
    Logger::log("Loading sprite: " + file_name);

    std::string json = Reader::get_file_contents(file);
    if (json.empty()) {
      Logger::error("Failed to read prefab: " + file);
      continue;
    }

    auto prefab = nlohmann::json::parse(json);

    // loop through all the keys in the json array
    // get all of the values from the json and try to create an Sprite from it
    for (auto &[key, value] : prefab.items()) {
      try {
        auto name = value["name"].get<std::string>();
        auto dst_x = value["x"].get<float>();
        auto dst_y = value["y"].get<float>();
        auto wid = value["width"].get<int>();
        auto hei = value["height"].get<int>();
        auto palette_name = value["palette"].get<std::string>();

        auto spr = Sprite();
        spr.sheet = palette_name;
        spr.dst_x = dst_x;
        spr.dst_y = dst_y;
        spr.wid = wid;
        spr.hei = hei;

        m_sprites.insert(std::make_pair(name, spr));

        Logger::log("Sprite loaded: " + name);
      } catch (nlohmann::json::exception &e) {
        Logger::error("Failed to load sprite: " + file + " " + e.what());
      }
    }
    // m_prefabs.insert(std::make_pair(file_name, file));
  }
}

//"assets" are the png files
void Res::load_assets() {
  auto files = Reader::get_extension_files("res/assets", ".png");
  m_asset_textures.clear();
  m_asset_files.clear();

  for (auto file : files) {
    std::string path = file;
    std::string file_name = path.substr(path.find_last_of("/\\") + 1);
    file_name = file_name.substr(0, file_name.find_last_of("."));
    Logger::log("Loading asset: " + file_name);

    GPU_Image *texture = GPU_LoadImage(file.c_str());
    if (!texture) {
      Logger::error("Failed to load asset: " + file);
      continue;
    }
    GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);

    m_asset_textures.insert(
        std::pair<const char *, GPU_Image *>(file_name.c_str(), texture));
    Logger::log("Asset loaded: " + file_name);

    AssetHelper assetHelper;
    assetHelper.file = file;
    assetHelper.last_edited_time = get_aseprite_edited_time(file.c_str());
    m_asset_files.push_back(assetHelper);
  }
}

// FIX: To my older self..
//  this shader part needs to be rewritten to easily load more shaders, for now
//  its hard Loading remember for every frag we need a vert (and we can change
//  the .glsl to .frag and .vert)
void Res::load_shaders() {
  // auto files = Reader::get_extension_files("res/shaders", ".glsl");
  std::string shader_vert = "res/shaders/light_vertex.glsl";
  std::string shader_frag = "res/shaders/newoutline.frag.glsl";

  std::string vert_code = Reader::get_file_contents(shader_vert);
  std::string frag_code = Reader::get_file_contents(shader_frag);

  if (vert_code.empty() || frag_code.empty()) {
    Logger::error("Failed to read shader: " + shader_vert + " or " +
                  shader_frag);
    return;
  }

  auto vert_shader = GPU_CompileShader(GPU_VERTEX_SHADER, vert_code.c_str());
  if (!vert_shader) {
    Logger::error("Failed to compile shader: " + shader_vert);
    return;
  }

  auto frag_shader = GPU_CompileShader(GPU_FRAGMENT_SHADER, frag_code.c_str());
  if (!frag_shader) {
    Logger::error("Failed to compile shader: " + shader_frag);
    return;
  }

  Uint32 program = GPU_LinkShaders(vert_shader, frag_shader);
  if (!program) {
    Logger::error("Failed to link shader: " + shader_vert + " and " +
                  shader_frag);
    return;
  }

  GPU_ShaderBlock block =
      GPU_LoadShaderBlock(program, "gpu_Vertex", "gpu_TexCoord", "gpu_Color",
                          "gpu_ModelViewProjectionMatrix");

  GPU_ActivateShaderProgram(program, &block);
  m_shader_blocks.push_back(block);
  m_shaders_id.push_back(program);
  Logger::log("Loaded shader: " + shader_vert + " and " + shader_frag);

  /*for (auto file : files) {
    std::string path = file;
    std::string file_name = path.substr(path.find_last_of("/\\") + 1);
    file_name = file_name.substr(0, file_name.find_last_of("."));
    Logger::log("Loading shader: " + file_name);

    std::string shader_code = Reader::get_file_contents(file);
    if(shader_code.empty()){
      Logger::error("Failed to read shader: " + file);
      continue;
    }

    GPU_ShaderEnum shader_type;
    if (file_name.find("vertex") != std::string::npos) {
      shader_type = GPU_VERTEX_SHADER;
    } else if (file_name.find("fragment") != std::string::npos) {
      shader_type = GPU_FRAGMENT_SHADER;
    } else {
      Logger::error("Failed to get shader type: " + file);
      continue;
    }

    GPU_ShaderBlock block;
    auto shader = GPU_CompileShader(shader_type, shader_code.c_str());
    if (!shader) {
      Logger::error("Failed to compile shader: " + file);
      continue;
    }

    auto program = GPU_LinkShaders(shader, 0);
    if (!program) {
      Logger::error("Failed to link shader: " + file);
      GPU_FreeShader(shader);
      continue;
    }

    m_shaders.push_back(file);
  }*/
}

void Res::load_data() {
  auto files = Reader::get_extension_files("res/data", ".json");

  for (auto file : files) {
    std::string path = file;
    std::string file_name = path.substr(path.find_last_of("/\\") + 1);
    file_name = file_name.substr(0, file_name.find_last_of("."));
    Logger::log("Loading data: " + file_name);

    std::string json = Reader::get_file_contents(file);
    if (json.empty()) {
      Logger::error("Failed to read data: " + file);
      continue;
    }

    auto data = nlohmann::json::parse(json);

    for (auto &value : data["data"]) {
      try {
        auto name = value["name"].get<std::string>();
        auto folder = value["folder"].get<std::string>();

        Item item;
        item.name = name;
        item.folder = folder;

        if (value.contains("sprite")) {
          auto sprite_name = value["sprite"]["name"].get<std::string>();
          item.sprite = sprite_name;
        }

        if (value.contains("params")) {
          for (auto &p : value["params"]) {
            auto name = p["name"].get<std::string>();
            auto type = p["type"].get<std::string>();

            if (type == "int") {
              auto val = p["value"].get<int>();
              auto param = std::make_unique<ItemParamInt>();
              param->value = val;
              item.params.insert(std::make_pair(name, std::move(param)));
            } else if (type == "float") {
              auto val = p["value"].get<float>();
              auto param = std::make_unique<ItemParamFloat>();
              param->value = val;
              item.params.insert(std::make_pair(name, std::move(param)));
            } else if (type == "string") {
              auto val = p["value"].get<std::string>();
              auto param = std::make_unique<ItemParamString>();
              param->value = val;
              item.params.insert(std::make_pair(name, std::move(param)));
            } else if (type == "bool") {
              auto val = p["value"].get<bool>();
              auto param = std::make_unique<ItemParamBool>();
              param->value = val;
              item.params.insert(std::make_pair(name, std::move(param)));
            } else if (type == "vec2") {
              auto x = p["value"]["x"].get<float>();
              auto y = p["value"]["y"].get<float>();
              auto param = std::make_unique<ItemParamVec2>();
              param->value = vec2{x, y};
              item.params.insert(std::make_pair(name, std::move(param)));
            } else {
              Logger::error("Unknown param type: " + type);
            }
          }
        }

        auto key_name = folder + "/" + name;
        m_items.insert(std::make_pair(key_name, item));

        Logger::log("Data loaded: " + key_name);
      } catch (nlohmann::json::exception &e) {
        Logger::error("Failed to load data: " + file + " " + e.what());
      }
    }
    // m_prefabs.insert(std::make_pair(file_name, file));
  }
}

std::vector<Item> Res::get_items_from_folder(std::string folder) {
  std::vector<Item> items;

  for (const auto &[key, item] : m_items) {
    if (item.folder == folder) {
      items.push_back(item);
    }
  }

  return items;
}

void Res::load_animations() {}

const Item &Res::get_item(std::string name) {
  try {
    return m_items.at(name);
  } catch (const std::out_of_range &e) {
    Logger::error("Item " + std::string(name) + " not found!");
    throw;
  }
}

Sprite Res::get_sprite(std::string name) {
  try {
    return m_sprites.at(name);
  } catch (const std::out_of_range &e) {
    Logger::error("Sprite " + std::string(name) + " not found!");
    return Sprite();
  }
}

SpriteAnim Res::get_animation(std::string name) {
  try {
    return m_sprite_anims.at(name);
  } catch (const std::out_of_range &e) {
    Logger::error("Sprite Anim" + std::string(name) + " not found!");
    return SpriteAnim();
  }
}

// FIX: needs to be rewrite later, this is just returning the light shader for
// now
Uint32 Res::get_shader_id() { return m_shaders_id[0]; }
// same as the above
GPU_ShaderBlock Res::get_shader_block() { return m_shader_blocks[0]; }

GPU_Image **Res::get_texture(std::string name) {
  try {
    return &m_aseprite_textures.at(name);
  } catch (const std::out_of_range &e) {
    Logger::error("Texture " + std::string(name) + " not found!");
    return nullptr;
  }
}

GPU_Image **Res::get_asset_texture(std::string name) {
  try {
    return &m_asset_textures.at(name);
  } catch (const std::out_of_range &e) {
    Logger::error("Asset Texture " + std::string(name) + " not found!");
    return nullptr;
  }
}

void Res::update() {
#if _DEBUG
  for (auto f : m_aseprite_files) {
    std::string new_edited_time = get_aseprite_edited_time(f.file.c_str());

    if (new_edited_time != f.last_edited_time) {
      Logger::log("Aseprite file changed: " + f.file);
      f.last_edited_time = new_edited_time;
      load_aseprites();
    }
  }

  for (auto f : m_asset_files) {
    std::string new_edited_time = get_aseprite_edited_time(f.file.c_str());

    if (new_edited_time != f.last_edited_time) {
      Logger::log("Asset file changed: " + f.file);
      f.last_edited_time = new_edited_time;
      load_assets();
    }
  }
#endif
}
