#include <alsa/asoundlib.h>
#include <fstream>
#include <iostream>
#include <libwave/AudioFileUtil.h>
#include <vector>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    throw std::runtime_error{"invalid arguments"};
  }
  auto file = AudioFileUtil::openFile(argv[1]);
  snd_pcm_t* handle = nullptr;
  snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

  snd_pcm_hw_params_t* params = nullptr;
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(handle, params);

  snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_FLOAT_LE);
  snd_pcm_hw_params_set_channels(handle, params, file->channels());
  snd_pcm_hw_params_set_rate(handle, params, file->sampleRate(), 0);
  snd_pcm_hw_params(handle, params);

  snd_pcm_uframes_t frames;
  int dir;
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  std::vector<float> buffer(frames * file->channels());
  do {
    auto readSamples = file->readSamples(std::span(buffer));
    if (readSamples > 0) {
      auto ret = snd_pcm_writei(handle, buffer.data(), readSamples / file->channels());
      if (ret < 0)
        ret = snd_pcm_recover(handle, ret, 0);
      if (ret < 0) {
        std::cerr << "snd_pcm_writei failed: " << snd_strerror(ret) << std::endl;
        break;
      }
    }
  } while (file->bytesLeft() >= file->channels() * sizeof(float));
  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  return 0;
}
