#pragma once

#include <SDL_ttf.h>
#include <iostream>
#include <string>
class FontManager {
private:
  std::string m_fontPath;
  TTF_Font   *m_font_lg = nullptr;
  TTF_Font   *m_font_md = nullptr;
  TTF_Font   *m_font_sm = nullptr;

public:
  FontManager(const std::string &fontPath) :
      m_fontPath(fontPath) {
    if (TTF_Init() != 0) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to initialize SDL_ttf: %s",
                   TTF_GetError());
      return;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "SDL_ttf initialized successfully.");
    loadFonts(fontPath);
    }

  void loadFonts(const std::string &fontPath) {
    const int SMALL_FONT_POINT_SIZE  = 14;
    const int MEDIUM_FONT_POINT_SIZE = 28;
    const int LARGE_FONT_POINT_SIZE  = 48;

    m_font_sm = TTF_OpenFont(fontPath.c_str(), SMALL_FONT_POINT_SIZE);
    m_font_md = TTF_OpenFont(fontPath.c_str(), MEDIUM_FONT_POINT_SIZE);
    m_font_lg = TTF_OpenFont(fontPath.c_str(), LARGE_FONT_POINT_SIZE);

    const bool fontsLoaded =
        (m_font_lg != nullptr && m_font_md != nullptr && m_font_sm != nullptr);

    if (!fontsLoaded) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load fonts: %s", TTF_GetError());
      return;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Fonts loaded successfully!");
  }

  TTF_Font *getFontLg() {
    return m_font_lg;
  }

  TTF_Font *getFontMd() {
    return m_font_md;
  }

  TTF_Font *getFontSm() {
    return m_font_sm;
  }

  ~FontManager() {

    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Cleaning up fonts...");

    if (m_font_md != nullptr) {
      TTF_CloseFont(m_font_md);
      m_font_md = nullptr;
    }

    if (m_font_sm != nullptr) {
      TTF_CloseFont(m_font_sm);
      m_font_sm = nullptr;
    }

    if (m_font_lg != nullptr) {
      TTF_CloseFont(m_font_lg);
      m_font_lg = nullptr;
    }

    TTF_Quit();

    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Fonts cleaned up successfully!");
  }
};