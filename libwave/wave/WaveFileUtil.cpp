#include "WaveFileUtil.h"
#include "UncompressedWaveFile.h"
#include "UnknownChunk.h"
#include "WaveAudioFile.h"
#include <memory>
#include <vector>

static void read(std::ifstream& stream, char* ptr, std::size_t count) {
  if (!stream.read(ptr, count)) {
    throw std::runtime_error{"error while reading stream"};
  }
}

template <typename T> void read(std::ifstream& stream, T& value) {
  static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
  read(stream, reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T> T read(std::ifstream& stream) {
  T value{};
  read(stream, value);
  return value;
}

static std::string readText(std::ifstream& stream, std::size_t size) {
  if (size < 1) {
    return "";
  }
  auto buffer = std::make_unique<char>(size);
  if (!stream.read(buffer.get(), size)) {
    throw std::runtime_error{"error while reading stream"};
  }
  return std::string{buffer.get(), size};
}

static void skipText(std::ifstream& stream, const std::string& text) {
  auto value = readText(stream, text.size());
  if (text != value) {
    throw std::runtime_error{"invalid data in stream"};
  }
}

RiffHeader WaveFileUtil::readRiff(std::ifstream& stream) {
  return std::get<RiffHeader>(readRiffAndUnknows(stream));
}

std::tuple<RiffHeader, std::vector<UnknownChunk>> WaveFileUtil::readRiffAndUnknows(std::ifstream& stream) {
  skipText(stream, "RIFF");
  RiffHeader riff{};
  read(stream, riff.size);
  skipText(stream, "WAVE");
  std::vector<UnknownChunk> unknownChunks;
  // break on first data chunk or eof
  while (stream) {
    std::string chunkId;
    try {
      chunkId = readText(stream, 4);
    } catch (const std::runtime_error& e) {
      if (stream.eof()) {
        break;
      }
      throw e;
    }
    if (chunkId == "fmt ") {
      auto size = read<std::uint32_t>(stream);
      auto& fmt = riff.fmt.emplace();
      fmt.streamInfo.emplace(stream.tellg(), size);
      read(stream, fmt.format);
      read(stream, fmt.channels);
      read(stream, fmt.samplesPerSecond);
      read(stream, fmt.averageBytesPerSecond);
      read(stream, fmt.blockAlign);
      read(stream, fmt.bitsPerSample);
      if (size >= 18) {
        auto extensionSize = read<std::int16_t>(stream);
        if (extensionSize > 0) {
          if (extensionSize == 22) {
            auto& extension = fmt.extension.emplace<Fmt::Extension>();
            read(stream, extension.validBitsPerSample);
            read(stream, extension.channelMask);
            read(stream, extension.subFormat);
          } else {
            auto& extension = fmt.extension.emplace<std::vector<char>>(extensionSize);
            read(stream, extension.data(), extension.size());
          }
        }
      }
    } else if (chunkId == "fact") {
      auto size = read<std::uint32_t>(stream);
      if (size != 4) {
        throw std::runtime_error{"invalid data in stream"};
      }
      auto& fact = riff.fact.emplace();
      fact.streamInfo.emplace(stream.tellg(), size);
      read(stream, fact.sampleLength);
    } else if (chunkId == "data") {
      auto size = read<std::uint32_t>(stream);
      riff.dataChunk.emplace(stream.tellg(), size);
      break;
    } else { // unknown sub-chunk id
      auto size = read<std::uint32_t>(stream);
      if (!stream.seekg(size, std::ios::cur)) {
        throw std::runtime_error{"invalid data in stream"};
      }
      unknownChunks.emplace_back(std::move(chunkId), OffsetAndSize{stream.tellg(), size});
    }
  }
  return std::make_tuple(std::move(riff), std::move(unknownChunks));
}
