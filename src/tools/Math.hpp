#pragma once

#include "Common.hpp"

namespace Math {
double lerp(double a, double b,
            double t); // i guess the name of this is not lerp..
float prox_falloff(float a, float b,
                   float k = 1); // K = the value i want to exponentially fall
                                 // off to 0, default is 1
double move_to(double a, double b, double t);

int clamp(int min, int value, int max);
float clamp(float min, float value, float max);
int round(double value);
int floor(double value);
double sign(double value);
double abs(double value);
float fabs(float value);
double pow(double base, double exponent);
double sqrt(double value);
double angle(vec2 a, vec2 b);

template <typename T> T max(T a, T b) { return (a > b) ? b : a; }

int percentage(int x, int y);

int dist(int a, int b);
int dist_vec(vec2 a, vec2 b);

bool approx(int a, int b, int tolerance = 10);

int mid(int a, int b);
vec2 mid_vec(vec2 a, vec2 b);
void normalize(vec2 &v);
}; // namespace Math
