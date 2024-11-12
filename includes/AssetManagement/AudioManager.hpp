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
  std::map<AudioTrack, Mix_Music *> m_audioTracks = {
      {AudioTrack::MainMenu, nullptr},
      {AudioTrack::Play, nullptr},
  };
  std::map<AudioSample, Mix_Chunk *> m_audioSamples = {
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

  bool                       m_tracksMuted      = false;
  bool                       m_samplesMuted     = false;
  int                        m_savedTrackVolume = MIX_MAX_VOLUME;
  std::map<AudioSample, int> m_savedSampleVolumes;

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
  void        playSample(AudioSample sample, int loops = 0);
  void        stopTrack();
  void        pauseTrack();
  void        resumeTrack();
  static void setTrackVolume(int volume);
  void        setSampleVolume(AudioSample sample, int volume);

  int        getSampleVolume(AudioSample sampleTag);
  static int getTrackVolume();

  void muteTracks();
  void unmuteTracks();
  void muteSamples();
  void unmuteSamples();
  void muteAll();
  void unmuteAll();

  void toggleMuteAll();
  void toggleMuteTracks();
  void toggleMuteSamples();

  AudioTrack  getCurrentAudioTrack() const;
  AudioTrack  getLastAudioTrack() const;
  AudioSample getLastAudioSample() const;

  [[nodiscard]] static bool isTrackPlaying();
  [[nodiscard]] static bool isTrackPaused();
};