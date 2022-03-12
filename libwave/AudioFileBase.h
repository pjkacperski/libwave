#pragma once

#include <cstdint>
#include <fstream>
#include <span>

class AudioFileBase
{
public:
  std::size_t channels() const;
  std::size_t sampleRate() const;
  std::size_t bitsPerSample() const;

protected:
  AudioFileBase(std::ifstream stream, std::size_t totalBytes, std::size_t channels, std::size_t sampleRate,
                std::size_t bitsPerSample);

  std::size_t rawBytesLeft() const;
  template <typename T, std::size_t Extent> std::size_t read(std::span<T, Extent> out);

private:
  std::ifstream m_stream;
  std::size_t m_totalBytes;
  std::size_t m_channels;
  std::size_t m_sampleRate;
  std::size_t m_bitsPerSample;
  std::size_t m_bytesRead;
};

template <typename T, std::size_t Extent> std::size_t AudioFileBase::read(std::span<T, Extent> out)
{
  static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
  const auto count = std::min(out.size(), rawBytesLeft() / sizeof(T));
  m_stream.read(reinterpret_cast<char*>(out.data()), count * sizeof(T));
  const auto bytesRead = m_stream.gcount();
  m_bytesRead += bytesRead;
  return bytesRead / sizeof(T);
}
