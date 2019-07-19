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
  size_t numToRead = nBytes;
  size_t offset    = 0;
  
  // If there's a partial ring item put it in the front of the buffer.
  // It's a logic error for it not to fit.

  if (m_partialItemSize) {
    if (m_partialItemSize >= numToRead) {
      throw std::logic_error("You need to declare bigger buffers for this data\n");
    }
    memcpy(result.s_pData, m_pPartialItem, m_partialItemSize);
    numToRead -= m_partialItemSize;
    offset     = m_partialItemSize;  // Read starting here in the buffer.
    m_partialItemSize = 0;	// We absorbed the partial itemsize..
  }

  // We can read the remainder of the item.

  char* pDest = reinterpret_cast<char*>(result.s_pData);
  pDest += offset;

  ssize_t nRead = ::read(m_nFd, pDest, numToRead);

  // if < 0, error,
  // if ==   EOF,
  // Anything else is a good read of nRead (our buffer has nRead + offset bytes).

  if (nRead < 0) {
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
			      "Doing bulk read from file");
  } else if (nRead == 0) {
    assert(offset == 0);     // if there was a partial item that's a bug
  } else {
    // We've added data to the buffer.   
    // - Compute How many complete ring items we have, containing how many bytes.
    // - Determine If we have a partial ring item at the end of the block.
    // - Save any partial item.
    //  It's going to be convenient to have a uint32_t pointer (to pick up the ring item sizes.
    //  and a char pointer to step through the buffer.

    size_t bufferBytes = nRead + offset;
    char* pBuffer = reinterpret_cast<char*>(result.s_pData);
    while(bufferBytes) {
      std::uint32_t* pItem = reinterpret_cast<std::uint32_t*>(pBuffer);
      std::uint32_t itemSize = *pItem;

      // Does it fully fit:

      if (itemSize <= bufferBytes) {
	result.s_nItems++;
	result.s_nBytes += itemSize;
	
	bufferBytes -= itemSize;          // Book keeping to advance to the
	pBuffer += itemSize;              // next item.
      } else {				  // We're at a partial item:
	savePartialItem(pBuffer, bufferBytes); // Save that item.
	bufferBytes = 0;		       // Done with the buffer.
      }
    }
    
    return result;
  }

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
    delete []m_pPartialItem;  	// No-op for null pointer.
    m_pPartialItem = new char[nBytes];
    m_partialItemBlockSize = nBytes;
  }
  memcpy(m_pPartialItem, pItem, nBytes);
  m_partialItemSize = nBytes;
  
}
