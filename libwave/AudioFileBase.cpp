#include "AudioFileBase.h"

AudioFileBase::AudioFileBase(std::size_t channels, std::size_t sampleRate, std::size_t bitsPerSample)
    : m_channels{channels}, m_sampleRate{sampleRate}, m_bitsPerSample{bitsPerSample} {
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
