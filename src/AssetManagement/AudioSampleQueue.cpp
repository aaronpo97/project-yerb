#include "../../includes/AssetManagement/AudioSampleQueue.hpp"

AudioSampleQueue::AudioSampleQueue(AudioManager &audioManager) :
    m_audioManager(audioManager),
    m_cooldowns{
        {AudioSample::SHOOT, 100},
        {AudioSample::ENEMY_COLLISION, 200},
        {AudioSample::ITEM_ACQUIRED, 150},
        {AudioSample::SPEED_BOOST, 150},
        {AudioSample::SLOWNESS_DEBUFF, 150},
        {AudioSample::BULLET_HIT_01, 100},
        {AudioSample::BULLET_HIT_02, 100},
    } {}

void AudioSampleQueue::queueSample(const AudioSample         sample,
                                   const AudioSamplePriority priority) {
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

void AudioSampleQueue::update() {
  const Uint64     currentTime           = SDL_GetTicks64();
  size_t           soundsPlayedThisFrame = 0;
  constexpr size_t MAX_SOUNDS_PER_FRAME  = AudioManager::MAX_SAMPLES_PER_FRAME;

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

  if (m_sampleQueue.size() <= 20) {
    return;
  }
  // Clear any remaining low priority sounds if queue is getting too large

  std::priority_queue<QueuedSample> tempQueue;
  while (!m_sampleQueue.empty()) {
    const QueuedSample &queuedSample = m_sampleQueue.top();
    if (queuedSample.priority >= AudioSamplePriority::STANDARD) {
      tempQueue.push(queuedSample);
    }
    m_sampleQueue.pop();
  }

  m_sampleQueue = std::move(tempQueue);
}
