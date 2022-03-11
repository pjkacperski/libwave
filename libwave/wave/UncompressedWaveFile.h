#pragma once

#include "../AudioFileBase.h"
#include "WaveFormat.h"
#include <fstream>
#include <limits>
#include <span>
#include <type_traits>
#include <vector>

template <typename In, typename Normalization> class UncompressedWaveFile : public AudioFileBase {
public:
  UncompressedWaveFile(std::ifstream stream, std::size_t totalBytes, WaveFormat format, std::size_t channels,
                       std::size_t sampleRate, std::size_t bitsPerSample);

  std::size_t bytesLeft() const;
  WaveFormat format() const;
  template <typename T> std::size_t samplesLeft() const;

  template <typename Out, std::size_t N> std::size_t readSamples(std::span<Out, N> out);

private:
  template <typename... Args> struct TypeTag {};

  // when we read native file type we don't have to use temporary buffer
  template <typename std::size_t N> std::size_t readSamples(std::span<In, N>, TypeTag<In, In>);

  // read samples into temporary buffer, normalize each value and write it into output
  template <typename Out, std::size_t N> std::size_t readSamples(std::span<Out, N> out, TypeTag<In, Out>);

  std::ifstream m_stream;
  std::size_t m_totalBytes;
  WaveFormat m_format;
  std::vector<In> m_buffer;
  std::size_t m_bytesRead;
};

template <typename In, typename Normalization>
UncompressedWaveFile<In, Normalization>::UncompressedWaveFile(std::ifstream stream, std::size_t totalBytes,
                                                              WaveFormat format, std::size_t channels,
                                                              std::size_t sampleRate, std::size_t bitsPerSample)
    : AudioFileBase{channels, sampleRate, bitsPerSample}, m_stream{std::move(stream)},
      m_totalBytes{totalBytes}, m_format{format}, m_bytesRead{0} {
}

template <typename In, typename Normalization> std::size_t UncompressedWaveFile<In, Normalization>::bytesLeft() const {
  return m_totalBytes - m_bytesRead;
}

template <typename In, typename Normalization>
template <typename T>
std::size_t UncompressedWaveFile<In, Normalization>::samplesLeft() const {
  return bytesLeft() / sizeof(T);
}

template <typename In, typename Normalization> WaveFormat UncompressedWaveFile<In, Normalization>::format() const {
  return m_format;
}

template <typename In, typename Normalization>
template <typename Out, std::size_t N>
std::size_t UncompressedWaveFile<In, Normalization>::readSamples(std::span<Out, N> out) {
  return readSamples(out, TypeTag<In, Out>{});
}

template <typename In, typename Normalization>
template <std::size_t N>
std::size_t UncompressedWaveFile<In, Normalization>::readSamples(std::span<In, N> out, TypeTag<In, In>) {
  static_assert(std::is_trivially_copyable_v<In>, "In type must be trivially copyable");
  const auto count = std::min(out.size(), samplesLeft<In>());
  m_stream.read(reinterpret_cast<char*>(out.data()), count * sizeof(In));
  const auto bytesRead = m_stream.gcount();
  m_bytesRead += bytesRead;
  return bytesRead / sizeof(In);
}

template <typename In, typename Normalization>
template <typename Out, std::size_t N>
std::size_t UncompressedWaveFile<In, Normalization>::readSamples(std::span<Out, N> out, TypeTag<In, Out>) {
  static_assert(std::is_trivially_copyable_v<In>, "In type must be trivially copyable");
  const auto count = std::min(out.size(), samplesLeft<In>());
  if (m_buffer.size() < count) {
    m_buffer.resize(count);
  }
  m_stream.read(reinterpret_cast<char*>(m_buffer.data()), count * sizeof(In));
  const auto bytesRead = m_stream.gcount();
  const auto samplesRead = bytesRead / sizeof(In);
  for (std::size_t i = 0; i < samplesRead; ++i) {
    out[i] = Normalization::template normalize<In, Out>(m_buffer[i]);
  }
  m_bytesRead += bytesRead;
  return samplesRead;
}
