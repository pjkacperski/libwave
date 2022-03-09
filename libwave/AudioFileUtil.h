#pragma once

#include "AudioFile.h"
#include <filesystem>

class AudioFileUtil {
public:
  enum class FileType
  {
    WavFile
  };

  static std::unique_ptr<AudioFile> openFile(const std::filesystem::path& path);
  static std::unique_ptr<AudioFile> openFile(const std::filesystem::path& path, FileType type);
};
