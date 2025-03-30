#include "../../includes/SystemManagement/AudioManager.hpp"

#include <iostream>

/**
 *
 * Constructor for AudioManager.
 *
 * Initializes SDL audio subsystem and loads all audio tracks and samples.
 *
 * @throws std::runtime_error if SDL_Init(SDL_INIT_AUDIO) or Mix_OpenAudio fails.
 *
 * @param frequency The frequency to playback audio at (in Hz).
 * @param format Audio format, one of SDL's AUDIO_* values.
 * @param channels number of channels.
 * @param chunksize Audio buffer size in sample FRAMES (total samples divided by channel
 * count).
 */
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

/**
 * Destructor for AudioManager.
 *
 * Calls cleanup() to free audio resources and close the audio subsystem.
 */
AudioManager::~AudioManager() {
  cleanup();
}

/**
 * Loads all audio tracks and samples.
 *
 * This function loads all audio tracks and samples into the AudioManager.
 * It uses the loadTrack() and loadSample() methods to load each track and sample.
 */
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

/**
 * Loads an audio track from a file.
 *
 * @param track The audio track to load.
 * @param filepath The path to the audio file.
 *
 * @throws std::runtime_error if Mix_LoadMUS fails.
 */
void AudioManager::loadTrack(const AudioTrack track, const Path &filepath) {
  m_audioTracks[track] = Mix_LoadMUS(filepath.c_str());
  if (!m_audioTracks[track]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadMUS error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadMUS error");
  }
}

/**
 * Loads an audio sample from a file.
 *
 * @param sample The audio sample to load.
 * @param filepath The path to the audio file.
 *
 * @throws std::runtime_error if Mix_LoadWAV fails.
 */
void AudioManager::loadSample(const AudioSample sample, const Path &filepath) {
  m_audioSamples[sample] = Mix_LoadWAV(filepath.c_str());
  if (!m_audioSamples[sample]) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Mix_LoadWAV error: %s", Mix_GetError());
    cleanup();
    throw std::runtime_error("Mix_LoadWAV error");
  }

  setSampleVolume(sample, sample == AudioSample::BULLET_HIT_01 ? DEFAULT_SAMPLE_VOLUME / 2
                                                               : DEFAULT_SAMPLE_VOLUME);
}

/**
 * Plays an audio track.
 *
 * If the track is already playing, it will be ignored.
 *
 * @param track The audio track to play.
 * @param loops The number of times to loop the track (-1 for infinite).
 */
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

/**
 * Plays an audio sample.
 *
 * @param sample The audio sample to play.
 * @param loops The number of times to loop the sample (0 for no loop).
 */
void AudioManager::playSample(const AudioSample sample, const int loops) {
  if (m_audioSamples[sample]) {
    Mix_PlayChannel(-1, m_audioSamples[sample], loops);
    m_lastAudioSample = sample;
  }
}

/**
 * Stops the currently playing audio track.
 *
 * This function stops the currently playing audio track and sets the current
 * audio track to NONE.
 */
void AudioManager::stopTrack() {
  m_lastAudioTrack    = m_currentAudioTrack;
  m_currentAudioTrack = AudioTrack::NONE;
  Mix_HaltMusic();
}

/**
 * Pauses the currently playing audio track.
 *
 * This function pauses the currently playing audio track and sets the
 * m_audioTrackPaused flag to true.
 */
void AudioManager::pauseTrack() {
  m_audioTrackPaused = true;
  Mix_PauseMusic();
}

/**
 * Resumes the currently paused audio track.
 *
 * This function resumes the currently paused audio track and sets the
 * m_audioTrackPaused flag to false.
 */
void AudioManager::resumeTrack() {
  m_audioTrackPaused = false;
  Mix_ResumeMusic();
}

/**
 * Sets the volume of the currently playing audio track.
 *
 * @param volume The volume level (0 to MIX_MAX_VOLUME).
 */
void AudioManager::setTrackVolume(int volume) {
  if (volume > MIX_MAX_VOLUME) {
    volume = MIX_MAX_VOLUME;
  }

  if (volume < 0) {
    volume = 0;
  }

  Mix_VolumeMusic(volume);
}

/**
 * Sets the volume of a specific audio sample.
 *
 * @param sampleTag The audio sample to set the volume for.
 * @param volume The volume level (0 to MIX_MAX_VOLUME).
 */
void AudioManager::setSampleVolume(const AudioSample sampleTag, int volume) {
  if (volume > MIX_MAX_VOLUME) {
    volume = MIX_MAX_VOLUME;
  }

  if (volume < 0) {
    volume = 0;
  }

  Mix_VolumeChunk(m_audioSamples[sampleTag], volume);
}

/**
 * Gets the volume of a specific audio sample.
 *
 * @param sampleTag The audio sample to get the volume for.
 * @return The volume level (0 to MIX_MAX_VOLUME).
 */
int AudioManager::getSampleVolume(const AudioSample sampleTag) {
  const auto sample = m_audioSamples[sampleTag];
  // Use -1 to query for the current sample volume.
  return Mix_VolumeChunk(sample, -1);
}

/**
 * Gets the volume of the currently playing audio track.
 *
 * @return The volume level (0 to MIX_MAX_VOLUME).
 */
int AudioManager::getTrackVolume() {
  // Use -1 to query for the current track volume.
  return Mix_VolumeMusic(-1);
}

/**
 * Checks if the currently playing audio track is paused.
 *
 * @return true if the track is paused, false otherwise.
 */
bool AudioManager::isTrackPlaying() {
  return Mix_PlayingMusic() == 1;
}

/**
 * Checks if the currently playing audio track is paused.
 *
 * @return true if the track is paused, false otherwise.
 */
bool AudioManager::isTrackPaused() {
  return Mix_PausedMusic() == 1;
}

/**
 * Cleans up the audio resources and closes the audio subsystem.
 *
 * This function frees all loaded audio tracks and samples, stops any
 * currently playing track, and closes the audio subsystem.
 */
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

/**
 * Mutes all audio tracks.
 *
 * This function sets the volume of all audio tracks to 0 and stores the
 * current volume level for later restoration.
 */
void AudioManager::muteTracks() {
  if (!m_tracksMuted) {
    // -1 queries for the current volume
    m_savedTrackVolume = getTrackVolume();
    Mix_VolumeMusic(0);
    m_tracksMuted = true;
  }
}

/**
 * Unmutes all audio tracks.
 *
 * This function restores the volume of all audio tracks to their previous
 * level.
 *
 *
 */
void AudioManager::unmuteTracks() {
  if (m_tracksMuted) {
    Mix_VolumeMusic(m_savedTrackVolume);
    m_tracksMuted = false;
  }
}

/**
 * Mutes all audio samples.
 *
 * This function sets the volume of all audio samples to 0 and stores the
 * current volume level for later restoration.
 */
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

/**
 * Unmutes all audio samples.
 *
 * This function restores the volume of all audio samples to their previous
 * level.
 */
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

/**
 * Toggles the mute state of all audio tracks and samples.
 *
 * If any track or sample is muted, it will be unmuted, and vice versa.
 */
void AudioManager::muteAll() {
  muteTracks();
  muteSamples();
}

/**
 * Unmutes all audio tracks and samples.
 *
 * This function restores the volume of all audio tracks and samples to their
 * previous level.
 */
void AudioManager::unmuteAll() {
  unmuteTracks();
  unmuteSamples();
}

/**
 * Toggles the mute state of all audio tracks and samples.
 *
 * If any track or sample is muted, it will be unmuted, and vice versa.
 */
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

/**
 * Toggles the mute state of all audio tracks.
 *
 * If any track is muted, it will be unmuted, and vice versa.
 */
void AudioManager::toggleMuteTracks() {
  m_tracksMuted ? unmuteTracks() : muteTracks();
}

/**
 * Toggles the mute state of all audio samples.
 *
 * If any sample is muted, it will be unmuted, and vice versa.
 */
void AudioManager::toggleMuteSamples() {
  m_samplesMuted ? unmuteSamples() : muteSamples();
}