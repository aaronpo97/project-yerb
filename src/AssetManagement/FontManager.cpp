#include "../../includes/AssetManagement/FontManager.hpp"

FontManager::FontManager(const Path &fontPath) :
    m_fontPath(fontPath) {
  if (TTF_Init() != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to initialize SDL_ttf: %s", TTF_GetError());
    return;
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "SDL_ttf initialized successfully.");
  loadFonts(fontPath);
}

void FontManager::loadFonts(const Path &fontPath) {
  constexpr int SMALL_FONT_POINT_SIZE  = 18;
  constexpr int MEDIUM_FONT_POINT_SIZE = 28;
  constexpr int LARGE_FONT_POINT_SIZE  = 48;

  m_font_sm = TTF_OpenFont(fontPath.c_str(), SMALL_FONT_POINT_SIZE);
  m_font_md = TTF_OpenFont(fontPath.c_str(), MEDIUM_FONT_POINT_SIZE);
  m_font_lg = TTF_OpenFont(fontPath.c_str(), LARGE_FONT_POINT_SIZE);

  const bool fontsLoaded =
      m_font_lg != nullptr && m_font_md != nullptr && m_font_sm != nullptr;

  if (!fontsLoaded) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load fonts: %s", TTF_GetError());
    return;
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Fonts loaded successfully!");
}

TTF_Font *FontManager::getFontLg() const {
  return m_font_lg;
}

TTF_Font *FontManager::getFontMd() const {
  return m_font_md;
}

TTF_Font *FontManager::getFontSm() const {
  return m_font_sm;
}

FontManager::~FontManager() {
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
