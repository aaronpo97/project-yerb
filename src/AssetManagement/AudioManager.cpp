#include "../../includes/AssetManagement/AudioManager.hpp"

#define MAIN_MENU_MUSIC_PATH "assets/audio/tracks/main_menu.ogg"
#define PLAY_MUSIC_PATH "assets/audio/tracks/play.ogg"
#define ITEM_ACQUIRED_SOUND_PATH "assets/audio/samples/item_acquired.wav"
#define ENEMY_COLLIDES_SOUND_PATH "assets/audio/samples/enemy_collides.wav"
#define SPEED_BOOST_COLLIDES_SOUND_PATH "assets/audio/samples/speed_boost_collides.wav"
#define SLOWNESS_DEBUFF_COLLIDES_SOUND_PATH "assets/audio/samples/slowness_debuff_collides.wav"
#define MENU_MOVE_SOUND_PATH "assets/audio/samples/menu_move.wav"
#define MENU_SELECT_SOUND_PATH "assets/audio/samples/menu_select.wav"

AudioManager::AudioManager(int frequency, Uint16 format, int channels, int chunksize) :
    m_frequency(frequency), m_format(format), m_channels(channels), m_chunksize(chunksize) {
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    throw std::runtime_error("SDL_Init failed");
  }

  if (Mix_OpenAudio(m_frequency, m_format, m_channels, m_chunksize) != 0) {
    SDL_Quit();
    throw std::runtime_error("Mix_OpenAudio failed");
  }

  loadAllAudio();
}

AudioManager::~AudioManager() {
  cleanup();
}

void AudioManager::loadAllAudio() {
  loadTrack(AudioTrack::MainMenu, MAIN_MENU_MUSIC_PATH);
  loadTrack(AudioTrack::Play, PLAY_MUSIC_PATH);

  loadSample(AudioSample::ItemAcquired, ITEM_ACQUIRED_SOUND_PATH);
  loadSample(AudioSample::EnemyCollides, ENEMY_COLLIDES_SOUND_PATH);
  loadSample(AudioSample::SpeedBoostCollides, SPEED_BOOST_COLLIDES_SOUND_PATH);
  loadSample(AudioSample::SlownessDebuffCollides, SLOWNESS_DEBUFF_COLLIDES_SOUND_PATH);
  loadSample(AudioSample::MenuMove, MENU_MOVE_SOUND_PATH);
  loadSample(AudioSample::MenuSelect, MENU_SELECT_SOUND_PATH);
}

void AudioManager::loadTrack(AudioTrack track, const std::string &filepath) {
  m_music[track] = Mix_LoadMUS(filepath.c_str());
  if (!m_music[track]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadMUS error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadMUS error");
  }
}

void AudioManager::loadSample(AudioSample effect, const std::string &filepath) {
  m_soundEffects[effect] = Mix_LoadWAV(filepath.c_str());
  if (!m_soundEffects[effect]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadWAV error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadWAV error");
  }
}

void AudioManager::playTrack(AudioTrack track, int loops) {
  if (track == m_currentAudioTrack) {
    SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Track %d is already playing, ignoring request.",
                static_cast<int>(track));
    return;
  }

  m_lastAudioTrack = m_currentAudioTrack;
  if (m_music[track]) {
    m_currentAudioTrack = track;
    Mix_PlayMusic(m_music[track], loops);
  }
}

void AudioManager::playSample(AudioSample effect, int loops) {
  if (m_soundEffects[effect]) {
    Mix_PlayChannel(-1, m_soundEffects[effect], loops);
    m_lastAudioSample = effect;
  }
}

void AudioManager::stopTrack() {
  m_lastAudioTrack    = m_currentAudioTrack;
  m_currentAudioTrack = AudioTrack::None;
  Mix_HaltMusic();
}

void AudioManager::pauseTrack() {
  m_audioTrackPaused = true;
  Mix_PauseMusic();
}

void AudioManager::resumeTrack() {
  m_audioTrackPaused = false;
  Mix_ResumeMusic();
}

void AudioManager::setTrackVolume(int volume) {
  if (volume > MIX_MAX_VOLUME) {
    volume = MIX_MAX_VOLUME;
  }

  if (volume < 0) {
    volume = 0;
  }

  Mix_VolumeMusic(volume);
}

void AudioManager::setSampleVolume(AudioSample effect, int volume) {
  Mix_VolumeChunk(m_soundEffects[effect], volume);
}

bool AudioManager::isTrackPlaying() const {
  return Mix_PlayingMusic() == 1;
}

bool AudioManager::isTrackPaused() const {
  return Mix_PausedMusic() == 1;
}

void AudioManager::cleanup() {
  m_currentAudioTrack = AudioTrack::None;
  m_lastAudioSample   = AudioSample::None;

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

AudioTrack AudioManager::getCurrentAudioTrack() const {
  return m_currentAudioTrack;
}

AudioTrack AudioManager::getLastAudioTrack() const {
  return m_lastAudioTrack;
}

AudioSample AudioManager::getLastAudioSample() const {
  return m_lastAudioSample;
}