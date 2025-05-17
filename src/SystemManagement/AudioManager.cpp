#include "../../includes/SystemManagement/AudioManager.hpp"

#include <iostream>

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
  setTrackVolume(DEFAULT_TRACK_VOLUME);
}

AudioManager::~AudioManager() {
  cleanup();
}

void AudioManager::loadAllAudio() {
  loadTrack(AudioTrack::MAIN_MENU, AudioPath::MAIN_MENU);
  loadTrack(AudioTrack::PLAY, AudioPath::PLAY);

  loadSample(AudioSample::ITEM_ACQUIRED, AudioPath::ITEM_ACQUIRED);
  loadSample(AudioSample::ENEMY_COLLISION, AudioPath::ENEMY_COLLISION);
  loadSample(AudioSample::SPEED_BOOST, AudioPath::SPEED_BOOST);
  loadSample(AudioSample::SLOWNESS_DEBUFF, AudioPath::SLOWNESS_DEBUFF);
  loadSample(AudioSample::MENU_MOVE, AudioPath::MENU_MOVE);
  loadSample(AudioSample::MENU_SELECT, AudioPath::MENU_SELECT);
  loadSample(AudioSample::SHOOT, AudioPath::SHOOT);
  loadSample(AudioSample::BULLET_HIT_01, AudioPath::BULLET_HIT_01);
  loadSample(AudioSample::BULLET_HIT_02, AudioPath::BULLET_HIT_02);
}

void AudioManager::loadTrack(const AudioTrack track, const Path &filepath) {
  m_audioTracks[track] = Mix_LoadMUS(filepath.c_str());
  if (!m_audioTracks[track]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadMUS error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadMUS error");
  }
}

void AudioManager::loadSample(const AudioSample sample, const Path &filepath) {
  m_audioSamples[sample] = Mix_LoadWAV(filepath.c_str());
  if (!m_audioSamples[sample]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadWAV error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadWAV error");
  }

  setSampleVolume(sample,
                  sample == AudioSample::BULLET_HIT_01 ? DEFAULT_SAMPLE_VOLUME / 2
                                                       : DEFAULT_SAMPLE_VOLUME);
}

void AudioManager::playTrack(const AudioTrack track, const int loops) {
  if (track == m_currentAudioTrack) {
    SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO,
                "Track %d is already playing, ignoring request.",
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
  m_currentAudioTrack = AudioTrack::NONE;
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
  m_currentAudioTrack = AudioTrack::NONE;
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

  SDL_QuitSubSystem(SDL_INIT_AUDIO);
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