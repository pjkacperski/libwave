#pragma once

#include <array>
#include <cmath>
#include <cstdint>

namespace g711
{
namespace detail
{
constexpr std::int16_t decodeAlawByte(std::uint8_t logval)
{
  logval ^= 0b0101'0101u;
  auto exponent = (logval & 0x70u) >> 4;
  auto mantissa = (logval & 0x0Fu) << 3; // mantissa * 2^3 (for 13 bit -> left shifted 16 bit conversion)
  auto result = 0;
  if (exponent > 0)
  {
    result = mantissa + 128 + 4 << exponent; // 128 = 16 << 3, 4 = 1/2 quantisation step
  }
  else
  {
    result = mantissa + 4 << 1; // 4 = 1/2 quantisation step
  }
  return logval < 128 ? -result : result; // sign bit is inversed
}

constexpr std::int16_t decodeUlawByte(std::uint8_t logval)
{
  logval ^= 0b1111'1111u;
  auto exponent = (logval & 0x70u) >> 4;
  auto mantissa = (logval & 0x0Fu) << 3; // 2 * mantissa * 2^2 (for 14 bit -> left shifted 16 bit conversion)
  auto result = (33 << 2) + mantissa << exponent;
  return logval > 127 ? -result + (33 << 2) : result - (33 << 2);
}

template <typename Decoder> constexpr auto generateLookupTable(Decoder&& decoder)
{
  using ResultType = decltype(std::forward<Decoder>(decoder)(0));
  std::array<ResultType, 256> lookup{};
  for (std::size_t i = 0; i < 256; ++i)
  {
    lookup[i] = std::forward<Decoder>(decoder)(static_cast<std::uint8_t>(i));
  }
  return lookup;
}
} // namespace detail

enum class Encoding
{
  Alaw,
  Ulaw
};

struct Alaw
{
  static constexpr std::int16_t decode(std::uint8_t logval)
  {
    constexpr auto lookup = detail::generateLookupTable(detail::decodeAlawByte);
    return lookup[logval];
  }

  static constexpr auto encoding()
  {
    return Encoding::Alaw;
  }
};

struct Ulaw
{
  static constexpr std::int16_t decode(std::uint8_t logval)
  {
    constexpr auto lookup = detail::generateLookupTable(detail::decodeUlawByte);
    return lookup[logval];
  }

  static constexpr auto encoding()
  {
    return Encoding::Ulaw;
  }
};
} // namespace g711
