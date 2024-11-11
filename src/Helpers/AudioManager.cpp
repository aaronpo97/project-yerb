#include "../../includes/Helpers/AudioManager.hpp"

AudioManager::AudioManager(int frequency, Uint16 format, int channels, int chunksize) {
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    throw std::runtime_error("SDL_Init failed");
  }

  if (Mix_OpenAudio(frequency, format, channels, chunksize) != 0) {
    SDL_Quit();
    throw std::runtime_error("Mix_OpenAudio failed");
  }

  loadAllAudio();
}

AudioManager::~AudioManager() {
  cleanup();
}

void AudioManager::loadAllAudio() {
  loadMusic(MusicTrack::MainMenu, "assets/audio/tracks/main_menu.ogg");
  loadMusic(MusicTrack::Play, "assets/audio/tracks/play.ogg");

  loadSample(SoundEffect::ItemAcquired, "assets/audio/samples/item_acquired.wav");
  loadSample(SoundEffect::EnemyCollides, "assets/audio/samples/enemy_collides.wav");
  loadSample(SoundEffect::SpeedBoostCollides, "assets/audio/samples/speed_boost_collides.wav");
  loadSample(SoundEffect::SlownessDebuffCollides,
             "assets/audio/samples/slowness_debuff_collides.wav");
  loadSample(SoundEffect::MenuMove, "assets/audio/samples/menu_move.wav");
  loadSample(SoundEffect::MenuSelect, "assets/audio/samples/menu_select.wav");
}

void AudioManager::loadMusic(MusicTrack track, const std::string &filepath) {
  m_music[track] = Mix_LoadMUS(filepath.c_str());
  if (!m_music[track]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadMUS error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadMUS error");
  }
}

void AudioManager::loadSample(SoundEffect effect, const std::string &filepath) {
  m_soundEffects[effect] = Mix_LoadWAV(filepath.c_str());
  if (!m_soundEffects[effect]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadWAV error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadWAV error");
  }
}

void AudioManager::playMusic(MusicTrack track, int loops) {
  if (m_music[track]) {
    Mix_PlayMusic(m_music[track], loops);
  }
}

void AudioManager::playSample(SoundEffect effect, int loops) {
  if (m_soundEffects[effect]) {
    Mix_PlayChannel(-1, m_soundEffects[effect], loops);
  }
}

void AudioManager::stopMusic() {
  Mix_HaltMusic();
}

void AudioManager::pauseMusic() {
  Mix_PauseMusic();
}

void AudioManager::resumeMusic() {
  Mix_ResumeMusic();
}

void AudioManager::setMusicVolume(int volume) {
  Mix_VolumeMusic(volume);
}

void AudioManager::setSoundVolume(SoundEffect effect, int volume) {
  Mix_VolumeChunk(m_soundEffects[effect], volume);
}

bool AudioManager::isMusicPlaying() const {
  return Mix_PlayingMusic() == 1;
}

bool AudioManager::isMusicPaused() const {
  return Mix_PausedMusic() == 1;
}

void AudioManager::cleanup() {
  for (auto &pair : m_music) {
    if (pair.second) {
      Mix_FreeMusic(pair.second);
      pair.second = nullptr;
    }
  }

  for (auto &pair : m_soundEffects) {
    if (pair.second) {
      Mix_FreeChunk(pair.second);
      pair.second = nullptr;
    }
  }

  Mix_CloseAudio();
  Mix_Quit();

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "AudioManager cleaned up successfully!");
}
