#include "../../includes/AssetManagement/AudioManager.hpp"

AudioManager::AudioManager(const int    frequency,
                           const Uint16 format,
                           const int    channels,
                           const int    chunksize) :
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

  loadSample(AudioSample::ITEM_ACQUIRED, ITEM_ACQUIRED_SOUND_PATH);
  loadSample(AudioSample::ENEMY_COLLIDES, ENEMY_COLLIDES_SOUND_PATH);
  loadSample(AudioSample::SPEED_BOOST_ACQUIRED, SPEED_BOOST_ACQUIRED_SOUND_PATH);
  loadSample(AudioSample::SLOWNESS_DEBUFF_ACQUIRED, SLOWNESS_DEBUFF_ACQUIRED_SOUND_PATH);
  loadSample(AudioSample::MENU_MOVE, MENU_MOVE_SOUND_PATH);
  loadSample(AudioSample::MENU_SELECT, MENU_SELECT_SOUND_PATH);
  loadSample(AudioSample::SHOOT, SHOOT_SOUND_PATH);
  loadSample(AudioSample::BULLET_HIT_01, BULLET_HIT_01_SOUND_PATH);
  loadSample(AudioSample::BULLET_HIT_02, BULLET_HIT_02_SOUND_PATH);
}

void AudioManager::loadTrack(const AudioTrack track, const std::string &filepath) {
  m_audioTracks[track] = Mix_LoadMUS(filepath.c_str());
  if (!m_audioTracks[track]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadMUS error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadMUS error");
  }
}

void AudioManager::loadSample(const AudioSample effect, const std::string &filepath) {
  m_audioSamples[effect] = Mix_LoadWAV(filepath.c_str());
  if (!m_audioSamples[effect]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadWAV error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadWAV error");
  }
}

void AudioManager::playTrack(const AudioTrack track, const int loops) {
  if (track == m_currentAudioTrack) {
    SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Track %d is already playing, ignoring request.",
                static_cast<int>(track));
    return;
  }

  m_lastAudioTrack = m_currentAudioTrack;
  if (m_audioTracks[track]) {
    m_currentAudioTrack = track;
    Mix_PlayMusic(m_audioTracks[track], loops);
  }
}

void AudioManager::playSample(const AudioSample sample, const int loops) {
  if (m_audioSamples[sample]) {
    Mix_PlayChannel(-1, m_audioSamples[sample], loops);
    m_lastAudioSample = sample;
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

void AudioManager::setSampleVolume(const AudioSample sampleTag, int volume) {
  if (volume > MIX_MAX_VOLUME) {
    volume = MIX_MAX_VOLUME;
  }

  if (volume < 0) {
    volume = 0;
  }

  Mix_VolumeChunk(m_audioSamples[sampleTag], volume);
}

int AudioManager::getSampleVolume(const AudioSample sampleTag) {
  const auto sample = m_audioSamples[sampleTag];
  // Use -1 to query for the current sample volume.
  return Mix_VolumeChunk(sample, -1);
}

int AudioManager::getTrackVolume() {
  // Use -1 to query for the current track volume.
  return Mix_VolumeMusic(-1);
}

bool AudioManager::isTrackPlaying() {
  return Mix_PlayingMusic() == 1;
}

bool AudioManager::isTrackPaused() {
  return Mix_PausedMusic() == 1;
}

void AudioManager::cleanup() {
  m_currentAudioTrack = AudioTrack::None;
  m_lastAudioSample   = AudioSample::NONE;

  for (auto &[sampleTag, sample] : m_audioSamples) {
    if (sample != nullptr) {
      SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Freeing audio sample %d", sampleTag);
      Mix_FreeChunk(sample);
      sample = nullptr;
    }
  }

  for (auto &[trackTag, track] : m_audioTracks) {
    if (track != nullptr) {
      SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "Freeing audio track %d", trackTag);
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

void AudioManager::muteTracks() {
  if (!m_tracksMuted) {
    // -1 queries for the current volume
    m_savedTrackVolume = getTrackVolume();
    Mix_VolumeMusic(0);
    m_tracksMuted = true;
  }
}

void AudioManager::unmuteTracks() {
  if (m_tracksMuted) {
    Mix_VolumeMusic(m_savedTrackVolume);
    m_tracksMuted = false;
  }
}

void AudioManager::muteSamples() {
  if (m_samplesMuted) {
    return;
  }

  for (const auto &[sampleTag, sample] : m_audioSamples) {
    if (sample != nullptr) {
      m_savedSampleVolumes[sampleTag] = getSampleVolume(sampleTag);
      setSampleVolume(sampleTag, 0);
    }
  }

  m_samplesMuted = true;
}

void AudioManager::unmuteSamples() {
  if (!m_samplesMuted) {
    return;
  }

  for (const auto &[sampleTag, audioSample] : m_audioSamples) {
    if (audioSample != nullptr) {
      const int savedVolume = m_savedSampleVolumes[sampleTag];
      setSampleVolume(sampleTag, savedVolume);
    }
  }
  m_samplesMuted = false;
}

void AudioManager::muteAll() {
  muteTracks();
  muteSamples();
}

void AudioManager::unmuteAll() {
  unmuteTracks();
  unmuteSamples();
}

void AudioManager::toggleMuteAll() {
  const bool anyMuted = m_tracksMuted || m_samplesMuted;

  if (anyMuted) {
    unmuteTracks();
    unmuteSamples();
  } else {
    muteTracks();
    muteSamples();
  }
}

void AudioManager::toggleMuteTracks() {
  m_tracksMuted ? unmuteTracks() : muteTracks();
}

void AudioManager::toggleMuteSamples() {
  m_samplesMuted ? unmuteSamples() : muteSamples();
}