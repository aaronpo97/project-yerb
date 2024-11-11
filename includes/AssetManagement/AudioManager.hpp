#pragma once

#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <array>
#include <map>
#include <memory>
#include <stdexcept>
#include <string_view>

enum class SoundEffect {
  ItemAcquired,
  EnemyCollides,
  SpeedBoostCollides,
  SlownessDebuffCollides,
  MenuMove,
  MenuSelect,
};

enum class MusicTrack {
  MainMenu,
  Play,
};

class AudioManager {
private:
  std::map<MusicTrack, Mix_Music *> m_music = {
      {MusicTrack::MainMenu, nullptr},
      {MusicTrack::Play, nullptr},
  };
  std::map<SoundEffect, Mix_Chunk *> m_soundEffects = {
      {SoundEffect::ItemAcquired, nullptr},
      {SoundEffect::EnemyCollides, nullptr},
      {SoundEffect::SpeedBoostCollides, nullptr},
      {SoundEffect::SlownessDebuffCollides, nullptr},
      {SoundEffect::MenuMove, nullptr},
      {SoundEffect::MenuSelect, nullptr},
  };

public:
  explicit AudioManager(int    frequency = 44100,
                        Uint16 format    = MIX_DEFAULT_FORMAT,
                        int    channels  = 2,
                        int    chunksize = 2048);

  ~AudioManager();

  void loadAllAudio();

  void playMusic(MusicTrack track, int loops = -1);
  void playSample(SoundEffect effect, int loops = 0);
  void stopMusic();
  void pauseMusic();
  void resumeMusic();
  void setMusicVolume(int volume);
  void setSoundVolume(SoundEffect effect, int volume);

  [[nodiscard]] bool isMusicPlaying() const;
  [[nodiscard]] bool isMusicPaused() const;

private:
  void loadMusic(MusicTrack track, const std::string &filepath);
  void loadSample(SoundEffect effect, const std::string &filepath);
  void cleanup();
};
