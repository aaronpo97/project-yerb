#pragma once

#include "../Configuration/ConfigManager.hpp"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <memory>
#include <string>
class Scene; // Resolve circular dependency with forward declaration
class GameEngine {
protected:
  std::map<std::string, std::shared_ptr<Scene>> m_scenes;
  std::string                                   m_currentSceneName;
  SDL_Renderer                                 *m_renderer  = nullptr;
  SDL_Window                                   *m_window    = nullptr;
  TTF_Font                                     *m_font_lg   = nullptr;
  TTF_Font                                     *m_font_md   = nullptr;
  TTF_Font                                     *m_font_sm   = nullptr;
  bool                                          m_isRunning = false;
  ConfigManager                                 m_configManager;

  void update();

public:
  GameEngine();
  ~GameEngine();

  SDL_Renderer *getRenderer();
  SDL_Window   *getWindow();
  bool          isRunning();

  ConfigManager &getConfigManager();

  void run();
  void quit();

  void loadScene(const std::string &name, std::shared_ptr<Scene> scene);

  TTF_Font *getFontLg() {
    return m_font_lg;
  }

  TTF_Font *getFontMd() {
    return m_font_md;
  }

  TTF_Font *getFontSm() {
    return m_font_sm;
  }

  static void mainLoop(void *arg);

  void sUserInput();
};
