#include "AudioFileUtil.h"
#include "wave/WaveFileUtil.h"
#include <algorithm>
#include <cctype>
#include <string>

std::unique_ptr<AudioFile> AudioFileUtil::openFile(const std::filesystem::path& path) {
  auto extension = path.extension().string();
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  if (extension == ".wav") {
    return openFile(path, FileType::WavFile);
  }
  throw std::runtime_error{"file has unrecognized extension"};
}

std::unique_ptr<AudioFile> AudioFileUtil::openFile(const std::filesystem::path& path, FileType type) {
  switch (type) {
  case FileType::WavFile:
    return WaveFileUtil::openFile(path);

  default:
    throw std::runtime_error{"invalid file type"};
  }
}
