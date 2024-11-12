#pragma once

#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <array>
#include <map>
#include <memory>
#include <stdexcept>
#include <string_view>

enum class Sample {
  ItemAcquired,
  EnemyCollides,
  SpeedBoostCollides,
  SlownessDebuffCollides,
  MenuMove,
  MenuSelect,
};

enum class Track {
  MainMenu,
  Play,
};

class AudioManager {
private:
  std::map<Track, Mix_Music *> m_music = {
      {Track::MainMenu, nullptr},
      {Track::Play, nullptr},
  };
  std::map<Sample, Mix_Chunk *> m_soundEffects = {
      {Sample::ItemAcquired, nullptr},       {Sample::EnemyCollides, nullptr},
      {Sample::SpeedBoostCollides, nullptr}, {Sample::SlownessDebuffCollides, nullptr},
      {Sample::MenuMove, nullptr},           {Sample::MenuSelect, nullptr},
  };

public:
  explicit AudioManager(int    frequency = 44100,
                        Uint16 format    = MIX_DEFAULT_FORMAT,
                        int    channels  = 2,
                        int    chunksize = 2048);

  ~AudioManager();

  void loadAllAudio();

  void playTrack(Track track, int loops = -1);
  void playSample(Sample effect, int loops = 0);
  void stopTrack();
  void pauseTrack();
  void resumeTrack();
  void setTrackVolume(int volume);
  void setSampleVolume(Sample effect, int volume);

  [[nodiscard]] bool isTrackPlaying() const;
  [[nodiscard]] bool isTrackPaused() const;

private:
  void loadTrack(Track track, const std::string &filepath);
  void loadSample(Sample effect, const std::string &filepath);
  void cleanup();
};
