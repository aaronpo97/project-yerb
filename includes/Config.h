#ifndef CONFIG_H
#define CONFIG_H

#include "./Vec2.h"
#include <SDL.h>
#include <string>

class ShapeConfig {
public:
  float     height;
  float     width;
  SDL_Color color;

  // Constructor
  ShapeConfig(float h = 0.0f, float w = 0.0f, SDL_Color c = {0, 0, 0, 255}) :
      height(h), width(w), color(c) {}
};

class GameConfig {
public:
  Vec2        windowSize;
  std::string windowTitle;
  std::string fontPath;

  // Constructor
  GameConfig(Vec2        size  = Vec2(800, 600),
             std::string title = "Game",
             std::string font  = "font.ttf") :
      windowSize(size), windowTitle(title), fontPath(font) {}
};

class PlayerConfig {
public:
  float       baseSpeed;
  float       speedBoostMultiplier = 1.0f;
  float       slownessMultiplier   = 1.0f;
  ShapeConfig shape;

  // Constructor
  PlayerConfig(float s = 5.0f, ShapeConfig sh = ShapeConfig()) :
      baseSpeed(s), shape(sh) {}
};

class EnemyConfig {
public:
  float       speed;
  ShapeConfig shape;

  // Constructor
  EnemyConfig(float s = 3.0f, ShapeConfig sh = ShapeConfig()) :
      speed(s), shape(sh) {}
};

#endif // CONFIG_H