#pragma once

#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <array>
#include <map>
#include <memory>
#include <stdexcept>
#include <string_view>

#define MAIN_MENU_MUSIC_PATH "assets/audio/tracks/main_menu.ogg"
#define PLAY_MUSIC_PATH "assets/audio/tracks/play.ogg"
#define ITEM_ACQUIRED_SOUND_PATH "assets/audio/samples/item_acquired.wav"
#define ENEMY_COLLIDES_SOUND_PATH "assets/audio/samples/enemy_collides.wav"
#define SPEED_BOOST_ACQUIRED_SOUND_PATH "assets/audio/samples/speed_boost_acquired.wav"
#define SLOWNESS_DEBUFF_ACQUIRED_SOUND_PATH "assets/audio/samples/slowness_debuff_acquired.wav"
#define MENU_MOVE_SOUND_PATH "assets/audio/samples/menu_move.wav"
#define MENU_SELECT_SOUND_PATH "assets/audio/samples/menu_select.wav"
#define SHOOT_SOUND_PATH "assets/audio/samples/shoot.wav"
#define BULLET_HIT_01_SOUND_PATH "assets/audio/samples/bullet_hit_01.wav"
#define BULLET_HIT_02_SOUND_PATH "assets/audio/samples/bullet_hit_02.wav"

enum class AudioSample {
  None,
  ItemAcquired,
  EnemyCollides,
  SpeedBoostAcquired,
  SlownessDebuffAcquired,
  MenuMove,
  MenuSelect,
  Shoot,
  BulletHit01,
  BulletHit02,
  s
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
      {AudioSample::SpeedBoostAcquired, nullptr},
      {AudioSample::SlownessDebuffAcquired, nullptr},
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