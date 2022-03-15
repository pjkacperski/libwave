#include "AlawWaveFile.h"

AlawWaveFile::AlawWaveFile(std::ifstream stream, std::size_t totalBytes, std::size_t channels, std::size_t sampleRate,
                           std::size_t bitsPerSample)
    : AudioFileBase{std::move(stream), totalBytes, channels, sampleRate, bitsPerSample}
{
}

WaveFormat AlawWaveFile::format() const
{
  return WaveFormat::Alaw;
}

std::size_t AlawWaveFile::samplesLeft() const
{
  return rawBytesLeft();
}
