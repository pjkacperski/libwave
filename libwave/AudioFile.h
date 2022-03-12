#pragma once

#include <cstdint>
#include <span>

class AudioFile {
public:
  virtual ~AudioFile() = default;

  virtual std::size_t channels() const = 0;
  virtual std::size_t sampleRate() const = 0;

  virtual std::size_t samplesLeft() const = 0;

  virtual std::size_t readSamples(std::span<std::uint8_t> outout) = 0;
  virtual std::size_t readSamples(std::span<std::int16_t> outout) = 0;
  virtual std::size_t readSamples(std::span<std::int32_t> output) = 0;
  virtual std::size_t readSamples(std::span<float> output) = 0;
  virtual std::size_t readSamples(std::span<double> output) = 0;
};
