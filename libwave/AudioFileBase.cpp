#include "AudioFileBase.h"

AudioFileBase::AudioFileBase(std::ifstream stream, std::size_t totalBytes, std::size_t channels, std::size_t sampleRate,
                             std::size_t bitsPerSample)
    : m_stream{std::move(stream)}, m_totalBytes{totalBytes}, m_channels{channels}, m_sampleRate{sampleRate},
      m_bitsPerSample{bitsPerSample}, m_bytesRead{0} {
}

std::size_t AudioFileBase::channels() const {
  return m_channels;
}

std::size_t AudioFileBase::sampleRate() const {
  return m_sampleRate;
}

std::size_t AudioFileBase::bitsPerSample() const {
  return m_bitsPerSample;
}

std::size_t AudioFileBase::rawBytesLeft() const {
  return m_totalBytes - m_bytesRead;
}
