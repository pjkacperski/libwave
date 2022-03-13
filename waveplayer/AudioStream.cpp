#include "AudioStream.h"
#include <portaudiocpp/Device.hxx>
#include <portaudiocpp/DirectionSpecificStreamParameters.hxx>
#include <portaudiocpp/StreamParameters.hxx>
#include <portaudiocpp/System.hxx>
#include <span>
#include <stdexcept>

AudioStream::AudioStream(std::unique_ptr<AudioFile> file, std::function<void()> playbackFinished)
    : m_file{std::move(file)}, m_playbackFinished{std::move(playbackFinished)}
{
}

AudioStream::~AudioStream()
{
  stop();
}

void AudioStream::start()
{
  if (m_stream)
  {
    return;
  }
  auto& outDevice = portaudio::System::instance().defaultOutputDevice();
  if (outDevice.index() == paNoDevice)
  {
    throw std::runtime_error{"PortAudio could not find any output devices"};
  }
  portaudio::DirectionSpecificStreamParameters params(outDevice, static_cast<int>(m_file->channels()),
                                                      portaudio::FLOAT32, true, outDevice.defaultLowOutputLatency(),
                                                      nullptr);
  constexpr auto FRAMES_PER_BUFFER = 16 * 1024;
  m_stream.emplace(
      params, static_cast<double>(m_file->sampleRate()), FRAMES_PER_BUFFER, paClipOff | paDitherOff,
      [this](auto* outputBufferPtr, auto framesPerBuffer, const auto*, auto flags) {
        if (dataCallback(outputBufferPtr, framesPerBuffer, flags))
        {
          return PaStreamCallbackResult::paContinue;
        }
        return PaStreamCallbackResult::paComplete;
      },
      [this]() { finishedCallback(); });
  m_stream->start();
}

void AudioStream::stop()
{
  if (m_stream)
  {
    m_stream->stop();
    m_stream.reset();
  }
}

bool AudioStream::dataCallback(float* outBuffer, unsigned long framesPerBuffer, PaStreamCallbackFlags)
{
  const auto count = framesPerBuffer * m_file->channels();
  auto read = m_file->readSamples(std::span(outBuffer, count));
  if (read < count)
  {
    std::fill_n(outBuffer + read, count - read, 0.0f);
    return false;
  }
  return true;
}

void AudioStream::finishedCallback()
{
  if (m_playbackFinished)
  {
    m_playbackFinished();
  }
}
