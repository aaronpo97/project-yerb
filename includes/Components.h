#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "./Vec2.h"

#include <SDL2/SDL.h>
#include <iostream>

class CTransform {
public:
  Vec2  topLeftCornerPos = {0, 0};
  Vec2  velocity         = {0, 0};
  float angle            = 0;

  CTransform(const Vec2 &position, const Vec2 &velocity, const float angle) :
      topLeftCornerPos(position), velocity(velocity), angle(angle) {}
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

  RGBA color;
};

class CShape {
private:
  // Renderer is only used to draw the shape and should not be directly modified
  SDL_Renderer *renderer;

public:
  SDL_Rect rect;
  RGBA     color;

  // Inject the renderer dependency
  CShape(SDL_Renderer *renderer, ShapeConfig config) :
      renderer(renderer) {

    if (renderer == nullptr) {
      throw std::runtime_error("CShape entity initialization failed: Renderer is null.");
    }

    rect.h = config.height;
    rect.w = config.width;

    color = {config.color.r, config.color.g, config.color.b, config.color.a};
    SDL_SetRenderDrawColor(renderer, config.color.r, config.color.g, config.color.b, 255);

    SDL_RenderFillRect(renderer, &rect);
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

class CLifespan {
public:
  Uint64 birthTime;
  Uint64 lifespan;

  CLifespan() :
      birthTime(SDL_GetTicks()), lifespan(0) {}
  CLifespan(Uint64 lifespan) :
      birthTime(SDL_GetTicks()), lifespan(lifespan) {}
};

enum EffectTypes { Speed, Slowness };
struct Effect {
  Uint64      startTime;
  Uint64      duration;
  EffectTypes type;
};

class CEffects {
  std::vector<Effect> effects;

public:
  CEffects() {}
  void addEffect(const Effect &effect) {

    // do not add the effect if it already exists
    for (const auto &existingEffect : effects) {
      if (existingEffect.type == effect.type) {
        return;
      }
    }

    std::string effectType;
    std::string effectEmoji;
    switch (effect.type) {
      case EffectTypes::Speed: {
        effectType  = "Speed";
        effectEmoji = "🚀";
        break;
      }
      case EffectTypes::Slowness: {
        effectType  = "Slowness";
        effectEmoji = "🐢";
        break;
      }
      default: {
        effectType  = "Unknown";
        effectEmoji = "❓";
        break;
      }
    }

    std::cout << "Effect added: " << effectType << " for " << effect.duration << "ms."
              << effectEmoji << std::endl;
    effects.push_back(effect);
  }

  const std::vector<Effect> &getEffects() {
    return effects;
  }

  void removeEffect(const EffectTypes type) {
    effects.erase(std::remove_if(effects.begin(), effects.end(),
                                 [type](const Effect &effect) { return effect.type == type; }),
                  effects.end());
  }

  const bool hasEffect(const EffectTypes type) {
    return std::find_if(effects.begin(), effects.end(), [type](const Effect &effect) {
             return effect.type == type;
           }) != effects.end();
  }
};
#endif // COMPONENTS_H
