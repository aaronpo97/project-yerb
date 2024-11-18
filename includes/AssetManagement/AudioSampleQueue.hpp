#pragma once

#include "AudioManager.hpp"
#include <queue>
#include <unordered_map>

enum class AudioSamplePriority { BACKGROUND, STANDARD, IMPORTANT, CRITICAL };

struct QueuedSample {
  AudioSample         sample;
  AudioSamplePriority priority;
  Uint64              timestamp;

  bool operator<(const QueuedSample &other) const {
    if (priority != other.priority) {
      return priority < other.priority;
    }
    return timestamp > other.timestamp; // Earlier sounds first within same priority
  }
};

class AudioSampleQueue {
private:
  std::priority_queue<QueuedSample>       m_sampleQueue;
  std::unordered_map<AudioSample, Uint64> m_lastPlayTimes;
  AudioManager                           &m_audioManager;

  static constexpr Uint64                 MIN_REPLAY_INTERVAL = 50;
  std::unordered_map<AudioSample, Uint64> m_cooldowns;

public:
  explicit AudioSampleQueue(AudioManager &audioManager);
  void queueSample(AudioSample sample, AudioSamplePriority priority);
  void update();
};