#pragma once

#include "Fact.h"
#include "Fmt.h"
#include "OffsetAndSize.h"
#include <optional>

struct RiffHeader {
  std::uint32_t size;
  std::optional<Fmt> fmt;
  std::optional<Fact> fact;
  std::optional<OffsetAndSize> dataChunk;
};
