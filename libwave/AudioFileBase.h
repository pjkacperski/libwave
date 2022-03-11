#pragma once

#include <cstdint>

class AudioFileBase {
public:
  std::size_t channels() const;
  std::size_t sampleRate() const;
  std::size_t bitsPerSample() const;

protected:
  AudioFileBase(std::size_t channels, std::size_t sampleRate, std::size_t bitsPerSample);

private:
  std::size_t m_channels;
  std::size_t m_sampleRate;
  std::size_t m_bitsPerSample;
};
