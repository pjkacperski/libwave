#pragma once

#include "../AudioFile.h"

template <typename WaveFileImpl> class WaveAudioFile : public AudioFile {
public:
  template <typename... Args> WaveAudioFile(Args&&... args);

  ~WaveAudioFile() = default;

  std::size_t sampleRate() const override;
  std::size_t channels() const override;
  std::size_t bytesLeft() const override;

  std::size_t readSamples(std::span<std::uint8_t> outout) override;
  std::size_t readSamples(std::span<std::int16_t> outout) override;
  std::size_t readSamples(std::span<std::int32_t> output) override;
  std::size_t readSamples(std::span<float> output) override;
  std::size_t readSamples(std::span<double> output) override;

private:
  WaveFileImpl m_impl;
};

template <typename WaveFileImpl>
template <typename... Args>
WaveAudioFile<WaveFileImpl>::WaveAudioFile(Args&&... args) : m_impl{std::forward<Args>(args)...} {
}

template <typename WaveFileImpl> std::size_t WaveAudioFile<WaveFileImpl>::sampleRate() const {
  return m_impl.sampleRate();
}

template <typename WaveFileImpl> std::size_t WaveAudioFile<WaveFileImpl>::channels() const {
  return m_impl.channels();
}

template <typename WaveFileImpl> std::size_t WaveAudioFile<WaveFileImpl>::bytesLeft() const {
  return m_impl.bytesLeft();
}

template <typename WaveFileImpl> std::size_t WaveAudioFile<WaveFileImpl>::readSamples(std::span<std::uint8_t> output) {
  return m_impl.readSamples(output);
}

template <typename WaveFileImpl> std::size_t WaveAudioFile<WaveFileImpl>::readSamples(std::span<std::int16_t> output) {
  return m_impl.readSamples(output);
}

template <typename WaveFileImpl> std::size_t WaveAudioFile<WaveFileImpl>::readSamples(std::span<std::int32_t> output) {
  return m_impl.readSamples(output);
}

template <typename WaveFileImpl> std::size_t WaveAudioFile<WaveFileImpl>::readSamples(std::span<float> output) {
  return m_impl.readSamples(output);
}

template <typename WaveFileImpl> std::size_t WaveAudioFile<WaveFileImpl>::readSamples(std::span<double> output) {
  return m_impl.readSamples(output);
}
