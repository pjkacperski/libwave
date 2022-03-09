#pragma once

#include "OffsetAndSize.h"
#include <string>

struct UnknownChunk {
  std::string id;
  OffsetAndSize streamInfo;
};
