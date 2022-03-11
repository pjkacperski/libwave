#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

struct ScalarNormalization {

  // T -> T, no conversion needed
  template <typename To, typename From>
  static constexpr std::enable_if_t<std::is_same_v<To, From>, To> normalize(From value);

  // integral type -> floating point type (-1, 1)
  template <typename From, typename To>
  static constexpr std::enable_if_t<std::is_integral_v<From> && std::is_floating_point_v<To>, To> normalize(From value);

  // floating point type (-1, 1) -> integral type
  template <typename From, typename To>
  static constexpr std::enable_if_t<std::is_floating_point_v<From> && std::is_integral_v<To>, To> normalize(From value);

  // floating point T -> floating point U
  template <typename From, typename To>
  static constexpr std::enable_if_t<
      std::is_floating_point_v<From> && std::is_floating_point_v<To> && !std::is_same_v<From, To>, To>
  normalize(From value);

  // all other types: fallback to From -> float -> To
  template <typename From, typename To>
  static constexpr std::enable_if_t<!std::is_floating_point_v<To> && !std::is_floating_point_v<From>, To>
  normalize(From value);

  template <typename Unsigned> struct UnsignedToSigned {};
};

template <> struct ScalarNormalization::UnsignedToSigned<std::uint8_t> { using type = std::int16_t; };
template <> struct ScalarNormalization::UnsignedToSigned<std::uint16_t> { using type = std::int32_t; };
template <> struct ScalarNormalization::UnsignedToSigned<std::uint32_t> { using type = std::int64_t; };
template <> struct ScalarNormalization::UnsignedToSigned<std::uint64_t> { using type = double; };

template <typename To, typename From>
constexpr std::enable_if_t<std::is_same_v<To, From>, To> ScalarNormalization::normalize(From value) {
  return value;
}

template <typename From, typename To>
std::enable_if_t<std::is_integral_v<From> && std::is_floating_point_v<To>, To> constexpr ScalarNormalization::normalize(
    From value) {
  if constexpr (std::is_unsigned_v<From>) {
    return static_cast<To>(static_cast<typename UnsignedToSigned<From>::type>(value) -
                           std::numeric_limits<From>::max() / 2) /
           (std::numeric_limits<From>::max() / 2);
  } else {
    return value < 0 ? static_cast<To>(-value) / std::numeric_limits<From>::min()
                     : static_cast<To>(value) / std::numeric_limits<From>::max();
  }
}

template <typename From, typename To>
constexpr std::enable_if_t<std::is_floating_point_v<From> && std::is_integral_v<To>, To>
ScalarNormalization::normalize(From value) {
  if constexpr (std::is_unsigned_v<To>) {
    return static_cast<To>((value + 1) / 2 * std::numeric_limits<From>::max());
  } else {
    return value < 0 ? static_cast<To>(-value * std::numeric_limits<To>::min())
                     : static_cast<To>(value * std::numeric_limits<To>::max());
  }
}

template <typename From, typename To>
constexpr std::enable_if_t<std::is_floating_point_v<From> && std::is_floating_point_v<To> && !std::is_same_v<From, To>,
                           To>
ScalarNormalization::normalize(From value) {
  return static_cast<To>(value);
}

template <typename From, typename To>
constexpr std::enable_if_t<!std::is_floating_point_v<To> && !std::is_floating_point_v<From>, To>
ScalarNormalization::normalize(From value) {
  return normalize<float, To>(normalize<From, float>(value));
}
