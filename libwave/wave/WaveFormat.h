#pragma once

#include <cstdint>

enum class WaveFormat : std::uint16_t
{
  Pcm = 0x0001,
  IeeeFloat = 0x0003,
  Alaw = 0x0006,
  Ulaw = 0x0007,
  Extensible = 0xFFFE
};
