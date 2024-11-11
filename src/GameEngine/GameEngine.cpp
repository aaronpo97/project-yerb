#include "../../includes/GameEngine/GameEngine.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include <filesystem>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

GameEngine::GameEngine() {
  if (!std::filesystem::exists("./assets")) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Assets folder not found!");
    cleanup();
    throw std::runtime_error("Assets folder not found!");
  }

  const std::string CONFIG_PATH = "./assets/config.json";

  m_configManager = std::make_unique<ConfigManager>(CONFIG_PATH);
  m_audioManager  = std::make_unique<AudioManager>();

  const auto &gameConfig = m_configManager->getGameConfig();

  const std::string &FONT_PATH    = gameConfig.fontPath;
  const std::string &WINDOW_TITLE = gameConfig.windowTitle;
  const Vec2        &WINDOW_SIZE  = gameConfig.windowSize;

  m_fontManager = std::make_unique<FontManager>(FONT_PATH);

  // Create Video System
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL video system is not ready to go: %s",
                 SDL_GetError());
    cleanup();
    throw std::runtime_error("SDL video system is not ready to go");
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "SDL video system initialized successfully!");

  // Create Window
  m_window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, static_cast<int>(WINDOW_SIZE.x),
                              static_cast<int>(WINDOW_SIZE.y), SDL_WINDOW_SHOWN);
  if (m_window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Window could not be created: %s", SDL_GetError());
    cleanup();
    throw std::runtime_error("Window could not be created.");
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Window created successfully!");

  // Create Renderer
  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (m_renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Renderer could not be created: %s", SDL_GetError());
    cleanup();
    throw std::runtime_error("Renderer could not be created");
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Renderer created successfully!");

  // Setup Renderer
  constexpr SDL_Color backgroundColor = {.r = 0, .g = 0, .b = 0, .a = 255};
  SDL_SetRenderDrawColor(m_renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b,
                         backgroundColor.a);
  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderClear(m_renderer);
  SDL_RenderPresent(m_renderer);

  // Initialization successful, set running flag to true and load the first scene
  m_isRunning = true;

  SDL_LogInfo(SDL_LOG_CATEGORY_SYSTEM, "Game engine initialized successfully!");
  const std::shared_ptr<Scene> menuScene = std::make_shared<MenuScene>(this);
  loadScene("Menu", menuScene);
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

GameEngine::~GameEngine() {
  cleanup();
}

void GameEngine::update() {
  const std::shared_ptr<Scene> activeScene = m_scenes[m_currentSceneName];
  if (activeScene == nullptr) {
    return;
  }

  m_scenes[m_currentSceneName]->update();
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
