#ifndef CRINGFILEBLOCKREADER_H
#define CRINGFILEBLOCKREADER_H
#include "CRingBlockReader.h"

#include <cstdint>
#include <stddef.h>
#include <unistd.h>



class CRingFileBlockReader : public CRingBlockReader
{
private:
  int m_nFd;

  
  

public:
  CRingFileBlockReader(const char* filename);
  CRingFileBlockReader(int fd);
  virtual ~CRingFileBlockReader();

protected:
  ssize_t readBlock(void* pBuffer, size_t nBytes);
};


#endif
