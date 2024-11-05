#pragma once

#include "../Configuration/ConfigManager.hpp"
#include "../Configuration/FontManager.hpp"
#include <SDL2/SDL.h>
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
  bool                                          m_isRunning = false;
  std::unique_ptr<ConfigManager>                m_configManager;
  std::unique_ptr<FontManager>                  m_fontManager;

  void        update();
  void        quit();
  static void mainLoop(void *arg);
  void        cleanup();
  void        sUserInput();

public:
  GameEngine();
  ~GameEngine();

  SDL_Renderer  *getRenderer();
  SDL_Window    *getWindow();
  bool           isRunning() const;
  void           loadScene(const std::string &name, const std::shared_ptr<Scene> &scene);
  ConfigManager &getConfigManager();
  FontManager   &getFontManager();

  void run();
};
