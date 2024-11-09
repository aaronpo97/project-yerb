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

  bool isRunning() const;
  void loadScene(const std::string &sceneName, const std::shared_ptr<Scene> &scene);

  ConfigManager &getConfigManager() const;
  FontManager   &getFontManager() const;
  SDL_Renderer  *getRenderer() const;
  SDL_Window    *getWindow() const;

  void run();
};
