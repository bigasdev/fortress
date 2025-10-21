#include "global.hpp"

Engine *g_engine;
GameManager *g_game_manager;
Res *g_res;
Renderer *g_renderer;
Camera *g_camera;
Cooldown *g_cooldown;
SoundManager *g_sound_manager;
InputManager *g_input_manager;
TickManager *g_tick_manager;
EntityFactory *g_entity_factory;

bool g_controller_connected;
bool g_paused;

float g_light_falloff;
