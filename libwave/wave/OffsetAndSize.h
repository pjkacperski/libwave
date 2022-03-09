#pragma once

#include <fstream>

struct OffsetAndSize {
  std::ios::pos_type streamOffset;
  std::size_t size;
};
