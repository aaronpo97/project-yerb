#pragma once

#include "../AssetManagement/AudioSampleQueue.hpp"
#include "../AssetManagement/FontManager.hpp"
#include "../AssetManagement/ImageManager.hpp"
#include "../Configuration/ConfigManager.hpp"
#include "../SystemManagement/AudioManager.hpp"
#include "../SystemManagement/VideoManager.hpp"

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
  bool                                          m_isRunning = false;
  std::unique_ptr<ConfigManager>                m_configManager;
  std::unique_ptr<FontManager>                  m_fontManager;
  std::unique_ptr<AudioManager>                 m_audioManager;
  std::unique_ptr<ImageManager>                 m_imageManager;
  std::unique_ptr<AudioSampleQueue>             m_audioSampleQueue;
  std::unique_ptr<VideoManager>                 m_videoManager;

  void update();

  static void mainLoop(void *arg);
  static void cleanup();

  void sUserInput();

  static std::unique_ptr<ConfigManager> createConfigManager(const Path &configPath);
  static std::unique_ptr<AudioManager>  createAudioManager();
  static std::unique_ptr<ImageManager>  createImageManager();
  std::unique_ptr<VideoManager>         createVideoManager() const;
  std::unique_ptr<FontManager>          createFontManager();
  std::unique_ptr<AudioSampleQueue>     initializeAudioSampleQueue();

public:
       GameEngine();
  ~    GameEngine();
  void quit();
  bool isRunning() const;
  void loadScene(const std::string &sceneName, const std::shared_ptr<Scene> &scene);

  ConfigManager    &getConfigManager() const;
  FontManager      &getFontManager() const;
  AudioManager     &getAudioManager() const;
  AudioSampleQueue &getAudioSampleQueue() const;
  VideoManager     &getVideoManager() const;
  ImageManager     &getImageManager() const;

  void run();
};
