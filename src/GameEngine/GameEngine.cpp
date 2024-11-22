#include "../../includes/GameEngine/GameEngine.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include <filesystem>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

GameEngine::GameEngine() {
  const Path ASSETS_DIR_PATH  = "assets";
  const Path CONFIG_DIR_PATH  = "config";
  const Path CONFIG_FILE_PATH = CONFIG_DIR_PATH / "config.json";

  if (!std::filesystem::exists(ASSETS_DIR_PATH)) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Assets folder not found!");
    cleanup();
    throw std::runtime_error("Assets folder not found!");
  }

  m_configManager    = createConfigManager(CONFIG_FILE_PATH);
  m_audioManager     = createAudioManager();
  m_audioSampleQueue = initializeAudioSampleQueue();
  m_fontManager      = createFontManager();
  initializeVideoSystem();
  m_window   = createWindow();
  m_renderer = createRenderer();
  setupRenderer();

  m_isRunning = true;

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

std::unique_ptr<AudioManager> GameEngine::createAudioManager() {
  constexpr int    FREQUENCY = 44100;
  constexpr Uint16 FORMAT    = MIX_DEFAULT_FORMAT;
  constexpr int    CHANNELS  = 2;
  constexpr int    CHUNKSIZE = 2048;

  return std::make_unique<AudioManager>(FREQUENCY, FORMAT, CHANNELS, CHUNKSIZE);
}

std::unique_ptr<AudioSampleQueue> GameEngine::initializeAudioSampleQueue() {
  if (m_audioManager == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager not initialized");
    cleanup();
    throw std::runtime_error("AudioManager not initialized");
  }
  return std::make_unique<AudioSampleQueue>(*m_audioManager);
}

/**
 * @brief Create a FontManager object
 * @throws std::runtime_error if ConfigManager is not initialized
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
 * @brief Initialize the video system
 * @throws std::runtime_error if SDL video system could not be initialized
 */
void GameEngine::initializeVideoSystem() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL video system is not ready: %s", SDL_GetError());
    cleanup();
    throw std::runtime_error("SDL video system is not ready to go");
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "SDL video system initialized successfully!");
}

/**
 * @brief Create a window
 * @throws std::runtime_error if ConfigManager is not initialized
 * @throws std::runtime_error if window could not be created
 */
SDL_Window *GameEngine::createWindow() {
  if (m_configManager == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "ConfigManager not initialized");
    cleanup();
    throw std::runtime_error("ConfigManager not initialized");
  }

  const auto &gameConfig = m_configManager->getGameConfig();

  const auto WINDOW_TITLE  = gameConfig.windowTitle.c_str();
  const auto WINDOW_WIDTH  = static_cast<int>(gameConfig.windowSize.x);
  const auto WINDOW_HEIGHT = static_cast<int>(gameConfig.windowSize.y);

  SDL_Window *window =
      SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Window could not be created: %s", SDL_GetError());
    cleanup();
    throw std::runtime_error("Window could not be created.");
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Window created successfully!");
  return window;
}

/**
 * @brief Create a renderer
 * @throws std::runtime_error if window is not initialized
 * @throws std::runtime_error if renderer could not be created
 */
SDL_Renderer *GameEngine::createRenderer() {
  if (m_window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Window is not initialized");
    cleanup();
    throw std::runtime_error("Window is not initialized");
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Renderer could not be created: %s", SDL_GetError());
    cleanup();
    throw std::runtime_error("Renderer could not be created");
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Renderer created successfully!");
  return renderer;
}

/**
 * @brief Set up the renderer
 * @throws std::runtime_error if renderer is not initialized
 */
void GameEngine::setupRenderer() {
  if (m_renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Renderer is not initialized");
    cleanup();
    throw std::runtime_error("Renderer is not initialized");
  }

  constexpr SDL_Color backgroundColor = {.r = 0, .g = 0, .b = 0, .a = 255};
  SDL_SetRenderDrawColor(m_renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b,
                         backgroundColor.a);
  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderClear(m_renderer);
  SDL_RenderPresent(m_renderer);
}

void GameEngine::cleanup() {
  if (m_renderer != nullptr) {
    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Renderer cleaned up successfully!");
  }
  if (m_window != nullptr) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Window cleaned up successfully!");
  }

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

SDL_Renderer *GameEngine::getRenderer() const {
  return m_renderer;
}

SDL_Window *GameEngine::getWindow() const {
  return m_window;
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
  m_isRunning = false;
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

void GameEngine::sUserInput() {
  SDL_Event event;

  const std::shared_ptr<Scene> activeScene = m_scenes[m_currentSceneName];

  while (!!SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      quit();
      return;
    }
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
      // If current scene doesn't have an action associated with this key, skip it
      if (!activeScene->getActionMap().contains(event.key.keysym.sym)) {
        continue;
      }

      // Determine start or end action by whether key was pressed or released
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

      const ActionState actionState =
          event.type == SDL_MOUSEBUTTONDOWN ? ActionState::START : ActionState::END;

      const std::string &actionName = activeScene->getActionMap().at(event.button.button);

      const Vec2 mousePosition = {static_cast<float>(event.button.x),
                                  static_cast<float>(event.button.y)};

      Action action(actionName, actionState, mousePosition);
      activeScene->sDoAction(action);
    }
  }
}

void GameEngine::mainLoop(void *arg) {
  auto *gameEngine = static_cast<GameEngine *>(arg);
  gameEngine->sUserInput();
  gameEngine->update();
}
