#pragma once

#include "OffsetAndSize.h"
#include "WaveFormat.h"
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

struct Fmt
{
  struct Extension
  {
    std::uint16_t validBitsPerSample;
    std::uint32_t channelMask;
    std::uint8_t subFormat[16];
  };

  std::optional<OffsetAndSize> streamInfo;
  WaveFormat format;
  std::uint16_t channels;
  std::uint32_t samplesPerSecond;
  std::uint32_t averageBytesPerSecond;
  std::uint16_t blockAlign;
  std::uint16_t bitsPerSample;
  std::variant<std::monostate, Extension, std::vector<char>> extension;
};
