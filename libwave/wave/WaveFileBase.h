#pragma once

#include "WaveFormat.h"
#include <cstdint>

class WaveFileBase {
public:
  WaveFileBase(WaveFormat format, std::size_t channels, std::size_t sampleRate, std::size_t bitsPerSample);

  WaveFormat format() const;
  std::size_t channels() const;
  std::size_t sampleRate() const;
  std::size_t bitsPerSample() const;

private:
  WaveFormat m_format;
  std::size_t m_channels;
  std::size_t m_sampleRate;
  std::size_t m_bitsPerSample;
};
