#pragma once

#include "OffsetAndSize.h"
#include <cstdint>
#include <optional>

struct Fact {
  std::optional<OffsetAndSize> streamInfo;
  std::uint32_t sampleLength;
};