#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "./Vec2.h"

#include <SDL2/SDL.h>
#include <iostream>

class CTransform {
public:
  Vec2  position = {0, 0};
  Vec2  velocity = {0, 0};
  float angle    = 0;

  CTransform(const Vec2 &position, const Vec2 &velocity, const float angle) :
      position(position), velocity(velocity), angle(angle) {}
};

class CCollision {
public:
  float radius = 0;
  CCollision(const float radius) :
      radius(radius) {}
};

struct RGBA {
  float r;
  float g;
  float b;
  float a;
};
struct ShapeConfig {
  float height;
  float width;
  RGBA  color;
};

class CShape {
public:
  SDL_Rect      rect;
  SDL_Renderer *renderer;

  CShape(SDL_Renderer *renderer, ShapeConfig config) :
      renderer(renderer) {

    if (renderer == nullptr) {
      throw std::runtime_error("CShape entity initialization failed: Renderer is null.");
    }

    rect.h = config.height;
    rect.w = config.width;
    rect.x = 100;
    rect.y = 100;

    SDL_SetRenderDrawColor(renderer, config.color.r, config.color.g, config.color.b,
                           config.color.a);

    SDL_RenderFillRect(renderer, &rect);

    std::cout << "CShape entity created" << std::endl;
  }
};

class CInput {
public:
  bool forward  = false;
  bool backward = false;
  bool left     = false;
  bool right    = false;

  CInput() {}
};
#endif // COMPONENTS_H
