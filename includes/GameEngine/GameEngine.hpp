#pragma once

#include "../AssetManagement/AudioManager.hpp"
#include "../AssetManagement/AudioSampleQueue.hpp"
#include "../AssetManagement/FontManager.hpp"
#include "../Configuration/ConfigManager.hpp"

#include <SDL2/SDL.h>
#include <filesystem>
#include <map>

#include <string>
typedef std::filesystem::path Path;
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
  std::unique_ptr<AudioSampleQueue>             m_audioSampleQueue;

  void                                  update();
  void                                  quit();
  static void                           mainLoop(void *arg);
  void                                  cleanup();
  void                                  sUserInput();
  static std::unique_ptr<ConfigManager> createConfigManager(const Path &configPath);
  std::unique_ptr<FontManager>          createFontManager();
  static std::unique_ptr<AudioManager>  createAudioManager();
  std::unique_ptr<AudioSampleQueue>     initializeAudioSampleQueue();
  void                                  initializeVideoSystem();
  SDL_Renderer                         *createRenderer();
  void                                  setupRenderer();
  SDL_Window                           *createWindow();

public:
   GameEngine();
  ~GameEngine();

  bool isRunning() const;
  void loadScene(const std::string &sceneName, const std::shared_ptr<Scene> &scene);

  ConfigManager    &getConfigManager() const;
  FontManager      &getFontManager() const;
  AudioManager     &getAudioManager() const;
  AudioSampleQueue &getAudioSampleQueue() const;
  SDL_Renderer     *getRenderer() const;
  SDL_Window       *getWindow() const;

  void run();
};
