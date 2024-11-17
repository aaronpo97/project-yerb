#pragma once

#include "AudioManager.hpp"
#include <queue>
#include <unordered_map>

enum class AudioPriority { BACKGROUND, STANDARD, IMPORTANT, CRITICAL };

struct QueuedSample {
  AudioSample   sample;
  AudioPriority priority;
  Uint64        timestamp;

  bool operator<(const QueuedSample &other) const {
    if (priority != other.priority) {
      return priority < other.priority;
    }
    return timestamp > other.timestamp; // Earlier sounds first within same priority
  }
};

class AudioSampleManager {
private:
  std::priority_queue<QueuedSample>       m_sampleQueue;
  std::unordered_map<AudioSample, Uint64> m_lastPlayTimes;
  AudioManager                           &m_audioManager;

  static constexpr Uint64                 MIN_REPLAY_INTERVAL  = 50;
  static constexpr size_t                 MAX_SOUNDS_PER_FRAME = 4;
  std::unordered_map<AudioSample, Uint64> m_cooldowns          = {
      {AudioSample::SHOOT, 100},
      {AudioSample::ENEMY_COLLIDES, 200},
      {AudioSample::ITEM_ACQUIRED, 150},
      {AudioSample::SPEED_BOOST_ACQUIRED, 150},
      {AudioSample::SLOWNESS_DEBUFF_ACQUIRED, 150},
      {AudioSample::BULLET_HIT_01, 100},
      {AudioSample::BULLET_HIT_02, 100},
  };

public:
  explicit AudioSampleManager(AudioManager &audioManager) :
      m_audioManager(audioManager) {}

  void queueSample(const AudioSample sample, const AudioPriority priority) {
    const Uint64 currentTime = SDL_GetTicks64();

    if (m_lastPlayTimes.contains(sample)) {
      const Uint64 lastPlayTime      = m_lastPlayTimes.find(sample)->second;
      const Uint64 timeSinceLastPlay = currentTime - lastPlayTime;
      if (const Uint64 cooldown = m_cooldowns[sample]; timeSinceLastPlay < cooldown) {
        return; // Sound is still in cooldown
      }
    }

    m_sampleQueue.push({.sample = sample, .priority = priority, .timestamp = currentTime});
  }

  void update() {
    const Uint64 currentTime           = SDL_GetTicks64();
    size_t       soundsPlayedThisFrame = 0;

    while (!m_sampleQueue.empty() && soundsPlayedThisFrame < MAX_SOUNDS_PER_FRAME) {
      const auto &[sample, priority, timestamp] = m_sampleQueue.top();

      // Check if sound is still in cooldown
      const bool soundInCooldown = currentTime - timestamp > 500;

      if (soundInCooldown) {
        m_sampleQueue.pop();
        continue;
      }

      m_audioManager.playSample(sample);
      m_lastPlayTimes[sample] = currentTime;

      m_sampleQueue.pop();
      soundsPlayedThisFrame += 1;
    }

    // Clear any remaining low priority sounds if queue is getting too large
    if (m_sampleQueue.size() > 20) {
      std::priority_queue<QueuedSample> tempQueue;
      while (!m_sampleQueue.empty()) {
        const QueuedSample &sound = m_sampleQueue.top();
        if (sound.priority >= AudioPriority::STANDARD) {
          tempQueue.push(sound);
        }
        m_sampleQueue.pop();
      }

      m_sampleQueue = std::move(tempQueue);
    }
  }
};