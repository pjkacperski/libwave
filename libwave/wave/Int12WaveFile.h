#pragma once

#include "../AudioFileBase.h"
#include "WaveFormat.h"
#include <cstdint>
#include <fstream>
#include <span>
#include <vector>

template <typename Normalization> class Int12WaveFile : public AudioFileBase
{
public:
  Int12WaveFile(std::ifstream stream, std::size_t totalBytes, std::size_t channels, std::size_t sampleRate,
                WaveFormat format);

  WaveFormat format() const;
  std::size_t samplesLeft() const;

  template <std::size_t Extent> std::size_t readRawSamples(std::span<std::uint16_t, Extent> out);
  template <typename T, std::size_t Extent> std::size_t readSamples(std::span<T, Extent> out);

private:
  template <typename T> static constexpr T normalize(std::uint16_t value12bit);

  WaveFormat m_format;
  std::vector<std::uint16_t> m_buffer;
};

template <typename Normalization>
Int12WaveFile<Normalization>::Int12WaveFile(std::ifstream stream, std::size_t totalBytes, std::size_t channels,
                                            std::size_t sampleRate, WaveFormat format)
    : AudioFileBase{std::move(stream), totalBytes, channels, sampleRate, 12u}, m_format{format}
{
}

template <typename Normalization> WaveFormat Int12WaveFile<Normalization>::format() const
{
  return m_format;
}

template <typename Normalization> std::size_t Int12WaveFile<Normalization>::samplesLeft() const
{
  return rawBytesLeft() / 2;
}

template <typename Normalization>
template <std::size_t Extent>
std::size_t Int12WaveFile<Normalization>::readRawSamples(std::span<std::uint16_t, Extent> out)
{
  return read(out);
}

template <typename Normalization>
template <typename T, std::size_t Extent>
std::size_t Int12WaveFile<Normalization>::readSamples(std::span<T, Extent> out)
{
  if (m_buffer.size() < out.size())
  {
    m_buffer.resize(out.size());
  }
  auto samplesRead = readRawSamples(std::span(m_buffer).subspan(0, out.size()));
  for (std::size_t i = 0; i < samplesRead; ++i)
  {
    out[i] = normalize<T>(m_buffer[i]);
  }
  return samplesRead;
}

template <typename Normalization>
template <typename T>
constexpr T Int12WaveFile<Normalization>::normalize(std::uint16_t value12bit)
{
  value12bit >>= 4;
  const auto sign = value12bit & 0x0800u;
  return Normalization::template normalize<float, T>(sign ? static_cast<float>(0x07FFu & value12bit) / 2048 - 1
                                                          : static_cast<float>(0x07FFu & value12bit) / 2047);
}
