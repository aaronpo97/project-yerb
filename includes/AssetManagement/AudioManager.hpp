#pragma once

#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <array>
#include <map>
#include <memory>
#include <stdexcept>
#include <string_view>

enum class AudioSample {
  None,
  ItemAcquired,
  EnemyCollides,
  SpeedBoostCollides,
  SlownessDebuffCollides,
  MenuMove,
  MenuSelect,
};

enum class AudioTrack {
  None,
  MainMenu,
  Play,
};

class AudioManager {
private:
  std::map<AudioTrack, Mix_Music *> m_music = {
      {AudioTrack::MainMenu, nullptr},
      {AudioTrack::Play, nullptr},
  };
  std::map<AudioSample, Mix_Chunk *> m_soundEffects = {
      {AudioSample::ItemAcquired, nullptr},
      {AudioSample::EnemyCollides, nullptr},
      {AudioSample::SpeedBoostCollides, nullptr},
      {AudioSample::SlownessDebuffCollides, nullptr},
      {AudioSample::MenuMove, nullptr},
      {AudioSample::MenuSelect, nullptr},
  };

  int    m_frequency = 0;
  Uint16 m_format    = 0;
  int    m_channels  = 0;
  int    m_chunksize = 0;

  AudioTrack  m_currentAudioTrack = AudioTrack::None;
  AudioTrack  m_lastAudioTrack    = AudioTrack::None;
  AudioSample m_lastAudioSample   = AudioSample::None;
  bool        m_audioTrackPaused  = false;

  void loadTrack(AudioTrack track, const std::string &filepath);
  void loadSample(AudioSample effect, const std::string &filepath);
  void cleanup();

public:
  explicit AudioManager(int    frequency = 44100,
                        Uint16 format    = MIX_DEFAULT_FORMAT,
                        int    channels  = 2,
                        int    chunksize = 2048);

  ~AudioManager();

  void loadAllAudio();

  void        playTrack(AudioTrack track, int loops = -1);
  void        playSample(AudioSample effect, int loops = 0);
  void        stopTrack();
  void        pauseTrack();
  void        resumeTrack();
  static void setTrackVolume(int volume);
  void        setSampleVolume(AudioSample effect, int volume);

  AudioTrack  getCurrentAudioTrack() const;
  AudioTrack  getLastAudioTrack() const;
  AudioSample getLastAudioSample() const;

  [[nodiscard]] static bool isTrackPlaying();
  [[nodiscard]] static bool isTrackPaused();
};
