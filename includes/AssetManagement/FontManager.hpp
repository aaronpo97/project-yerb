#pragma once

#include <SDL_ttf.h>
#include <string>

class FontManager {
private:
  std::string m_fontPath;
  TTF_Font   *m_font_lg = nullptr;
  TTF_Font   *m_font_md = nullptr;
  TTF_Font   *m_font_sm = nullptr;

public:
  explicit FontManager(const std::string &fontPath);
  ~FontManager();

  void      loadFonts(const std::string &fontPath);
  TTF_Font *getFontLg() const;
  TTF_Font *getFontMd() const;
  TTF_Font *getFontSm() const;
};
