
#include<assert.h>
#include<string.h>
#include<stdint.h>
#include<string>

#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>

#include"WaveFile.h"

bool WaveFile::openOnDisk(const std::string& fileName, std::string& errMsg)
{
  assert(!fileName.empty());
  assert(m_fd < 0);
  m_fd = open(fileName.c_str(), O_CREAT | O_WRONLY, 0644);
  if (m_fd < 0)
    {
      errMsg =  strerror(errno);
      return 0;
    }
  return 1;
}

void WaveFile::writeHeader(size_t sampleRate) const
{
  assert(m_fd >= 0);
  const uint32_t byteRate=2 * sampleRate;
  unsigned char header[]={
    /* RIFF header */
    'R','I','F','F',            /* ChunkID */
    /* We cannot determine this number in advance */
    /* This is what espeak puts in this field when writing to stdout */
    0x24,0xf0,0xff,0x7f,        /* ChunkSize */
    'W','A','V','E',            /* Format */
    /* fmt */
    'f','m','t',' ',            /* Subchunk1ID */
    16,0,0,0,                 /* Subchunk1Size */
    1,0,                        /* AudioFormat (1=PCM) */
    1,0,                        /* NumChannels */
    0,0,0,0,                    /* SampleRate */
    0,0,0,0,                    /* ByteRate */
    2,0,                        /* BlockAlign */
    16,0,                       /* BitsPerSample */
    /* data */
    'd','a','t','a',          /* Subchunk2ID */
    /* Again using espeak as an example */
    0x00,0xf0,0xff,0x7f};     /* Subchunk2Size */
  /* Write actual sample rate */
  *reinterpret_cast<uint32_t*>(header+24) = sampleRate;
  /* Write actual byte rate */
  *reinterpret_cast<uint32_t*>(header+28) = byteRate;
  write(m_fd, header, sizeof(header));//FIXME:Check the result;
}

void WaveFile::onSamples(const char* buf, size_t numBytes) const
{
  assert(m_fd >= 0);
  write(m_fd, buf, numBytes);//FIXME:Check the result;
}
