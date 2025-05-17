#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
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

  void loadTrack(AudioTrack track, const Path &filepath);
  void loadSample(AudioSample sample, const Path &filepath);
  void cleanup();

public:
  explicit AudioManager(int    frequency = 44100,
                        Uint16 format    = MIX_DEFAULT_FORMAT,
                        int    channels  = 2,
                        int    chunksize = 2048);

  static constexpr size_t MAX_SAMPLES_PER_FRAME = 4;
  static constexpr int    DEFAULT_SAMPLE_VOLUME = MIX_MAX_VOLUME / MAX_SAMPLES_PER_FRAME;
  static constexpr int    DEFAULT_TRACK_VOLUME  = MIX_MAX_VOLUME * 0.8;

  ~AudioManager();

  void loadAllAudio();

  void        playTrack(AudioTrack track, int loops = -1);
  void        playSample(AudioSample sample, int loops = 0);
  void        stopTrack();
  void        pauseTrack();
  void        resumeTrack();
  static void setTrackVolume(int volume);
  void        setSampleVolume(AudioSample sampleTag, int volume);
  int         getSampleVolume(AudioSample sampleTag);
  static int  getTrackVolume();

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
