#include "AudioStream.h"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <libwave/AudioFileUtil.h>
#include <portaudiocpp/AutoSystem.hxx>

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "invalid arguments" << std::endl;
    return -1;
  }
  try
  {
    portaudio::AutoSystem portAudioHandle;
    std::condition_variable cv;
    std::atomic<bool> playbackFinished{false};
    AudioStream stream{AudioFileUtil::openFile(argv[1]), [&]() {
                         playbackFinished = true;
                         cv.notify_all();
                       }};
    stream.start();
    std::mutex mutex;
    while (!playbackFinished)
    {
      std::unique_lock lock{mutex};
      cv.wait_for(lock, std::chrono::milliseconds{250});
    }
    stream.stop();
  }
  catch (const std::exception& e)
  {
    std::cout << e.what() << std::endl;
    return -1;
  }
  return 0;
}
