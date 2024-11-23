#pragma once
#include "../Configuration/ConfigManager.hpp"
#include "../Helpers/Vec2.hpp"
#include <SDL2/SDL.h>

class VideoManager {
private:
  SDL_Renderer *m_renderer = nullptr;
  SDL_Window   *m_window   = nullptr;

  Vec2           m_currentWindowSize;
  ConfigManager &m_configManager;

  static void   initializeVideoSystem();
  void          setupRenderer() const;
  SDL_Renderer *createRenderer() const;
  SDL_Window   *createWindow();
  Vec2          getWindowSize() const;

public:
  explicit VideoManager(ConfigManager &configManager);

  ~VideoManager();

  SDL_Renderer *getRenderer() const;
  SDL_Window   *getWindow() const;
  void          cleanup();
};