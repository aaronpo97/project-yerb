#pragma once

#include "../AssetManagement/AudioManager.hpp"
#include "../AssetManagement/AudioSampleManager.hpp"
#include "../AssetManagement/FontManager.hpp"
#include "../Configuration/ConfigManager.hpp"

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
  std::unique_ptr<AudioManager>                 m_audioManager;
  std::unique_ptr<AudioSampleManager>           m_audioSampleManager;

  void                                  update();
  void                                  quit();
  static void                           mainLoop(void *arg);
  void                                  cleanup();
  void                                  sUserInput();
  static std::unique_ptr<ConfigManager> createConfigManager(const std::string &configPath);
  std::unique_ptr<FontManager>          createFontManager();
  static std::unique_ptr<AudioManager>  createAudioManager();
  std::unique_ptr<AudioSampleManager>   createAudioSampleManager();
  void                                  initializeVideoSystem();
  SDL_Renderer                         *createRenderer();
  void                                  setupRenderer();
  SDL_Window                           *createWindow();

public:
  GameEngine();
  ~GameEngine();

  bool isRunning() const;
  void loadScene(const std::string &sceneName, const std::shared_ptr<Scene> &scene);

  ConfigManager      &getConfigManager() const;
  FontManager        &getFontManager() const;
  AudioManager       &getAudioManager() const;
  AudioSampleManager &getAudioSampleManager() const;
  SDL_Renderer       *getRenderer() const;
  SDL_Window         *getWindow() const;

  void run();
};
