#pragma once

#include <SDL_ttf.h>
#include <filesystem>
#include <string>

typedef std::filesystem::path Path;

class FontManager {
  Path m_fontPath;

  int m_fontSizeSm;
  int m_fontSizeMd;
  int m_fontSizeLg;

  TTF_Font *m_fontLg = nullptr;
  TTF_Font *m_fontMd = nullptr;
  TTF_Font *m_fontSm = nullptr;

public:
  ~FontManager();

  FontManager(const Path &fontPath, int fontSizeSm, int fontSizeMd, int fontSizeLg);
  void      loadFonts(const Path &fontPath);
  TTF_Font *getFontLg() const;
  TTF_Font *getFontMd() const;
  TTF_Font *getFontSm() const;
};
