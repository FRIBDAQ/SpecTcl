/**
 *  Implement the block mode ring pusher.
 *  This version of the file reader blocks of data
 *  that contain many ring items.  The number of complete
 *  ring items is computed and the partial ring item is
 *  saved for the next read.
 */
#include "CRingFileBlockReader.h"

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
#include <string>
#include <poll.h>


#include <iostream>

/**
 * Constructor
 *    Open the file.
 * 
 * @param filename - name of the file to open.
 */
CRingFileBlockReader::CRingFileBlockReader(const char* filename) 
{
  std::string fName(filename);
  if (fName == "-") {
    m_nFd = STDIN_FILENO;
  } else {
    m_nFd = open(filename, O_RDONLY);
  }
  if (m_nFd < 0) {
    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
			    "Opening the ring item file");
  }
}
/**
 *  constructor:
 *      @param fd  - file descriptor already open on the input file.
 *      @note this supplies the ability to do ring block reading from
 *            e.g. a socket or stdin for pipeline processing.
 */
CRingFileBlockReader::CRingFileBlockReader(int fd) :
  m_nFd(fd)
{
  
}
/**
 *   Destructor
 *   - Close the file.
 *   - release any m_pPartialItem storage.
 */
CRingFileBlockReader::~CRingFileBlockReader()
{
  close(m_nFd);
}

/**
 *  readBlock
 *     Waits until data is available on the input and then does a
 *     read.  The read timeout is long (I'd prefer it be forever), as
 *     eventually we'll either get data, get end runs or get an EOF.
 *
 * @param pBuffer - pointer to where the data goes.
 * @param nBytes  - Max number of bytes that can be read.
 * @return ssize_t - >=0 are the number of bytes read.  <0 an error
 *                   with the reason in errno.
 */
ssize_t
CRingFileBlockReader::readBlock(void* pBuffer, size_t nBytes)
{
  // wait for readability:
 
 
  pollfd polls = {m_nFd, POLLIN, 0, };
 // while (poll(&polls, 1, 100000) == 0)     // 100 seconds should be fine.
 //   ;
    
  return ::read(m_nFd, pBuffer, nBytes);
  
}