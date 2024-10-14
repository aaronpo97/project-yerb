#include "Vec2.h"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>

#ifndef TEXT_HELPERS_H
#define TEXT_HELPERS_H
namespace TextHelpers {
  void renderLineOfText(SDL_Renderer      *renderer,
                        TTF_Font          *font,
                        const std::string &text,
                        const SDL_Color   &color,
                        const Vec2        &position);
}

#endif // TEXT_HELPERS_H