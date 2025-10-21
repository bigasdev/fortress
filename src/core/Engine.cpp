#include "Engine.hpp"
#include "../game/Game.hpp"
#include "../imgui/imgui.h"
#include "../renderer/AppGui.hpp"
#include "../renderer/Camera.hpp"
#include "../renderer/Renderer.hpp"
#include "../res/Res.hpp"
#include "../tools/Logger.hpp"
#include "../tools/Math.hpp"
#include "../tools/Profiler.hpp"
#include "Assert.hpp"
#include "InputManager.hpp"
#include "SDL.h"
#include "SDL_events.h"
#include "SDL_gpu.h"
#include "SDL_hints.h"
#include "SDL_image.h"
#include "SDL_keycode.h"
#include "SDL_mixer.h"
#include "SDL_render.h"
#include "SDL_scancode.h"
#include "SDL_timer.h"
#include "SDL_ttf.h"
#include "SDL_video.h"
#include "SoundManager.hpp"
#include "Timer.hpp"
#include "global.hpp"
#include <cassert>
#include <iostream>

// init
#ifndef WIN_WIDTH
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#endif

Engine::Engine() { Logger::setup_crash_handlers(); }

Engine::~Engine() {}

void Engine::init() {
  int init = -99;
#ifdef __EMSCRIPTEN__
  init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
#else
  init = SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_AUDIO);
#endif

  R_ASSERT(init == 0);

  if (init == 0) {
    Logger::log("SDL2 initialized");
    Logger::log_group("SDL2 version", SDL_GetRevision());
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  // get computer resolution
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);

  SDL_WindowFlags window_flags =
      (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI |
                        SDL_WINDOW_RESIZABLE);
  m_sdl_window = SDL_CreateWindow("Game", DM.w - (WIN_WIDTH * 1.1f),
                                  DM.h - (WIN_HEIGHT * 1.1f), WIN_WIDTH,
                                  WIN_HEIGHT, window_flags);
  m_window_size = {WIN_WIDTH, WIN_HEIGHT};

  GPU_SetInitWindow(SDL_GetWindowID(m_sdl_window));

  m_sdl_renderer =
      SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_ACCELERATED);
  R_ASSERT(m_sdl_renderer != nullptr);
  m_gpu = GPU_InitRenderer(GPU_RENDERER_OPENGL_3, WIN_WIDTH, WIN_HEIGHT,
                           SDL_RENDERER_ACCELERATED);
  R_ASSERT(m_gpu != nullptr);

  GPU_SetWindowResolution(m_window_size.x, m_window_size.y);

  if (m_renderer != nullptr && m_gpu != nullptr) {
    Logger::log("SDL2 renderer created");
    Logger::log("SDL2 GPU created");
  }
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) {
    Logger::log("SDL2 image initialized");
  }

  SDL_GL_SetSwapInterval(1);

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0) {
    Logger::log("SDL2 mixer initialized");
  } else {
    Logger::log("SDL2 mixer failed to initialize " +
                std::string(Mix_GetError()));
  }

  TTF_Init();

  R_ASSERT(m_sdl_window != nullptr);

  if (m_sdl_window != nullptr) {
    Logger::log("SDL2 window created");
  }

  m_running = true;
}

void Engine::post_init() {}

void Engine::load_step() {
  switch (m_load_state) {
  case LoadState::PROFILER:
    m_profiler = new Profiler();
    Logger::log("Profiler initialized");
    m_load_state = LoadState::RENDERER;
    m_load_progress += 5.0f;
    break;
  case LoadState::RENDERER:
    m_renderer = new Renderer(m_gpu);
    m_load_state = LoadState::SOUND;
    Logger::log("Renderer initialized");
    m_load_progress += 10.0f;
    break;
  case LoadState::SOUND:
    m_sound_manager = new SoundManager();
    g_sound_manager = m_sound_manager;
    m_load_state = LoadState::INPUT;
    Logger::log("Sound manager initialized");
    m_load_progress += 5.0f;
    ;
    break;
  case LoadState::INPUT:
    m_input_manager = new InputManager();
    g_input_manager = m_input_manager;
    m_load_state = LoadState::RESOURCES;
    Logger::log("Input manager initialized");
    m_load_progress += 5.0f;
    break;
  case LoadState::RESOURCES:
    m_res = new Res(m_sdl_renderer);
    m_res->init();
    g_engine = this;
    g_res = m_res;
    g_renderer = m_renderer;
    m_load_state = LoadState::INIT_SHADER;
    Logger::log("Resources initialized");
    m_load_progress += 50.0f;
    break;
  case LoadState::INIT_SHADER:
    // initializing shaders after resources are loaded
    m_renderer->init_shader(m_res->get_shaders());
    m_load_state = LoadState::START_GAME;
    Logger::log("Shaders initialized");
    m_load_progress += 5.0f;
    break;
  case LoadState::START_GAME:
    // starting game
    m_game = new Game();
    m_game->init();
    Logger::log("Game initialized");
    m_load_state = LoadState::START_IMGUI;
    m_load_progress += 20.0f;
    break;
  case LoadState::START_IMGUI:
#if _IMGUI
    SDL_GLContext &gl_context = m_gpu->context->context;
    GUI::setup(m_sdl_window, gl_context);
#endif
    Logger::log("Engine post init");
    m_loaded = true;
    break;
  }
}

bool m_moving = false;
void Engine::input() {
  if (!m_loaded)
    return;

  SDL_Event event;

  while (SDL_PollEvent(&event) != 0) {
#if _IMGUI
    GUI::event(event);
#endif
    g_input_manager->update(event);
    switch (event.type) {
    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        // updating window size
        {
          int h = 0, w = 0;
          SDL_GetWindowSize(m_sdl_window, &h, &w);
          m_window_size.x = h;
          m_window_size.y = w;
          GPU_SetWindowResolution(h, w);
        }
      }
      break;
    case SDL_QUIT:
      m_running = false;
      break;

    case SDL_KEYDOWN:
      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_ESCAPE:
#if _DEBUG
        m_running = false;
#endif
        break;
      }
    }
  }
}

void Engine::fixed_update() {
  if (!m_loaded) {
    return;
  }

  m_game->fixed_update(Timer::get_tmod());
}

void Engine::update() {
  if (!m_loaded) {
    load_step();
    return;
  }

  g_input_manager->tick_update();
  m_game->update(Timer::get_dt());
}

void Engine::post_update() {
  if (!m_loaded) {
    return;
  }

  m_game->post_update(Timer::get_dt());

#if _DEBUG
  m_profiler->update();
  m_renderer->post_update();
  m_res->update();
#endif
}

void Engine::draw() {
  GPU_Clear(m_gpu);
  if (!m_loaded) {
    GPU_SetCamera(m_gpu, nullptr);
    // loading screen will be here
    std::cout << "Loading... " << m_load_progress << "%\n";
    Rect rect;
    Col col = {255, 0, 0, 255};
    rect.x = m_window_size.x / 4;
    rect.y = m_window_size.y / 2 - 10;
    rect.w = (m_window_size.x / 2) * (m_load_progress / 100.0f);
    rect.h = 20;

    GPU_RectangleFilled(m_gpu, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h,
                        {col.r, col.g, col.b, col.a});

    GPU_Flip(m_gpu);
    return;
  }

  GPU_SetCamera(m_gpu, *g_camera->get_gpu_cam());

  m_game->draw_root();
  m_game->draw_ent();

  // TODO: lightning system
  // if uses lightning
  if (true) {
    Rect quad;
    quad.x = g_camera->get_pos().x;
    quad.y = g_camera->get_pos().y;
    quad.w = m_window_size.x;
    quad.h = m_window_size.y;
    Col col = {1, 1, 20, 90};
    auto light_falloff = Math::max(g_light_falloff, 1.0f);
    col.a = 100 * light_falloff;
    m_renderer->draw_rect(quad, col, true);

    // for each light point, just pseudo code to think on what i need
    //  for (auto light : lights) {
    g_renderer->draw_additive("light_test", vec2{100, 100});
    g_renderer->draw_additive("light_test2", vec2{100, 100});
  }

  // game draw
  GPU_SetCamera(m_gpu, nullptr);
  m_game->draw_ui();
  GPU_DeactivateShaderProgram();

#if _DEBUG
  GPU_SetCamera(m_gpu, nullptr);
  m_profiler->draw();

#if _IMGUI
  GPU_FlushBlitBuffer();
  GUI::draw([&]() {});
#endif

#endif

  GPU_Flip(m_gpu);
}

void Engine::quit() {
  m_game->clean();
  SDL_DestroyWindow(SDL_GetWindowFromID(GPU_GetInitWindow()));
  SDL_Quit();
  Logger::log("SDL2 quit");
  Logger::write_to_file("log.txt");
}
