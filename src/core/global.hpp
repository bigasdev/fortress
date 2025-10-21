#pragma once
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

class Engine;
class Res;
class SoundManager;
class Renderer;
class Camera;
class Cooldown;
class InputManager;
class TickManager;
class Entity;
class GameManager;
class EntityFactory;

extern Engine *g_engine;
extern GameManager *g_game_manager;
extern Res *g_res;
extern Renderer *g_renderer;
extern Camera *g_camera;
extern Cooldown *g_cooldown;
extern SoundManager *g_sound_manager;
extern InputManager *g_input_manager;
extern TickManager *g_tick_manager;
extern EntityFactory *g_entity_factory;

extern bool g_controller_connected;
extern bool g_paused;

extern float g_light_falloff;

#endif
