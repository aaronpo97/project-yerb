#include "../../includes/SystemManagement/VideoManager.hpp"
#include "../../includes/Configuration/ConfigManager.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <stdexcept>

typedef std::filesystem::path Path;

VideoManager::VideoManager(ConfigManager &configManager) :
    m_configManager(configManager) {
  const auto &[windowSize, windowTitle, fontPath, spawnInterval] =
      m_configManager.getGameConfig();

  initializeVideoSystem();
  m_window   = createWindow();
  m_renderer = createRenderer();

  setupRenderer();
}

void VideoManager::initializeVideoSystem() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL video system is not ready: %s", SDL_GetError());
    throw std::runtime_error("SDL video system is not ready to go");
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "SDL video system initialized successfully!");
}

/**
 * @brief Creates an SDL window with specified configurations.
 *
 * Initializes and creates an SDL window using configuration parameters from ConfigManager.
 * Sets various window flags based on the platform and configuration, and handles errors if
 * window creation fails. Calculates the display scale factor for High-DPI (Retina) displays.
 *
 * @return SDL_Window* Pointer to the created SDL window.
 * @throws std::runtime_error If the window could not be created.
 */
SDL_Window *VideoManager::createWindow() {
  Uint32 windowFlags = 0;

  constexpr Uint32 baseFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
  constexpr Uint32 macFlags  = SDL_WINDOW_FULLSCREEN_DESKTOP;
  windowFlags |= baseFlags;

#ifdef __APPLE__
  windowFlags |= macFlags;
#endif

  const auto &[windowSize, windowTitle, fontPath, spawnInterval] =
      m_configManager.getGameConfig();

  SDL_Window *window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, static_cast<int>(windowSize.x),
                                        static_cast<int>(windowSize.y), windowFlags);

  if (window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Window could not be created: %s", SDL_GetError());
    throw std::runtime_error("Window could not be created.");
  }

  int currentWindowWidth, currentWindowHeight;
  int drawableWidth, drawableHeight;

  SDL_GetWindowSize(window, &currentWindowWidth, &currentWindowHeight);
  SDL_GL_GetDrawableSize(window, &drawableWidth, &drawableHeight);

  m_currentWindowSize = {static_cast<float>(currentWindowWidth),
                         static_cast<float>(currentWindowHeight)};

  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Window created successfully!");
  return window;
}

/**
 * @brief Create a renderer
 * @throws std::runtime_error if window is not initialized
 * @throws std::runtime_error if renderer could not be created
 */
SDL_Renderer *VideoManager::createRenderer() const {
  if (m_window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Window is not initialized");
    throw std::runtime_error("Window is not initialized");
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Renderer could not be created: %s", SDL_GetError());
    throw std::runtime_error("Renderer could not be created");
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Renderer created successfully!");
  return renderer;
}

/**
 * @brief Set up the renderer with a background color and blend mode.
 * @throws std::runtime_error if renderer is not initialized
 */
void VideoManager::setupRenderer() const {
  if (m_renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Renderer is not initialized");
    throw std::runtime_error("Renderer is not initialized");
  }

  constexpr SDL_Color backgroundColor = {.r = 0, .g = 0, .b = 0, .a = 255};
  SDL_SetRenderDrawColor(m_renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b,
                         backgroundColor.a);
  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderClear(m_renderer);
  SDL_RenderPresent(m_renderer);
}

Vec2 VideoManager::getWindowSize() const {
  return m_currentWindowSize;
}
SDL_Renderer *VideoManager::getRenderer() const {
  return m_renderer;
}

SDL_Window *VideoManager::getWindow() const {
  return m_window;
}

void VideoManager::cleanup() {
  if (m_renderer != nullptr) {
    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;
    SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Renderer destroyed successfully!");
  }

  if (m_window != nullptr) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "Window destroyed successfully!");
  }

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "VideoManager cleaned up successfully!");
}

VideoManager::~VideoManager() {
  cleanup();
}