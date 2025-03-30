#include "../../includes/GameEngine/GameEngine.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/SystemManagement/VideoManager.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

GameEngine::GameEngine() {
  /*
   * Set up the paths for the assets and configuration files.
   *
   * The assets directory contains all the images, audio, and font files used in the game.
   * The configuration directory contains the configuration file for the game.
   *
   * The configuration file is a JSON file that contains the game settings such as window size,
   * font path, and more.
   */
  const Path ASSETS_DIR_PATH  = "assets";
  const Path CONFIG_DIR_PATH  = "config";
  const Path CONFIG_FILE_PATH = CONFIG_DIR_PATH / "config.json";

  /*
   * Throw an error if the assets directory is not found.
   */
  if (!std::filesystem::exists(ASSETS_DIR_PATH)) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Assets folder not found!");
    cleanup();
    throw std::runtime_error("Assets folder not found!");
  }

  /*
   * Initialize the managers and resources needed by the game engine.
   *
   * The game engine is composed of several managers that handle different aspects of the game.
   */
  m_configManager    = createConfigManager(CONFIG_FILE_PATH);
  m_audioManager     = createAudioManager();
  m_audioSampleQueue = initializeAudioSampleQueue();
  m_fontManager      = createFontManager();
  m_videoManager     = createVideoManager();
  m_surfaceManager   = createSurfaceManager();

  /*
   * Set the game engine to running state.
   */
  m_isRunning = true;

  /*
   * Log to console that the game engine has been initialized successfully.
   *
   * Sets up the menu scene and loads it into the game engine.
   */
  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Game engine initialized successfully!");
  const std::shared_ptr<Scene> menuScene = std::make_shared<MenuScene>(this);
  loadScene("Menu", menuScene);
}

GameEngine::~GameEngine() {
  cleanup();
}

std::unique_ptr<ConfigManager> GameEngine::createConfigManager(const Path &configPath) {
  return std::make_unique<ConfigManager>(configPath);
}

std::unique_ptr<SurfaceManager> GameEngine::createSurfaceManager() {
  return std::make_unique<SurfaceManager>();
}

std::unique_ptr<VideoManager> GameEngine::createVideoManager() const {
  if (m_configManager == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "ConfigManager not initialized");
    cleanup();
    throw std::runtime_error("ConfigManager not initialized");
  }

  return std::make_unique<VideoManager>(*m_configManager);
}

std::unique_ptr<AudioManager> GameEngine::createAudioManager() {
  constexpr int    FREQUENCY = 44100;
  constexpr Uint16 FORMAT    = MIX_DEFAULT_FORMAT;
  constexpr int    CHANNELS  = 2;
  constexpr int    CHUNKSIZE = 2048;

  return std::make_unique<AudioManager>(FREQUENCY, FORMAT, CHANNELS, CHUNKSIZE);
}

std::unique_ptr<AudioSampleQueue> GameEngine::initializeAudioSampleQueue() const {
  if (m_audioManager == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager not initialized");
    cleanup();
    throw std::runtime_error("AudioManager not initialized");
  }
  return std::make_unique<AudioSampleQueue>(*m_audioManager);
}

std::unique_ptr<FontManager> GameEngine::createFontManager() const {
  if (m_configManager == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "ConfigManager not initialized");
    cleanup();
    throw std::runtime_error("ConfigManager not initialized");
  }

  const std::string &fontPath = m_configManager->getGameConfig().fontPath;
  return std::make_unique<FontManager>(fontPath);
}

void GameEngine::cleanup() {
  SDL_Quit();
  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Game engine cleaned up successfully!");
}

void GameEngine::update() {
  const std::shared_ptr<Scene> &activeScene = m_scenes[m_currentSceneName];
  if (activeScene == nullptr) {
    return;
  }

  activeScene->update();
}

bool GameEngine::isRunning() const {
  return m_isRunning;
}

void GameEngine::run() {
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(mainLoop, this, 0, 1);
#else
  while (m_isRunning) {
    mainLoop(this);
  }
#endif
}

void GameEngine::quit() {
  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Quitting game engine...");
#ifdef __EMSCRIPTEN__
  emscripten_cancel_main_loop();
#else
  m_isRunning = false;
#endif
}

void GameEngine::loadScene(const std::string &sceneName, const std::shared_ptr<Scene> &scene) {
  m_scenes[sceneName] = scene;

  scene->setStartTime(SDL_GetTicks64());
  m_currentSceneName = sceneName;
}

ConfigManager &GameEngine::getConfigManager() const {
  if (!m_configManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "ConfigManager not initialized");
    throw std::runtime_error("ConfigManager not initialized");
  }

  return *m_configManager;
}

FontManager &GameEngine::getFontManager() const {
  if (!m_fontManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "FontManager not initialized");
    throw std::runtime_error("FontManager not initialized");
  }
  return *m_fontManager;
}

AudioManager &GameEngine::getAudioManager() const {
  if (!m_audioManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "AudioManager not initialized");
    throw std::runtime_error("AudioManager not initialized");
  }
  return *m_audioManager;
}

AudioSampleQueue &GameEngine::getAudioSampleQueue() const {
  if (!m_audioSampleQueue) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "AudioSampleManager not initialized");
    throw std::runtime_error("AudioSampleManager not initialized");
  }
  return *m_audioSampleQueue;
}

VideoManager &GameEngine::getVideoManager() const {
  if (!m_videoManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "VideoManager not initialized");
    throw std::runtime_error("VideoManager not initialized");
  }
  return *m_videoManager;
}

SurfaceManager &GameEngine::getSurfaceManager() const {
  if (!m_surfaceManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "ImageManager not initialized");
    throw std::runtime_error("ImageManager not initialized");
  }
  return *m_surfaceManager;
}

void GameEngine::sUserInput() {
  SDL_Event                    event;
  const std::shared_ptr<Scene> activeScene = m_scenes[m_currentSceneName];

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      quit();
      return;
    }

    // Handle window events
    if (event.type == SDL_WINDOWEVENT) {
      switch (event.window.event) {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          m_videoManager->updateWindowSize();
          activeScene->onSceneWindowResize();
          break;
        default:
          break;
      }
    }

    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
      if (!activeScene->getActionMap().contains(event.key.keysym.sym)) {
        continue;
      }

      const ActionState actionState =
          event.type == SDL_KEYDOWN ? ActionState::START : ActionState::END;

      const std::string &actionName = activeScene->getActionMap().at(event.key.keysym.sym);
      Action             action(actionName, actionState, std::nullopt);
      activeScene->sDoAction(action);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
      if (!activeScene->getActionMap().contains(event.button.button)) {
        continue;
      }

      /*
       * If the event is a mouse button event, set the action state to start on mouse down,
       * and end on mouse up.
       */
      const ActionState actionState =
          event.type == SDL_MOUSEBUTTONDOWN ? ActionState::START : ActionState::END;

      const std::string &actionName = activeScene->getActionMap().at(event.button.button);

      // Get mouse position and convert to game coordinates
      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);

      Vec2 gamePosition = {static_cast<float>(mouseX), static_cast<float>(mouseY)};

      Action action(actionName, actionState, gamePosition);
      activeScene->sDoAction(action);
    }

    // Mouse motion handling
    if (event.type == SDL_MOUSEMOTION &&
        activeScene->getActionMap().contains(SDL_MOUSEMOTION)) {
      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);
      Vec2 gamePosition = {static_cast<float>(mouseX), static_cast<float>(mouseY)};

      const std::string &actionName = activeScene->getActionMap().at(SDL_MOUSEMOTION);
      Action             action(actionName, ActionState::START, gamePosition);
      activeScene->sDoAction(action);
    }
  }
}

void GameEngine::mainLoop(void *arg) {
  auto *gameEngine = static_cast<GameEngine *>(arg);
#ifdef __EMSCRIPTEN__
  const bool isWebCanvasEnabled = VideoManager::isWebCanvasEnabled();
  if (!isWebCanvasEnabled) {
    gameEngine->quit();
    return;
  }
#endif
  gameEngine->sUserInput();
  gameEngine->update();
}
