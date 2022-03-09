#include "WaveFileBase.h"

WaveFileBase::WaveFileBase(WaveFormat format, std::size_t channels, std::size_t sampleRate, std::size_t bitsPerSample)
    : m_format{format}, m_channels{channels}, m_sampleRate{sampleRate}, m_bitsPerSample{bitsPerSample} {
}

WaveFormat WaveFileBase::format() const {
  return m_format;
}

std::size_t WaveFileBase::channels() const {
  return m_channels;
}

std::size_t WaveFileBase::sampleRate() const {
  return m_sampleRate;
}

std::size_t WaveFileBase::bitsPerSample() const {
  return m_bitsPerSample;
}
