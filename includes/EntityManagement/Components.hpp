#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

#include "../Configuration/Config.hpp"
#include "../Helpers/Vec2.hpp"

class CTransform {
public:
  Vec2  topLeftCornerPos = {0, 0};
  Vec2  velocity         = {0, 0};
  float angle            = 0;

  CTransform(const Vec2 &position, const Vec2 &velocity, const float angle) :
      topLeftCornerPos(position), velocity(velocity), angle(angle) {}

  CTransform() = default;
};

class CShape {
private:
  SDL_Renderer *renderer;

public:
  SDL_Rect  rect;
  SDL_Color color;

  CShape(SDL_Renderer *renderer, ShapeConfig config) :
      renderer(renderer) {

    if (renderer == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "CShape entity initialization failed: Renderer is null.");
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
      birthTime(SDL_GetTicks64()), lifespan(0) {}
  CLifespan(Uint64 lifespan) :
      birthTime(SDL_GetTicks64()), lifespan(lifespan) {}
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

    effects.push_back(effect);
  }

  const std::vector<Effect> &getEffects() const {
    return effects;
  }

  void removeEffect(const EffectTypes type) {
    effects.erase(std::remove_if(effects.begin(), effects.end(),
                                 [type](const Effect &effect) { return effect.type == type; }),
                  effects.end());
  }

  const bool hasEffect(const EffectTypes type) const {
    return std::find_if(effects.begin(), effects.end(), [type](const Effect &effect) {
             return effect.type == type;
           }) != effects.end();
  }

  void clearEffects() {
    effects.clear();
  }
};
