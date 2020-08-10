#ifndef CRINGFILEBLOCKREADER_H
#define CRINGFILEBLOCKREADER_H
#include <cstdint>
#include <stddef.h>


class CRingFileBlockReader {
private:
  int m_nFd;
  std::uint32_t  m_partialItemSize;       // How much data is in the partial item.
  std::uint32_t  m_partialItemBlockSize;	// How big is the buffer pointed to by m_pPartialItem.
  char* m_pPartialItem;

public:
  typedef struct _DataDescriptor {
    std::uint32_t s_nBytes;
    std::uint32_t s_nItems;
    void*         s_pData;
  } DataDescriptor, *pDataDescriptor;

public:
  CRingFileBlockReader(const int fd);
  virtual ~CRingFileBlockReader();

  DataDescriptor read(size_t nBytes);
private:
  void savePartialItem(void* pItem, size_t nBytes);
};


#endif
