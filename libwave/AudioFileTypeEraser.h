#pragma once

#include "AudioFile.h"

template <typename AudioFileImpl> class AudioFileTypeEraser : public AudioFile {
public:
  template <typename... Args> AudioFileTypeEraser(Args&&... args);

  ~AudioFileTypeEraser() = default;

  std::size_t sampleRate() const override;
  std::size_t channels() const override;

  std::size_t samplesLeft() const override;

  std::size_t readSamples(std::span<std::uint8_t> outout) override;
  std::size_t readSamples(std::span<std::int16_t> outout) override;
  std::size_t readSamples(std::span<std::int32_t> output) override;
  std::size_t readSamples(std::span<float> output) override;
  std::size_t readSamples(std::span<double> output) override;

private:
  AudioFileImpl m_impl;
};

template <typename AudioFileImpl>
template <typename... Args>
AudioFileTypeEraser<AudioFileImpl>::AudioFileTypeEraser(Args&&... args) : m_impl{std::forward<Args>(args)...} {
}

template <typename AudioFileImpl> std::size_t AudioFileTypeEraser<AudioFileImpl>::sampleRate() const {
  return m_impl.sampleRate();
}

template <typename AudioFileImpl> std::size_t AudioFileTypeEraser<AudioFileImpl>::channels() const {
  return m_impl.channels();
}

template <typename AudioFileImpl> std::size_t AudioFileTypeEraser<AudioFileImpl>::samplesLeft() const {
  return m_impl.samplesLeft();
}

template <typename AudioFileImpl>
std::size_t AudioFileTypeEraser<AudioFileImpl>::readSamples(std::span<std::uint8_t> output) {
  return m_impl.readSamples(output);
}

template <typename AudioFileImpl>
std::size_t AudioFileTypeEraser<AudioFileImpl>::readSamples(std::span<std::int16_t> output) {
  return m_impl.readSamples(output);
}

template <typename AudioFileImpl>
std::size_t AudioFileTypeEraser<AudioFileImpl>::readSamples(std::span<std::int32_t> output) {
  return m_impl.readSamples(output);
}

template <typename AudioFileImpl> std::size_t AudioFileTypeEraser<AudioFileImpl>::readSamples(std::span<float> output) {
  return m_impl.readSamples(output);
}

template <typename AudioFileImpl>
std::size_t AudioFileTypeEraser<AudioFileImpl>::readSamples(std::span<double> output) {
  return m_impl.readSamples(output);
}
