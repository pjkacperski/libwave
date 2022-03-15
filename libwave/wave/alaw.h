#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <utility>

namespace alaw
{
namespace detail
{
constexpr std::int16_t decode(std::uint8_t logval)
{
  logval ^= 0x55u; // xor byte with 0b01010101
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

template <std::size_t... Is> constexpr auto generateDecodeTable(std::index_sequence<Is...>)
{
  static_assert(sizeof...(Is) == 256, "alaw decode table must have exactly 256 elements");
  return std::array<std::int16_t, 256>{decode(static_cast<std::uint8_t>(Is))...};
}

constexpr auto generateDecodeTable()
{
  return generateDecodeTable(std::make_index_sequence<256>());
}
} // namespace detail

constexpr std::int16_t decode(std::uint8_t logval)
{
  constexpr auto lookup = detail::generateDecodeTable();
  return lookup[logval];
}

} // namespace alaw
