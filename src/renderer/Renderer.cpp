#include "Renderer.hpp"
#include "../core/Engine.hpp"
#include "../core/global.hpp"
#include "../renderer/Sprite.hpp"
#include "../res/Res.hpp"
#include "../tools/Logger.hpp"
#include "Camera.hpp"
#include "SDL_gpu.h"

Renderer::Renderer(GPU_Target *gpu) : m_gpu(gpu) {}

Renderer::~Renderer() {}

void Renderer::post_update() { m_calls = 0; }

void Renderer::init_shader(std::vector<std::string> shaders) {}

void Renderer::draw_rect(Rect rect, Col color, bool fill) {
  if (!fill) {
    // aditive mode
    GPU_Rectangle(m_gpu, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h,
                  {color.r, color.g, color.b, color.a});
  } else {
    GPU_RectangleFilled(m_gpu, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h,
                        {color.r, color.g, color.b, color.a});
  }
  m_calls++;
}

void Renderer::draw_circle(vec2 pos, int radius, Col color, bool fill) {
  if (!fill) {
    GPU_Circle(m_gpu, pos.x, pos.y, radius,
               {color.r, color.g, color.b, color.a});
  } else {
    GPU_CircleFilled(m_gpu, pos.x, pos.y, radius,
                     {color.r, color.g, color.b, color.a});
  }
  m_calls++;
}

void Renderer::draw_line(Line line, Col color) {
  GPU_Line(m_gpu, line.x1, line.y1, line.x2, line.y2,
           {color.r, color.g, color.b, color.a});
  m_calls++;
}

void Renderer::draw_text(vec2 pos, const char *text, TTF_Font *font, Col color,
                         int size, int width) {
  SDL_Surface *surfaceMessage = TTF_RenderText_Blended_Wrapped(
      font, text, {color.r, color.g, color.b, color.a}, width);
  GPU_Image *message = GPU_CopyImageFromSurface(surfaceMessage);
  GPU_SetImageFilter(message, GPU_FILTER_NEAREST);

  GPU_Rect src;
  src.x = 0;
  src.y = 0;
  src.w = surfaceMessage->w;
  src.h = surfaceMessage->h;

  GPU_Rect dst;
  dst.x = pos.x;
  dst.y = pos.y;
  dst.w = src.w * size;
  dst.h = src.h * size;

  SDL_FreeSurface(surfaceMessage);
  GPU_BlitRect(message, &src, m_gpu, &dst);
  GPU_FreeImage(message);
  m_calls++;
}

void Renderer::draw_from_sheet(GPU_Image *sheet, vec2 pos, Rect l_point,
                               bool use_shader) {
  GPU_Rect src;
  src.x = l_point.x * l_point.w;
  src.y = l_point.y * l_point.h;
  src.w = l_point.w;
  src.h = l_point.h;

  GPU_Rect dst;
  dst.x = static_cast<int>(pos.x);
  dst.y = static_cast<int>(pos.y);
  // the zoom is the key..
  dst.w = src.w * g_camera->get_game_scale();
  dst.h = src.h * g_camera->get_game_scale();

  if (use_shader) {
    auto program = g_res->get_shader_id();
    GPU_ShaderBlock block = g_res->get_shader_block();
    GPU_ActivateShaderProgram(program, &block);
    // auto col = g_res->get_color_primitive(2);
    GPU_SetUniformf(GPU_GetUniformLocation(program, "r"), 1.0);
    GPU_SetUniformf(GPU_GetUniformLocation(program, "g"), 0.0);
    GPU_SetUniformf(GPU_GetUniformLocation(program, "b"), 0.0);
    GPU_SetUniformf(GPU_GetUniformLocation(program, "width"), sheet->w);
    GPU_SetUniformf(GPU_GetUniformLocation(program, "height"), sheet->h);
  }

  GPU_BlitRectX(sheet, &src, m_gpu, &dst, 0, 0, 0, GPU_FLIP_NONE);

  m_calls++;

  GPU_DeactivateShaderProgram();
}

// draw of a sprite, this is the same as for drawing an entity but it can be
// used standalone
void Renderer::draw(GPU_Image *sheet, Sprite spr, vec2 pos) {
  if (!spr.visible)
    return;

  GPU_Rect src;
  src.x = spr.dst_x * spr.wid;
  src.y = spr.dst_y * spr.hei;
  src.w = spr.wid;
  src.h = spr.hei;

  GPU_Rect dst;
  dst.x = static_cast<int>(pos.x + (spr.spr_x * g_camera->get_game_scale()));
  dst.y = static_cast<int>(pos.y + (spr.spr_y * g_camera->get_game_scale()));
  // this works??
  dst.w = spr.wid * g_camera->get_game_scale() * spr.scale_x * spr.squash_x;
  dst.h = spr.hei * g_camera->get_game_scale() * spr.scale_y * spr.squash_y;

  GPU_FlipEnum flip = spr.dir == -1 ? GPU_FLIP_HORIZONTAL : GPU_FLIP_NONE;

  GPU_BlitRectX(sheet, &src, m_gpu, &dst, spr.angle, spr.pivot_x, spr.pivot_y,
                flip);
  m_calls++;
}

void Renderer::draw_additive(std::string name, vec2 pos) {
  // TODO: check this later to not have to load every time
  //  maybe switch the ownership to the system that will call it
  //  or maybe a map from commons lights
  auto texture = g_res->get_asset_texture(name);
  if (!texture) {
    Logger::error("Failed to get asset texture: " + name);
    return;
  }

  GPU_SetBlendMode(*texture, GPU_BLEND_ADD);
  GPU_SetRGBA(*texture, 255, 255, 255, 40);
  draw_asset(*texture, pos);
}

void Renderer::draw_asset(GPU_Image *sheet, vec2 pos, float angle, int pivot_x,
                          int pivot_y, int dir) {
  GPU_Rect src;
  src.x = 0;
  src.y = 0;
  src.w = sheet->w;
  src.h = sheet->h;

  GPU_Rect dst;
  dst.x = static_cast<int>(pos.x);
  dst.y = static_cast<int>(pos.y);
  // the zoom is the key..
  dst.w = src.w * g_camera->get_game_scale();
  dst.h = src.h * g_camera->get_game_scale();

  GPU_FlipEnum flip = dir == -1 ? GPU_FLIP_HORIZONTAL : GPU_FLIP_NONE;

  GPU_BlitRectX(sheet, &src, m_gpu, &dst, angle, pivot_x, pivot_y, flip);
  m_calls++;
}
