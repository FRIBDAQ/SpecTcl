/**
 *  Implement the block mode ring pusher.
 *  This version of the file reader blocks of data
 *  that contain many ring items.  The number of complete
 *  ring items is computed and the partial ring item is
 *  saved for the next read.
 */
#include "CRingFileBlockReader.h"
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <errno.h>
#include <string.h>
#include <system_error>
#include <assert.h>
#include <set>

static std::set<int> okErrors; // Acceptable errors in I/O operations.

/**
 * Constructor
 *    Open the file.
 * 
 * @param filename - name of the file to open.
 */
CRingFileBlockReader::CRingFileBlockReader(const int fd) :
  m_partialItemSize(0), m_partialItemBlockSize(0), m_pPartialItem(nullptr)
{
  m_nFd = fd;
  if (m_nFd < 0) {
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
			    "Opening the ring item file");
  }
}
/**
 *   Destructor
 *   - Close the file.
 *   - release any m_pPartialItem storage.
 */
CRingFileBlockReader::~CRingFileBlockReader()
{
  //  close(m_nFd);
  free(m_pPartialItem);
}

static bool 
badError(int error)
{
  // Stock the okErrors set if empty:

  if (okErrors.empty())
    {
      okErrors.insert(EAGAIN);
      okErrors.insert(EWOULDBLOCK);
      okErrors.insert(EINTR);
    }

  // Not in the set -> true.

  return (okErrors.count(error) == 0);
}

size_t readBlock(int fd, void* pBuffer,  size_t nBytes)
{
  return ::read(fd, pBuffer, nBytes);
}

/**
 * read a block of data and let the caller know how many full ring items there are
 * as well as how many bytes there are in those ringitems.
 *
 * @param nBytes - Maximum number of bytes we'll do (read size)
 * @return DataDesription - describes the data read, the s_pData was malloced
 *        and must be freed.
 */
CRingFileBlockReader::DataDescriptor
CRingFileBlockReader::read(size_t nBytes)
{
  DataDescriptor result = {0, 0, malloc(nBytes)};  
  if (!result.s_pData) {
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
			    "Initial buffer allocation");
  }
  
  // We read until one of the following conditions is true:
  // - readBlock returns 0 or errror indicating the program better exit.
  // - we have at least one ring item in the user buffer.
  // - The user buffer can't hold the partial ring item we have (error)..
  // - The read is complete in which case any partial ring item read
  //   is stored to be prepended to the next read operation.
  
  // If there's a partial item and the full item won't fit in the
  // user buffer we're done with an std::logic_error:
  
  if (m_partialItemSize && (m_partialItemSize > nBytes)) {
    throw std::logic_error("The buffer is not big enough for the partial item we have");
  }
  // IF there's a partial item copy it in to the buffer and figure out
  // where we need to append data and how much of a read we have left.
  
  int numToRead = nBytes;           // Will be the remaining buffer size.
  uint8_t* pNextBytes = static_cast<uint8_t*>(result.s_pData); // read here.
  uint8_t* pFirstByte = pNextBytes; // For distance calculations.
  uint32_t* pFront    = reinterpret_cast<uint32_t*>(pFirstByte); // For size.
  
  if (m_partialItemSize) {
    memcpy(pNextBytes, m_pPartialItem, m_partialItemSize);
    pNextBytes += m_partialItemSize;
    numToRead  -= m_partialItemSize;
    
    m_partialItemSize = 0;                 // no partial item now.
  }
  bool done = false;
  while (!done) {
    ssize_t nRead = readBlock(m_nFd, pNextBytes, numToRead);
    if (nRead < 0) {
      if ((errno != EINTR ) && (errno != EAGAIN)) {
        throw std::system_error(
				std::make_error_code(static_cast<std::errc>(errno)),
          "Reading a block of data."
				);
      } else {
        continue;                  // do the next loop pass.
      }
    }
    if (nRead == 0) {
      done = true;                // End file.
    } else {
      numToRead -= nRead;          // In case we're not done yet...
      pNextBytes += nRead;
      
      // If we have at least a ring item figure out how many, store any partial
      // and indicate done-ness.  Otherwise we need another pass.
      // Note that we must again check the item size against the buffer size.
      // In doing all this there's an implicit assumption that we'll get at
      // least sizeof(uint32_t) in reads, else we'll not get the size.
      
      assert(nRead >= sizeof(uint32_t));
      
      if (*pFront > nBytes) {
	std::cout << "Ring item size " << *pFront << " > buffer size " << nBytes << std::endl;
        throw std::logic_error("Buffer size is too small for a ring item");
      }
      if ((pNextBytes - pFirstByte) >= *pFront) {  // we have at least a ring item.
        uint32_t* p     = pFront;
        uint32_t  n     = pNextBytes - pFirstByte; // Number of bytes read.
        while ((n >= *p) && (n > 0)) {                          // There's still a ring item.
          result.s_nItems++;
          n  -= *p;
          result.s_nBytes += *p;
          p   = reinterpret_cast<uint32_t*>(
					    (reinterpret_cast<uint8_t*>(p) + *p)
					    );                                    // Next item.
        }
        // If there's any partial item we'ver got to squirrel it away:
        
        if (n) {
          savePartialItem(p, n);
        }
        
        done = true;
      }    
    }
    
    
  }
  if (result.s_nBytes == 0) {
    free(result.s_pData);
    result.s_pData = nullptr;
  }
  return result;

}
/////////////////////////////////////////////////////////////////
// Private utility methods:

/**
 * Save the partial item in the m_pPartialItem block.  If necessary
 * That's resized to fit.
 *   @param pItem - pointer to the partial item.
 *   @param nBytes - number of bytes of partial item.
 *
 *  @note The dance we do here is intended to ensure we only 
 *        sometimes need to allocated storage for the partial.
 */
void
CRingFileBlockReader::savePartialItem(void* pItem, size_t nBytes)
{
  if (m_partialItemBlockSize < nBytes) {
    delete []m_pPartialItem;  // No-op for null pointer.
    m_pPartialItem = new char[nBytes];
    m_partialItemBlockSize = nBytes;
  }
  memcpy(m_pPartialItem, pItem, nBytes);
  m_partialItemSize = nBytes;
}
