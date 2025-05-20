#pragma once

#include "./Vec2.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

namespace TextHelpers {
  void renderLineOfText(SDL_Renderer      *renderer,
                        TTF_Font          *font,
                        const std::string &text,
                        const SDL_Color   &color,
                        const Vec2        &position);
}
