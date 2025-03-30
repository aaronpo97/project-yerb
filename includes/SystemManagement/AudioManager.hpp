#pragma once

#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <filesystem>
#include <map>
#include <unordered_map>

typedef std::filesystem::path Path;

namespace AudioPath {
  const Path ASSETS  = "assets/audio";
  const Path TRACKS  = ASSETS / "tracks";
  const Path SAMPLES = ASSETS / "samples";

  const Path MAIN_MENU = TRACKS / "main_menu.ogg";
  const Path PLAY      = TRACKS / "play.ogg";

  const Path ITEM_ACQUIRED   = SAMPLES / "item_acquired.wav";
  const Path SPEED_BOOST     = SAMPLES / "speed_boost_acquired.wav";
  const Path ENEMY_COLLISION = SAMPLES / "enemy_collides.wav";
  const Path MENU_MOVE       = SAMPLES / "menu_move.wav";
  const Path MENU_SELECT     = SAMPLES / "menu_select.wav";
  const Path SHOOT           = SAMPLES / "shoot.wav";
  const Path BULLET_HIT_01   = SAMPLES / "bullet_hit_01.wav";
  const Path BULLET_HIT_02   = SAMPLES / "bullet_hit_02.wav";
  const Path SLOWNESS_DEBUFF = SAMPLES / "slowness_debuff_acquired.wav";
} // namespace AudioPath

enum class AudioSample {
  NONE,
  ITEM_ACQUIRED,
  ENEMY_COLLISION,
  SPEED_BOOST,
  SLOWNESS_DEBUFF,
  MENU_MOVE,
  MENU_SELECT,
  SHOOT,
  BULLET_HIT_01,
  BULLET_HIT_02,
};

enum class AudioTrack {
  NONE,
  MAIN_MENU,
  PLAY,
};

class AudioManager {
private:
  std::unordered_map<AudioTrack, Mix_Music *>  m_audioTracks;
  std::unordered_map<AudioSample, Mix_Chunk *> m_audioSamples;

  int    m_frequency = 0;
  Uint16 m_format    = 0;
  int    m_channels  = 0;
  int    m_chunksize = 0;

  AudioTrack  m_currentAudioTrack = AudioTrack::NONE;
  AudioTrack  m_lastAudioTrack    = AudioTrack::NONE;
  AudioSample m_lastAudioSample   = AudioSample::NONE;
  bool        m_audioTrackPaused  = false;

  bool                       m_tracksMuted      = false;
  bool                       m_samplesMuted     = false;
  int                        m_savedTrackVolume = MIX_MAX_VOLUME;
  std::map<AudioSample, int> m_savedSampleVolumes;

  /**
   * Loads an audio track from a file.
   *
   * @param track The audio track to load.
   * @param filepath The path to the audio file.
   *
   * @throws std::runtime_error if Mix_LoadMUS fails.
   */
  void loadTrack(AudioTrack track, const Path &filepath);

  /**
   * Loads an audio sample from a file.
   *
   * @param sample The audio sample to load.
   * @param filepath The path to the audio file.
   *
   * @throws std::runtime_error if Mix_LoadWAV fails.
   */
  void loadSample(AudioSample sample, const Path &filepath);

  /**
   * Cleans up the audio resources and closes the audio subsystem.
   *
   * This function frees all loaded audio tracks and samples, stops any
   * currently playing track, and closes the audio subsystem.
   */
  void cleanup();

public:
  /**
   *
   * Constructor for AudioManager.
   *
   * Initializes SDL audio subsystem and loads all audio tracks and samples.
   *
   * @throws std::runtime_error if SDL_Init(SDL_INIT_AUDIO) or Mix_OpenAudio fails.
   *
   * @param frequency The frequency to playback audio at (in Hz).
   * @param format Audio format, one of SDL's AUDIO_* values.
   * @param channels number of channels.
   * @param chunksize Audio buffer size in sample FRAMES (total samples divided by channel
   * count).
   */
  explicit AudioManager(int    frequency = 44100,
                        Uint16 format    = MIX_DEFAULT_FORMAT,
                        int    channels  = 2,
                        int    chunksize = 2048);

  static constexpr size_t MAX_SAMPLES_PER_FRAME = 4;
  static constexpr int    DEFAULT_SAMPLE_VOLUME = MIX_MAX_VOLUME / MAX_SAMPLES_PER_FRAME;
  static constexpr int    DEFAULT_TRACK_VOLUME  = MIX_MAX_VOLUME * 0.8;

  /**
   * Destructor for AudioManager.
   *
   * Calls cleanup() to free audio resources and close the audio subsystem.
   */
  ~AudioManager();

  /**
   * Loads all audio tracks and samples.
   *
   * This function loads all audio tracks and samples into the AudioManager.
   * It uses the loadTrack() and loadSample() methods to load each track and sample.
   */
  void loadAllAudio();

  /**
   * Plays an audio track.
   *
   * If the track is already playing, it will be ignored.
   *
   * @param track The audio track to play.
   * @param loops The number of times to loop the track (-1 for infinite).
   */
  void playTrack(AudioTrack track, int loops = -1);

  /**
   * Plays an audio sample.
   *
   * @param sample The audio sample to play.
   * @param loops The number of times to loop the sample (0 for no loop).
   */
  void playSample(AudioSample sample, int loops = 0);

  /**
   * Stops the currently playing audio track.
   *
   * This function stops the currently playing audio track and sets the current
   * audio track to NONE.
   */
  void stopTrack();

  /**
   * Pauses the currently playing audio track.
   *
   * This function pauses the currently playing audio track and sets the
   * m_audioTrackPaused flag to true.
   */
  void pauseTrack();

  /**
   * Resumes the currently paused audio track.
   *
   * This function resumes the currently paused audio track and sets the
   * m_audioTrackPaused flag to false.
   */
  void resumeTrack();

  /**
   * Sets the volume of the currently playing audio track.
   *
   * @param volume The volume level (0 to MIX_MAX_VOLUME).
   */
  static void setTrackVolume(int volume);

  /**
   * Sets the volume of a specific audio sample.
   *
   * @param sampleTag The audio sample to set the volume for.
   * @param volume The volume level (0 to MIX_MAX_VOLUME).
   */
  void setSampleVolume(AudioSample sampleTag, int volume);

  /**
   * Gets the volume of a specific audio sample.
   *
   * @param sampleTag The audio sample to get the volume for.
   * @return The volume level (0 to MIX_MAX_VOLUME).
   */
  int getSampleVolume(AudioSample sampleTag);

  /**
   * Gets the volume of the currently playing audio track.
   *
   * @return The volume level (0 to MIX_MAX_VOLUME).
   */
  static int getTrackVolume();

  /**
   * Mutes all audio tracks.
   *
   * This function sets the volume of all audio tracks to 0 and stores the
   * current volume level for later restoration.
   */
  void muteTracks();

  /**
   * Unmutes all audio tracks.
   *
   * This function restores the volume of all audio tracks to their previous
   * level.
   *
   *
   */
  void unmuteTracks();

  /**
   * Mutes all audio samples.
   *
   * This function sets the volume of all audio samples to 0 and stores the
   * current volume level for later restoration.
   */
  void muteSamples();

  /**
   * Unmutes all audio samples.
   *
   * This function restores the volume of all audio samples to their previous
   * level.
   */
  void unmuteSamples();

  /**
   * Toggles the mute state of all audio tracks and samples.
   *
   * If any track or sample is muted, it will be unmuted, and vice versa.
   */
  void muteAll();

  /**
   * Unmutes all audio tracks and samples.
   *
   * This function restores the volume of all audio tracks and samples to their
   * previous level.
   */
  void unmuteAll();

  /**
   * Toggles the mute state of all audio tracks and samples.
   *
   * If any track or sample is muted, it will be unmuted, and vice versa.
   */
  void toggleMuteAll();

  /**
   * Toggles the mute state of all audio tracks.
   *
   * If any track is muted, it will be unmuted, and vice versa.
   */
  void toggleMuteTracks();

  /**
   * Toggles the mute state of all audio samples.
   *
   * If any sample is muted, it will be unmuted, and vice versa.
   */
  void toggleMuteSamples();

  /**
   * Get the current audio track.
   *
   * @return AudioTrack The current audio track.
   */
  AudioTrack  getCurrentAudioTrack() const;

  /**
   * Get the last played audio track.
   *
   * @return AudioTrack The last played audio track.
   */
  AudioTrack  getLastAudioTrack() const;

  /**
   * Get the last played audio sample.
   *
   * @return AudioSample The last played audio sample. 
   */
  AudioSample getLastAudioSample() const;

  /**
   * Checks if the currently playing audio track is paused.
   *
   * @return true if the track is paused, false otherwise.
   */

  [[nodiscard]] static bool isTrackPlaying();

  /**
   * Checks if the currently playing audio track is paused.
   *
   * @return true if the track is paused, false otherwise.
   */
  [[nodiscard]] static bool isTrackPaused();
};
