#pragma once
#include "../Configuration/ConfigManager.hpp"
#include "../Helpers/Vec2.hpp"
#include <SDL2/SDL.h>

class VideoManager {
  SDL_Renderer *m_renderer = nullptr;
  SDL_Window   *m_window   = nullptr;

  Vec2           m_currentWindowSize;
  ConfigManager &m_configManager;

  /**
   * @brief Initializes the SDL video subsystem.
   *
   * This function sets up the SDL video system by calling SDL_Init with the
   * SDL_INIT_VIDEO flag.
   *
   * If the initialization fails, it logs an error message and throws a
   * std::runtime_error exception.
   *
   * On successful initialization, it logs a success message.
   *
   * @throws std::runtime_error If the SDL video subsystem fails to initialize.
   */
  static void initializeVideoSystem();

  /**
   * @brief Set up the renderer with a background color and blend mode.
   * @throws std::runtime_error if renderer is not initialized
   */
  void setupRenderer() const;

  /**
   * @brief Create a renderer
   * @throws std::runtime_error if window is not initialized
   * @throws std::runtime_error if renderer could not be created
   */
  SDL_Renderer *createRenderer() const;

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
  SDL_Window *createWindow();

  /**
   * Get the current window size.
   *
   * @returns `Vec2` The current window size as a Vec2 object (width, height).
   */
  Vec2 getWindowSize() const;

public:
  /**
   * @brief Constructor method for the VideoManager.
   *
   * This initializes the VideoManager object by initializing SDL_VIDEO and by creating an
   * SDL_Window and SDL_Renderer.
   *
   * @param configManager The ConfigManager object associated with the GameEngine class.
   */
  explicit VideoManager(ConfigManager &configManager);

  /**
   * @brief Destructor for the VideoManager.
   *
   * This calls the cleanup() method to free the SDL resources.
   */
  ~VideoManager();

  /**
   * @brief Updates the current window size.
   *
   * This function retrieves the current window size and updates the
   * m_currentWindowSize member variable. It also updates the game window
   * size in the ConfigManager.
   */
  void updateWindowSize();

  /**
   * Get a pointer to the SDL renderer.
   *
   * @returns SDL_Renderer* Pointer to the SDL renderer.
   */
  SDL_Renderer *getRenderer() const;

  /**
   * Get a pointer to the SDL window.
   *
   * @returns SDL_Window* Pointer to the SDL window.
   */
  SDL_Window *getWindow() const;

  /**
   * @brief Cleans up the SDL resources.
   *
   * This function destroys the SDL renderer and window, and quits the SDL video subsystem.
   */
  void cleanup();

#ifdef __EMSCRIPTEN__
  /**
   * @brief Check if the Web Canvas is enabled.
   *
   *  This method is only available when compiling for Emscripten.
   */
  static bool isWebCanvasEnabled();
#endif // __EMSCRIPTEN__
};