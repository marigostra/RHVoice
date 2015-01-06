
#ifndef RHVOICE_CLI_CLIENT_H
#define RHVOICE_CLI_CLIENT_H

#include "core/client.hpp"
#include"WaveFile.h"

class Client: public RHVoice::client
{
public:
  Client(const WaveFile& waveFile)
    : m_waveFile(waveFile)  {}
  virtual ~Client() {}

public:
  unsigned int get_audio_buffer_size() const
  {
    return 100;
  }

  bool play_speech(const short* samples,std::size_t count)
  {
    assert(samples);
    if (count < 1)
      return true;
    m_waveFile.onSamples((const char*)samples, count * sizeof(short));
    return true;
  }

  RHVoice::event_mask get_supported_events() const
  {
    return 0;
  }

  bool process_mark(const std::string& name)
  {
    return true;
  }

  bool play_audio(const std::string& src)
  {
    return true;
  }

  int get_sample_rate() const
  {
    return 16000;
  }

  bool set_sample_rate(int sample_rate)
  {
    std::cerr << "Sample rate " << sample_rate << std::endl;
    return true;
  }

  bool sentence_starts(std::size_t position,std::size_t length)
  {
    return true;
  }

  bool sentence_ends(std::size_t position,std::size_t length)
  {
    return true;
  }

  bool word_starts(std::size_t position,std::size_t length)
  {
    return true;
  }

  bool word_ends(std::size_t position,std::size_t length)
  {
    return true;
  }

private:
  const WaveFile& m_waveFile;
}; //class Client;

#endif //RHVOICE_CLI_CLIENT_H;
