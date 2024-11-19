#pragma once

#include <SDL_ttf.h>
#include <filesystem>
#include <string>

typedef std::filesystem::path Path;

class FontManager {
private:
  Path m_fontPath;
  TTF_Font   *m_font_lg = nullptr;
  TTF_Font   *m_font_md = nullptr;
  TTF_Font   *m_font_sm = nullptr;

public:
  explicit FontManager(const Path &fontPath);
  ~        FontManager();

  void      loadFonts(const Path &fontPath);
  TTF_Font *getFontLg() const;
  TTF_Font *getFontMd() const;
  TTF_Font *getFontSm() const;
};
