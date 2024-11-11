#include "../../includes/AssetManagement/AudioManager.hpp"

#define MAIN_MENU_MUSIC_PATH "assets/audio/tracks/main_menu.ogg"
#define PLAY_MUSIC_PATH "assets/audio/tracks/play.ogg"
#define ITEM_ACQUIRED_SOUND_PATH "assets/audio/samples/item_acquired.wav"
#define ENEMY_COLLIDES_SOUND_PATH "assets/audio/samples/enemy_collides.wav"
#define SPEED_BOOST_COLLIDES_SOUND_PATH "assets/audio/samples/speed_boost_collides.wav"
#define SLOWNESS_DEBUFF_COLLIDES_SOUND_PATH "assets/audio/samples/slowness_debuff_collides.wav"
#define MENU_MOVE_SOUND_PATH "assets/audio/samples/menu_move.wav"
#define MENU_SELECT_SOUND_PATH "assets/audio/samples/menu_select.wav"

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
  loadMusic(MusicTrack::MainMenu, MAIN_MENU_MUSIC_PATH);
  loadMusic(MusicTrack::Play, PLAY_MUSIC_PATH);

  loadSample(SoundEffect::ItemAcquired, ITEM_ACQUIRED_SOUND_PATH);
  loadSample(SoundEffect::EnemyCollides, ENEMY_COLLIDES_SOUND_PATH);
  loadSample(SoundEffect::SpeedBoostCollides, SPEED_BOOST_COLLIDES_SOUND_PATH);
  loadSample(SoundEffect::SlownessDebuffCollides, SLOWNESS_DEBUFF_COLLIDES_SOUND_PATH);
  loadSample(SoundEffect::MenuMove, MENU_MOVE_SOUND_PATH);
  loadSample(SoundEffect::MenuSelect, MENU_SELECT_SOUND_PATH);
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
  for (auto &[_sampleName, sample] : m_soundEffects) {
    if (sample != nullptr) {
      Mix_FreeChunk(sample);
      sample = nullptr;
    }
  }

  for (auto &[_trackName, track] : m_music) {
    if (track != nullptr) {
      Mix_FreeMusic(track);
      track = nullptr;
    }
  }

  Mix_CloseAudio();
  Mix_Quit();

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "AudioManager cleaned up successfully!");
}
