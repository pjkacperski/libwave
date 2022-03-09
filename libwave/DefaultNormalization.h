#pragma once

#include "TypeTag.h"
#include <limits>
#include <type_traits>

struct DefaultNormalization {
  template <typename From, typename To> static To normalize(const From& value) {
    return normalize(value, TypeTag<From, To>{});
  }

  // T -> T, no conversion needed
  template <typename T> static T normalize(const T& value, TypeTag<T, T>) {
    return value;
  }

  // integral type -> floating point type (-1, 1)
  template <typename From, typename To>
  static std::enable_if_t<std::is_integral_v<From> && std::is_floating_point_v<To>, To> normalize(const From& value,
                                                                                                  TypeTag<From, To>) {
    if constexpr (std::is_signed_v<From>) {
      return value < 0 ? static_cast<To>(-value) / std::numeric_limits<From>::min()
                       : static_cast<To>(value) / std::numeric_limits<From>::max();
    } else {
      return (static_cast<To>(value) / std::numeric_limits<From>::max()) * 2 - 1;
    }
  }

  // floating point type (-1, 1) -> integral type
  template <typename From, typename To>
  static std::enable_if_t<std::is_floating_point_v<From> && std::is_integral_v<To>, To> normalize(const From& value,
                                                                                                  TypeTag<From, To>) {
    if constexpr (std::is_signed_v<To>) {
      return value < 0 ? -value * std::numeric_limits<To>::min() : value * std::numeric_limits<To>::max();
    } else {
      return (value + 1) / 2 * std::numeric_limits<To>::max();
    }
  }

  // float -> double, doble -> float
  // clang-format off
  template <typename From, typename To> 
  static std::enable_if_t<(std::is_same_v<From, float> || std::is_same_v<From, double>) && 
                          (std::is_same_v<To, float> || std::is_same_v<To, double>) && 
                          !std::is_same_v<From, To>, To>
  normalize(const From& value, TypeTag<From, To>) {
    return static_cast<To>(value);
  }
  // clang-format on

  // all other types: fallback to From -> float -> To
  template <typename From, typename To>
  static std::enable_if_t<!std::is_floating_point_v<To> && !std::is_floating_point_v<From>, To>
  normalize(const From& value, TypeTag<From, To>) {
    return normalize(normalize(value, TypeTag<From, float>{}), TypeTag<float, To>{});
  }
};
