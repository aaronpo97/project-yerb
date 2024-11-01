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
  explicit FontManager(const std::string &fontPath) :
      m_fontPath(fontPath) {
    if (TTF_Init() != 0) {
      std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
      return;
    }

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
      std::cerr << "Failed to load fonts: " << TTF_GetError() << std::endl;
      return;
    }

    std::cout << "Fonts loaded successfully!" << std::endl;
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

    std::cout << "Cleaning up fonts..." << std::endl;

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

    std::cout << "Fonts cleaned up!" << std::endl;
  }
};