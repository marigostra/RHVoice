
#ifndef RHVOICE_WAVE_FILE_H
#define RHVOICE_WAVE_FILE_H

class WaveFile
{
public:
  WaveFile()
    : m_fd (-1) {}
  ~WaveFile() {close();}

public:
  bool openOnDisk(const std::string& fileName, std::string& errMsg);

  void openAsStdOut()
  {
    assert(m_fd == -1);
    m_fd = 1;
  }

  void close()
  {
    if (m_fd < 0)
      return;
    ::close(m_fd);
    m_fd = -1;
  }

  void writeHeader(size_t sampleRate) const;
  void onSamples(const char* buf, size_t numBytes) const;

private:
  int m_fd;
}; //class WaveFile;

#endif //RHVOICE_WAVE_FILE_H;
