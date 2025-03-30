#include "../../includes/GameEngine/GameEngine.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/SystemManagement/VideoManager.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/**
 * Constructs the GameEngine object and initializes all necessary managers and
 * resources.
 *
 * @throws std::runtime_error if the assets directory is not found.
 */
GameEngine::GameEngine() {

  /**
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

  /**
   * Throw an error if the assets directory is not found.
   */
  if (!std::filesystem::exists(ASSETS_DIR_PATH)) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Assets folder not found!");
    cleanup();
    throw std::runtime_error("Assets folder not found!");
  }

  /**
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

  /**
   * Set the game engine to running state.
   */
  m_isRunning = true;

  /**
   * Log to console that the game engine has been initialized successfully.
   *
   * Sets up the menu scene and loads it into the game engine.
   */
  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Game engine initialized successfully!");
  const std::shared_ptr<Scene> menuScene = std::make_shared<MenuScene>(this);
  loadScene("Menu", menuScene);
}

/**
 * Destructs the GameEngine object and cleans up all resources.
 *
 * Automatic cleanup for when the game engine goes out of scope.
 */
GameEngine::~GameEngine() {
  cleanup();
}

std::unique_ptr<ConfigManager> GameEngine::createConfigManager(const Path &configPath) {
  return std::make_unique<ConfigManager>(configPath);
}

/**
 * Create an ImageManager object.
 *
 * The ImageManager is responsible for managing the images in the game engine.
 *
 * When the ImageManager is created it loads images from the assets folder and stores them in
 * memory for rendering as SDL_Surface objects. When it goes out of scope, the surfaces are
 * destroyed.
 *
 * @returns std::unique_ptr<ImageManager> The ImageManager object initialized
 */
std::unique_ptr<SurfaceManager> GameEngine::createSurfaceManager() {
  return std::make_unique<SurfaceManager>();
}

/**
 * Create a VideoManager object.
 *
 * The video manager is responsible for managing the SDL window and renderer. It uses the
 * ConfigManager object to get the window size and other related configurations.
 *
 * When the video manager is created, the window and renderer are created and initialized.
 * When it goes out of scope, the window and renderer are destroyed.
 *
 * @throws std::runtime_error if ConfigManager is not initialized
 * @returns std::unique_ptr<VideoManager> The VideoManager object initialized
 */
std::unique_ptr<VideoManager> GameEngine::createVideoManager() const {
  if (m_configManager == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "ConfigManager not initialized");
    cleanup();
    throw std::runtime_error("ConfigManager not initialized");
  }

  return std::make_unique<VideoManager>(*m_configManager);
}

/**
 * Create an AudioManager object.
 *
 * The AudioManager is responsible for managing the audio system in the game engine.
 *
 * @throws  std::runtime_error if AudioManager is not initialized
 * @returns std::unique_ptr<AudioManager> The AudioManager object initialized
 */
std::unique_ptr<AudioManager> GameEngine::createAudioManager() {
  constexpr int    FREQUENCY = 44100;
  constexpr Uint16 FORMAT    = MIX_DEFAULT_FORMAT;
  constexpr int    CHANNELS  = 2;
  constexpr int    CHUNKSIZE = 2048;

  return std::make_unique<AudioManager>(FREQUENCY, FORMAT, CHANNELS, CHUNKSIZE);
}

/**
 * Initialize the AudioSampleQueue object.
 *
 * The AudioManager must be initialized before calling this function.
 *
 * The AudioSampleQueue's role in the game engine is to manage the queue of audio samples to be
 * played. It uses the AudioManager to play the audio samples.
 *
 * Used by the audio system in the `Scene` objects.
 *
 * @throws std::runtime_error if AudioManager is not initialized
 * @returns The AudioSampleQueue object initialized
 */
std::unique_ptr<AudioSampleQueue> GameEngine::initializeAudioSampleQueue() {
  if (m_audioManager == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager not initialized");
    cleanup();
    throw std::runtime_error("AudioManager not initialized");
  }
  return std::make_unique<AudioSampleQueue>(*m_audioManager);
}

/**
 * Create a FontManager object. Used by the rendering system in `Scene` objects.
 *
 * @throws std::runtime_error if ConfigManager is not initialized
 * @returns The FontManager object initializedSS
 */
std::unique_ptr<FontManager> GameEngine::createFontManager() {
  if (m_configManager == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "ConfigManager not initialized");
    cleanup();
    throw std::runtime_error("ConfigManager not initialized");
  }

  const std::string &fontPath = m_configManager->getGameConfig().fontPath;
  return std::make_unique<FontManager>(fontPath);
}

/**
 * Calls SDL_Quit to release all SDL resources.
 *
 * Called when the game engine is destructed.
 */
void GameEngine::cleanup() {
  SDL_Quit();
  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Game engine cleaned up successfully!");
}

/**
 * Calls the active scene's update method.
 *
 * This is used in the main loop to update on each frame.
 */
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

/**
 * This is the game engine's run method that is called by the C++ main function.
 *
 * Responsible for running the game engine main loop function.
 *
 * If the game engine is running in a web browser, it uses the emscripten main loop. Otherwise
 * it simply uses a while loop.
 */
void GameEngine::run() {
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(mainLoop, this, 0, 1);
#else
  while (m_isRunning) {
    mainLoop(this);
  }
#endif
}

/**
 * Quits the game engine by either stopping the main loop or canceling
 * the emscripten main loop.
 *
 * Called when the game engine is to be stopped.
 */
void GameEngine::quit() {
  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Quitting game engine...");
#ifdef __EMSCRIPTEN__
  emscripten_cancel_main_loop();
#else
  m_isRunning = false;
#endif
}

/**
 * Loads a scene into the game engine.
 *
 * @param sceneName The name of the scene to load
 * @param scene A shared pointer to the scene object to load
 */
void GameEngine::loadScene(const std::string &sceneName, const std::shared_ptr<Scene> &scene) {
  m_scenes[sceneName] = scene;

  scene->setStartTime(SDL_GetTicks64());
  m_currentSceneName = sceneName;
}

/**
 * Retrieves the ConfigManager instance associated with the game engine.
 *
 * @throws std::runtime_error if ConfigManager is not initialized.
 * @returns A reference to the initialized ConfigManager object.
 */
ConfigManager &GameEngine::getConfigManager() const {
  if (!m_configManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "ConfigManager not initialized");
    throw std::runtime_error("ConfigManager not initialized");
  }

  return *m_configManager;
}

/**
 * Retrieves the FontManager instance associated with the game engine.
 *
 * @throws std::runtime_error if FontManager is not initialized.
 * @returns A reference to the initialized FontManager object.
 */
FontManager &GameEngine::getFontManager() const {
  if (!m_fontManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "FontManager not initialized");
    throw std::runtime_error("FontManager not initialized");
  }
  return *m_fontManager;
}

/**
 * Retrieves the AudioManager instance associated with the game engine.
 *
 * @throws std::runtime_error if AudioManager is not initialized.
 * @returns A reference to the initialized AudioManager object.
 */
AudioManager &GameEngine::getAudioManager() const {
  if (!m_audioManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "AudioManager not initialized");
    throw std::runtime_error("AudioManager not initialized");
  }
  return *m_audioManager;
}

/**
 * Retrieves the AudioSampleQueue instance associated with the game engine.
 *
 * @throws std::runtime_error if AudioSampleQueue is not initialized.
 * @returns A reference to the initialized AudioSampleQueue object.
 */
AudioSampleQueue &GameEngine::getAudioSampleQueue() const {
  if (!m_audioSampleQueue) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "AudioSampleManager not initialized");
    throw std::runtime_error("AudioSampleManager not initialized");
  }
  return *m_audioSampleQueue;
}

/**
 * Retrieves the VideoManager instance associated with the game engine.
 *
 * @throws std::runtime_error if VideoManager is not initialized.
 * @returns A reference to the initialized VideoManager object.
 */
VideoManager &GameEngine::getVideoManager() const {
  if (!m_videoManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "VideoManager not initialized");
    throw std::runtime_error("VideoManager not initialized");
  }
  return *m_videoManager;
}

/**
 * Retrieves the ImageManager instance associated with the game engine.
 *
 * @throws std::runtime_error if ImageManager is not initialized.
 * @returns A reference to the initialized ImageManager object.
 */
SurfaceManager &GameEngine::getSurfaceManager() const {
  if (!m_surfaceManager) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "ImageManager not initialized");
    throw std::runtime_error("ImageManager not initialized");
  }
  return *m_surfaceManager;
}

/**
 * This is the method used by the game engine to manage the user input system.
 *
 * The game engine listens for SDL events such as key presses, mouse clicks, and window events.
 *
 * If the current active scene uses the event as a Scene Action, the action is triggered.
 *
 */
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

      /**
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

/**
 * The main loop function that is called by the game engine.
 *
 * This function is responsible for calling the user input system and updating the game engine.
 *
 * @param arg A pointer to the GameEngine object
 */
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
