#pragma once

#include "../AudioFileBase.h"
#include "WaveFormat.h"
#include <fstream>
#include <limits>
#include <span>
#include <type_traits>
#include <vector>

template <typename In, typename Normalization> class UncompressedWaveFile : public AudioFileBase
{
public:
  UncompressedWaveFile(std::ifstream stream, std::size_t totalBytes, std::size_t channels, std::size_t sampleRate,
                       std::size_t bitsPerSample, WaveFormat format);

  WaveFormat format() const;
  std::size_t samplesLeft() const;

  template <typename Out, std::size_t Extent> std::size_t readSamples(std::span<Out, Extent> out);

private:
  template <typename... Args> struct TypeTag
  {
  };

  // when we read native file type we don't have to use temporary buffer
  template <typename std::size_t Extent> std::size_t readSamples(std::span<In, Extent>, TypeTag<In, In>);

  // read samples into temporary buffer, normalize each value and write it into output
  template <typename Out, std::size_t Extent> std::size_t readSamples(std::span<Out, Extent> out, TypeTag<In, Out>);

  WaveFormat m_format;
  std::vector<In> m_buffer;
};

template <typename In, typename Normalization>
UncompressedWaveFile<In, Normalization>::UncompressedWaveFile(std::ifstream stream, std::size_t totalBytes,
                                                              std::size_t channels, std::size_t sampleRate,
                                                              std::size_t bitsPerSample, WaveFormat format)
    : AudioFileBase{std::move(stream), totalBytes, channels, sampleRate, bitsPerSample}, m_format{format}
{
}

template <typename In, typename Normalization> WaveFormat UncompressedWaveFile<In, Normalization>::format() const
{
  return m_format;
}

template <typename In, typename Normalization> std::size_t UncompressedWaveFile<In, Normalization>::samplesLeft() const
{
  return rawBytesLeft() / sizeof(In);
}

template <typename In, typename Normalization>
template <typename Out, std::size_t N>
std::size_t UncompressedWaveFile<In, Normalization>::readSamples(std::span<Out, N> out)
{
  return readSamples(out, TypeTag<In, Out>{});
}

template <typename In, typename Normalization>
template <std::size_t Extent>
std::size_t UncompressedWaveFile<In, Normalization>::readSamples(std::span<In, Extent> out, TypeTag<In, In>)
{
  return read(out);
}

template <typename In, typename Normalization>
template <typename Out, std::size_t Extent>
std::size_t UncompressedWaveFile<In, Normalization>::readSamples(std::span<Out, Extent> out, TypeTag<In, Out>)
{
  if (m_buffer.size() < out.size())
  {
    m_buffer.resize(out.size());
  }
  const auto samplesRead = read(std::span(m_buffer).subspan(0, out.size()));
  for (std::size_t i = 0; i < samplesRead; ++i)
  {
    out[i] = Normalization::template normalize<In, Out>(m_buffer[i]);
  }
  return samplesRead;
}
