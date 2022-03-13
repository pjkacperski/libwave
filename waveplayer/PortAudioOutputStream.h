#pragma once

#include <functional>
#include <portaudio.h>
#include <portaudiocpp/DirectionSpecificStreamParameters.hxx>
#include <portaudiocpp/Exception.hxx>

template <typename Output> class PortAudioOutputStream
{
public:
  using DataCallback = std::function<PaStreamCallbackResult(Output*, unsigned long, const PaStreamCallbackTimeInfo*,
                                                            PaStreamCallbackFlags)>;
  using FinishedCallback = std::function<void()>;

  PortAudioOutputStream(const portaudio::DirectionSpecificStreamParameters& params, double sampleRate,
                        std::size_t framesPerBuffer, PaStreamFlags flags, DataCallback dataCallback,
                        FinishedCallback finishedCallback);

  ~PortAudioOutputStream();

  void start();
  void stop();

private:
  void hookFinishedCallback();

  DataCallback m_dataCallback;
  FinishedCallback m_finishedCallback;
  PaStream* m_stream{nullptr};

  static int forwardDataCallback(const void*, void* outputBufferPtr, unsigned long framesPerBuffer,
                                 const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags flags, void* userData);
  static void forwardFinishedCallback(void* userData);
};

template <typename Output>
PortAudioOutputStream<Output>::PortAudioOutputStream(const portaudio::DirectionSpecificStreamParameters& params,
                                                     double sampleRate, std::size_t framesPerBuffer,
                                                     PaStreamFlags flags, DataCallback dataCallback,
                                                     FinishedCallback finishedCallback)
    : m_dataCallback{std::move(dataCallback)}, m_finishedCallback{std::move(finishedCallback)}
{
  auto result =
      Pa_OpenStream(&m_stream, portaudio::DirectionSpecificStreamParameters::null().paStreamParameters(),
                    params.paStreamParameters(), sampleRate, framesPerBuffer, flags, forwardDataCallback, this);
  if (result != PaErrorCode::paNoError)
  {
    throw portaudio::PaException{result};
  }
  hookFinishedCallback();
}

template <typename Output> PortAudioOutputStream<Output>::~PortAudioOutputStream()
{
  if (m_stream != nullptr)
  {
    Pa_CloseStream(m_stream);
    m_stream = nullptr;
  }
}

template <typename Output> void PortAudioOutputStream<Output>::start()
{
  auto result = Pa_StartStream(m_stream);
  if (result != PaErrorCode::paNoError)
  {
    throw portaudio::PaException{result};
  }
}

template <typename Output> void PortAudioOutputStream<Output>::stop()
{
  auto result = Pa_StopStream(m_stream);
  if (result != PaErrorCode::paNoError)
  {
    throw portaudio::PaException{result};
  }
}

template <typename Output> void PortAudioOutputStream<Output>::hookFinishedCallback()
{
  auto result = Pa_SetStreamFinishedCallback(m_stream, forwardFinishedCallback);
  if (result != PaErrorCode::paNoError)
  {
    throw portaudio::PaException{result};
  }
}

template <typename Output>
int PortAudioOutputStream<Output>::forwardDataCallback(const void*, void* outputBufferPtr,
                                                       unsigned long framesPerBuffer,
                                                       const PaStreamCallbackTimeInfo* timeInfo,
                                                       PaStreamCallbackFlags flags, void* userData)
{
  return static_cast<int>(reinterpret_cast<PortAudioOutputStream<Output>*>(userData)->m_dataCallback(
      reinterpret_cast<Output*>(outputBufferPtr), framesPerBuffer, timeInfo, flags));
}

template <typename Output> void PortAudioOutputStream<Output>::forwardFinishedCallback(void* userData)
{
  reinterpret_cast<PortAudioOutputStream<Output>*>(userData)->m_finishedCallback();
}
