#pragma once

#include "SDL_stdinc.h"
struct Rect {
  int x, y, w, h;

  Rect() : x(0), y(0), w(0), h(0) {}
  Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}

  bool intersects(const Rect &r) {
    return x < r.x + r.w && x + w > r.x && y < r.y + r.h && y + h > r.y;
  }
};

struct Line {
  int x1, y1, x2, y2;

  Line() : x1(0), y1(0), x2(0), y2(0) {}
  Line(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
};

struct vec2 {
  float x, y;

  vec2() : x(0), y(0) {}
  vec2(float x, float y) : x(x), y(y) {}
  bool zero() { return x == 0 && y == 0; }
  bool operator==(const vec2 &v) { return x == v.x && y == v.y; }
  bool operator!=(const vec2 &v) { return x != v.x || y != v.y; }
  vec2 operator+(const vec2 &v) { return vec2(x + v.x, y + v.y); }
  vec2 operator+=(const vec2 &v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  vec2 operator-(const vec2 &v) { return vec2(x - v.x, y - v.y); }
  vec2 operator*(const vec2 &v) { return vec2(x * v.x, y * v.y); }
  vec2 operator/(const vec2 &v) { return vec2(x / v.x, y / v.y); }
  vec2 operator/(const int &i) { return vec2(x / i, y / i); }
  vec2 operator*(const float &f) { return vec2(x * f, y * f); }

  int distance_x(vec2 v) { return v.x - x; }
  int distance_y(vec2 v) { return v.y - y; }
};

struct vec3 {
  float x, y, z;
};
// primitive color, its used to get 0 to 1 rgb values.
struct PCol {
  float r, g, b;
};

#define RED PCol{1, 0, 0}
#define GREEN PCol{0, 1, 0}
#define BLUE PCol{0, 0, 1}
#define WHITE PCol{1, 1, 1}
#define BLACK PCol{0, 0, 0}
#define YELLOW PCol{1, 1, 0}
#define CYAN PCol{0, 1, 1}
#define MAGENTA PCol{1, 0, 1}

struct Col {
  Uint8 r, g, b, a;

  Col() : r(0), g(0), b(0), a(0) {}
  Col(Uint8 r, Uint8 g, Uint8 b, Uint8 a) : r(r), g(g), b(b), a(a) {}

  Col without_alpha() { return Col(r, g, b, 255); }
  PCol to_pcol() { return {r / 255.f, g / 255.f, b / 255.f}; }
};

struct vec2i {
  int x, y;
};

struct vec3i {
  int x, y, z;
};
