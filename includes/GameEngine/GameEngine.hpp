#pragma once

#include "../Configuration/ConfigManager.hpp"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <memory>
#include <string>
class Scene; // Resolve circular dependency
class GameEngine {
protected:
  std::map<std::string, std::shared_ptr<Scene>> m_scenes;
  std::string                                   m_currentScene;
  SDL_Renderer                                 *m_renderer   = nullptr;
  SDL_Window                                   *m_window     = nullptr;
  TTF_Font                                     *m_font_big   = nullptr;
  TTF_Font                                     *m_font_small = nullptr;
  bool                                          m_isRunning  = false;
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

  void addScene(const std::string &name, std::shared_ptr<Scene> scene);
  void switchScene(const std::string &name);

  TTF_Font *getFontBig() {
    return m_font_big;
  }

  TTF_Font *getFontSmall() {
    return m_font_small;
  }

  static void mainLoop(void *arg);

  void sUserInput();
};
