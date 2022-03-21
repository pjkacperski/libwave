#pragma once

#include "../AudioFileBase.h"
#include "../ScalarNormalization.h"
#include "WaveFormat.h"
#include "g711.h"
#include <cstdint>
#include <fstream>
#include <span>
#include <vector>

template <typename Decoder> class G711WaveFile : public AudioFileBase
{
public:
  G711WaveFile(std::ifstream stream, std::size_t totalBytes, std::size_t channels, std::size_t sampleRate);

  WaveFormat format() const;
  std::size_t samplesLeft() const;

  template <typename T, std::size_t Extent> std::size_t readSamples(std::span<T, Extent> out);

private:
  std::vector<std::uint8_t> m_buffer;
};

template <typename Decoder>
G711WaveFile<Decoder>::G711WaveFile(std::ifstream stream, std::size_t totalBytes, std::size_t channels,
                                    std::size_t sampleRate)
    : AudioFileBase{std::move(stream), totalBytes, channels, sampleRate, 8u}
{
}

template <typename Decoder> WaveFormat G711WaveFile<Decoder>::format() const
{
  if constexpr (Decoder::encondig() == g711::Encoding::Alaw)
  {
    return WaveFormat::Alaw;
  }
  else if constexpr (Decoder::encoding() == g711::Encoding::Ulaw)
  {
    return WaveFormat::Ulaw;
  }
  else
  {
    static_assert("unknown G.711 encoding");
  }
}

template <typename Decoder> std::size_t G711WaveFile<Decoder>::samplesLeft() const
{
  return rawBytesLeft();
}

template <typename Decoder>
template <typename T, std::size_t Extent>
std::size_t G711WaveFile<Decoder>::readSamples(std::span<T, Extent> out)
{
  if (m_buffer.size() < out.size())
  {
    m_buffer.resize(out.size());
  }
  auto samplesRead = read(std::span(m_buffer).subspan(0, out.size()));
  for (std::size_t i = 0; i < samplesRead; ++i)
  {
    out[i] = ScalarNormalization::normalize<std::int16_t, T>(Decoder::decode(m_buffer[i]));
  }
  return samplesRead;
}
