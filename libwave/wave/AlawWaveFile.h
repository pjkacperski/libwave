#pragma once

#include "../AudioFileBase.h"
#include "../ScalarNormalization.h"
#include "WaveFormat.h"
#include "alaw.h"
#include <cstdint>
#include <fstream>
#include <span>
#include <vector>

class AlawWaveFile : public AudioFileBase
{
public:
  AlawWaveFile(std::ifstream stream, std::size_t totalBytes, std::size_t channels, std::size_t sampleRate,
               std::size_t bitsPerSample);

  WaveFormat format() const;
  std::size_t samplesLeft() const;

  template <typename T, std::size_t Extent> std::size_t readSamples(std::span<T, Extent> out);

private:
  std::vector<std::uint8_t> m_buffer;
};

template <typename T, std::size_t Extent> std::size_t AlawWaveFile::readSamples(std::span<T, Extent> out)
{
  if (m_buffer.size() < out.size())
  {
    m_buffer.resize(out.size());
  }
  auto samplesRead = read(std::span(m_buffer).subspan(0, out.size()));
  for (std::size_t i = 0; i < samplesRead; ++i)
  {
    out[i] = ScalarNormalization::normalize<std::int16_t, T>(alaw::decode(m_buffer[i]));
  }
  return samplesRead;
}
