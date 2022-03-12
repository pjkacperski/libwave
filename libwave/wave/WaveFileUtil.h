#pragma once

#include "../AudioFile.h"
#include "RiffHeader.h"
#include "UncompressedWaveFile.h"
#include "UnknownChunk.h"
#include <filesystem>
#include <memory>
#include <tuple>
#include <vector>

class WaveFileUtil
{
public:
  static RiffHeader readRiff(std::ifstream& stream);
  static std::tuple<RiffHeader, std::vector<UnknownChunk>> readRiffAndUnknows(std::ifstream& stream);

  static std::unique_ptr<AudioFile> openFile(const std::filesystem::path& path);
};
