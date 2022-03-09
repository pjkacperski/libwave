#pragma once

#include "../AudioFile.h"
#include "../DefaultNormalization.h"
#include "RiffHeader.h"
#include "UncompressedWaveFile.h"
#include "UnknownChunk.h"
#include "WaveAudioFile.h"
#include <filesystem>
#include <memory>
#include <tuple>
#include <vector>

class WaveFileUtil {
public:
  static RiffHeader readRiff(std::ifstream& stream);
  static std::tuple<RiffHeader, std::vector<UnknownChunk>> readRiffAndUnknows(std::ifstream& stream);

  template <typename Normalization = DefaultNormalization>
  static std::unique_ptr<AudioFile> openFile(const std::filesystem::path& path);

  template <typename Normalization = DefaultNormalization>
  static std::unique_ptr<AudioFile> openPcmFile(std::ifstream stream, const Fmt& fmt, const OffsetAndSize& dataChunk);

  template <typename Normalization = DefaultNormalization>
  static std::unique_ptr<AudioFile> openIeeeFloatFile(std::ifstream stream, const Fmt& fmt,
                                                      const OffsetAndSize& dataChunk);
};

template <typename Normalization> std::unique_ptr<AudioFile> WaveFileUtil::openFile(const std::filesystem::path& path) {
  std::ifstream stream{path, std::ios::binary};
  if (!stream) {
    throw std::invalid_argument{"path"};
  }
  auto header = readRiff(stream);
  if (!header.fmt) {
    throw std::runtime_error{"invalid file (no fmt chunk)"};
  }
  if (!header.dataChunk) {
    throw std::runtime_error{"invalid file (no data chunk)"};
  }
  switch (header.fmt->format) {
  case WaveFormat::Pcm:
    return openPcmFile<Normalization>(std::move(stream), *header.fmt, *header.dataChunk);
  case WaveFormat::IeeeFloat:
    return openIeeeFloatFile<Normalization>(std::move(stream), *header.fmt, *header.dataChunk);
  default:
    throw std::runtime_error{"invalid file (unsupported format)"};
  }
}

template <typename Normalization>
std::unique_ptr<AudioFile> WaveFileUtil::openPcmFile(std::ifstream stream, const Fmt& fmt,
                                                     const OffsetAndSize& dataChunk) {
  switch (fmt.bitsPerSample) {
  case 8:
    return std::make_unique<WaveAudioFile<UncompressedWaveFile<std::uint8_t, Normalization>>>(
        std::move(stream), dataChunk.size, WaveFormat::Pcm, fmt.channels, fmt.samplesPerSecond, 8u);
  case 16:
    return std::make_unique<WaveAudioFile<UncompressedWaveFile<std::int16_t, Normalization>>>(
        std::move(stream), dataChunk.size, WaveFormat::Pcm, fmt.channels, fmt.samplesPerSecond, 16u);
  case 32:
    return std::make_unique<WaveAudioFile<UncompressedWaveFile<std::int32_t, Normalization>>>(
        std::move(stream), dataChunk.size, WaveFormat::Pcm, fmt.channels, fmt.samplesPerSecond, 32u);
  default:
    throw std::runtime_error{"invalid file (unsupported bits per sample)"};
  }
}

template <typename Normalization>
std::unique_ptr<AudioFile> WaveFileUtil::openIeeeFloatFile(std::ifstream stream, const Fmt& fmt,
                                                           const OffsetAndSize& dataChunk) {
  switch (fmt.bitsPerSample) {
  case 32:
    return std::make_unique<WaveAudioFile<UncompressedWaveFile<float, Normalization>>>(
        std::move(stream), dataChunk.size, WaveFormat::IeeeFloat, fmt.channels, fmt.samplesPerSecond, 32u);
  default:
    throw std::runtime_error{"invalid file (unsupported bits per sample)"};
  }
}
