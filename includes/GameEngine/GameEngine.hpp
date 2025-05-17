#pragma once

#include "../AssetManagement/AudioSampleQueue.hpp"
#include "../AssetManagement/FontManager.hpp"
#include "../AssetManagement/TextureManager.hpp"
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
  std::unique_ptr<TextureManager>               m_texture_manager;
  std::unique_ptr<AudioSampleQueue>             m_audioSampleQueue;
  std::unique_ptr<VideoManager>                 m_videoManager;

  /**
   * Calls the active scene's update method.
   *
   * This is used in the main loop to update on each frame.
   */
  void update();

  /**
   * The main loop function that is called by the game engine.
   *
   * This function is responsible for calling the user input system and updating the game
   * engine.
   *
   * @param arg A pointer to the GameEngine object
   */
  static void mainLoop(void *arg);

  /**
   * Calls SDL_Quit to release all SDL resources.
   *
   * Called when the game engine is destructed.
   */
  static void cleanup();

  /**
   * This is the method used by the game engine to manage the user input system.
   *
   * The game engine listens for SDL events such as key presses, mouse clicks, and window
   * events.
   *
   * If the current active scene uses the event as a Scene Action, the action is triggered.
   *
   */
  void sUserInput();

  /**
   * Create a ConfigManager object.
   *
   *  The ConfigManager is responsible for managing the game engine's configuration.
   */
  static std::unique_ptr<ConfigManager> createConfigManager(const Path &configPath);

  /**
   * Create the TextureManager object.
   *
   * The TextureManager is responsible the allocated memory for textures and surfaces..
   *
   * When the TextureManager is created, it loads all surfaces from the assets folder and
   * stores them in memory and generates textures from the surfaces.
   *
   * When it goes out of scope, the textures and surfaces are automatically deleted.
   *
   * @returns std::unique_ptr<TextureManager> The initialized TextureManager object.
   */
  std::unique_ptr<TextureManager> createTextureManager() const;

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
  std::unique_ptr<VideoManager> createVideoManager() const;

  /**
   * Create an AudioManager object.
   *
   * The AudioManager is responsible for managing the audio system in the game engine.
   *
   * @throws  std::runtime_error if AudioManager is not initialized
   * @returns std::unique_ptr<AudioManager> The AudioManager object initialized
   */
  static std::unique_ptr<AudioManager> createAudioManager();

  /**
   * Create a FontManager object. Used by the rendering system in `Scene` objects.
   *
   * @throws std::runtime_error if ConfigManager is not initialized
   * @returns The FontManager object initialized
   */
  std::unique_ptr<FontManager> createFontManager() const;

  /**
   * Initialize the AudioSampleQueue object.
   *
   * The AudioManager must be initialized before calling this function.
   *
   * The AudioSampleQueue's role in the game engine is to manage the queue of audio samples
   * to be played. It uses the AudioManager to play the audio samples.
   *
   * Used by the audio system in the `Scene` objects.
   *
   * @throws std::runtime_error if AudioManager is not initialized
   * @returns The AudioSampleQueue object initialized
   */
  std::unique_ptr<AudioSampleQueue> initializeAudioSampleQueue() const;

public:
  /**
   * Constructs the GameEngine object and initializes all necessary managers and
   * resources.
   *
   * @throws std::runtime_error if the assets directory is not found.
   */
  GameEngine();

  /**
   * Destroys the GameEngine object and cleans up all resources.
   *
   * Automatic cleanup for when the game engine goes out of scope.
   */
  ~GameEngine();

  /**
   * Checks if the game engine is running.
   *
   * @returns true if the game engine is running, false otherwise.
   */
  bool isRunning() const;

  /**
   * Loads a scene into the game engine.
   *
   * @param sceneName The name of the scene to load
   * @param scene A shared pointer to the scene object to load
   */
  void loadScene(const std::string &sceneName, const std::shared_ptr<Scene> &scene);

  /**
   * Retrieves the ConfigManager instance associated with the game engine.
   *
   * @throws std::runtime_error if ConfigManager is not initialized.
   * @returns A reference to the initialized ConfigManager object.
   */
  ConfigManager &getConfigManager() const;

  /**
   * Retrieves the FontManager instance associated with the game engine.
   *
   * @throws std::runtime_error if FontManager is not initialized.
   * @returns A reference to the initialized FontManager object.
   */
  FontManager &getFontManager() const;

  /**
   * Retrieves the AudioManager instance associated with the game engine.
   *
   * @throws std::runtime_error if AudioManager is not initialized.
   * @returns A reference to the initialized AudioManager object.
   */
  AudioManager &getAudioManager() const;

  /**
   * Retrieves the AudioSampleQueue instance associated with the game engine.
   *
   * @throws std::runtime_error if AudioSampleQueue is not initialized.
   * @returns A reference to the initialized AudioSampleQueue object.
   */
  AudioSampleQueue &getAudioSampleQueue() const;

  /**
   * Retrieves the VideoManager instance associated with the game engine.
   *
   * @throws std::runtime_error if VideoManager is not initialized.
   * @returns A reference to the initialized VideoManager object.
   */
  VideoManager &getVideoManager() const;

  /**
   * Retrieves the TextureManager instance associated with the game engine.
   *
   * @throws std::runtime_error if TextureManager is not initialized.
   * @returns A reference to the initialized TextureManager object.
   */
  TextureManager &getTextureManager() const;

  /**
   * This is the game engine's run method that is called by the C++ main function.
   *
   * Responsible for running the game engine main loop function.
   *
   * If the game engine is running in a web browser, it uses the emscripten main loop.
   * Otherwise it simply uses a while loop.
   */
  void run();

  /**
   * Quits the game engine by either stopping the main loop or canceling
   * the emscripten main loop.
   *
   * Called when the game engine is to be stopped.
   */
  void quit();
};
