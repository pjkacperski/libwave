#pragma once

#include "PortAudioOutputStream.h"
#include <libwave/AudioFile.h>
#include <memory>
#include <optional>

class AudioStream
{
public:
  AudioStream(std::unique_ptr<AudioFile> file, std::function<void()> playbackFinished);
  ~AudioStream();

  void start();
  void stop();

private:
  bool dataCallback(float* outputBuffer, unsigned long framesPerBuffer, PaStreamCallbackFlags flags);
  void finishedCallback();

  std::unique_ptr<AudioFile> m_file;
  std::function<void()> m_playbackFinished;
  std::optional<PortAudioOutputStream<float>> m_stream;
};
