/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/**
 * @file io.cpp
 * @brief Commonly used I/O methods.
 * @author Ron Fox.
 */

#include "io.h"

#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <set>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <CTCLMutex.h>
#include <system_error>

static std::set<int>                 okErrors;	// Acceptable errors in I/O operations.

static CMutex csGaurd;
/**
 * Return true if an I/O errno is not an allowed one.
 * 
 * @param error - the  errno to check.
 *
 * @return bool - True if the error is a bad one.
 *
 * @note okErrors is a set that will contain the 'allowed' errors.
 */
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

static long maxIovecSize(-1);

namespace io {
  
/**
 * updateIov
 *    Given an input I/O vector, the number of items it has
 *    and the number of bytes that were most recently written,
 *    returns a pointer to the remaining I/O items to write and
 *    updates the number of items.
 *    Note that this may  mean that the input I/O vector may be modified
 *    if one of its items was only partially written.
 *
 * @param iov - the input I/O vector that writev just used.
 * @param[inout] nItems - the number of items in the vector (input). This will
 *               be modified to reflect the number of remaining items.
 *               note that this will be zero if there are no more items.
 * @param nBytes - Number of bytes successfully written by the last write.
 * @return struct iovec* - pointer to the first item in the vector that needs to be
 *               handed to the next call to writev.  This is undefined if
 *               all itmems were written.
 *  Tested
 */
struct iovec*
updateIov(struct iovec* iov, int& nItems, ssize_t nBytes)
{
  while ((nItems > 0) && (nBytes > 0)) {
    if (nBytes >= iov->iov_len) {         // Fully written:
      nBytes -= iov->iov_len;
      iov++;
      nItems--;
    } else {                            // Partially written, must adjust and break:
      iov->iov_len -= nBytes;
      uint8_t* p = static_cast<uint8_t*>(iov->iov_base);
      p += nBytes;
      iov->iov_base = p;
      nBytes = 0;                      // Done.
    }
  }
  return iov;
}


/**
 * Write a block of data to a file descriptor.
 * As with getBuffer, multiple writes are done..until either the entire data
 * are written or
 * *  A write ends in an eof condition.
 * *  A write ends in an error condition that is not EAGAIN, EWOUDLDBLOCK or EINTR.
 *
 * @param fd    - file descriptor to which the write goes:
 * @param pData - Pointer to the data to write.
 * @param size  - Number of words of data to write.
 * 
 * @throw int 0 - I/O showed eof on output.
 * @throw int errno - An error and why.
 */

void writeData (int fd, const void* pData , size_t size)
{
  const uint8_t* pSrc(reinterpret_cast<const uint8_t*>(pData));
  size_t   residual(size);
  ssize_t  nWritten;

  while (residual) {
    nWritten = write(fd, pSrc, residual);
    if (nWritten == 0) {
      throw 0;
    }
    if ((nWritten == -1) && badError(errno)) {
      throw errno;
    }
    // If an error now it must be a 'good' error... set the nWritten to 0 as no data was
    // transferred:

    if (nWritten < 0)
    {
      nWritten = 0;
    }
    // adjust the pointers, and residuals:


    residual -= nWritten;
    pSrc     += nWritten;
  }

}
/**
 * Get a buffer of data from  a file descritor.
 * If necessary multiple read() operation are performed to deal
 * with potential buffering between the source an us (e.g. we are typically
 * on the ass end of a pipe where the pipe buffer may be smaller than an
 * event buffer.
 * @param fd      - File descriptor to read from.
 * @param pBuffer - Pointer to a buffer big enough to hold the event buffer.
 * @param size    - Number of bytes in the buffer.
 *
 * @return size_t - Number of bytes read (might be fewer than nBytes if the EOF was hit
 *                  during the read.
 *
 * @throw int - errno on error.
 */
  size_t readData (int fd, void* pBuffer,  size_t nBytes)
{
  uint8_t* pDest(reinterpret_cast<uint8_t*>(pBuffer));
  size_t    residual(nBytes);
  ssize_t   nRead;

  // Read the buffer until :
  //  error other than EAGAIN, EWOULDBLOCK  or EINTR
  //  zero bytes read (end of file).
  //  Regardless of how all this ends, we are going to emit a message on sterr.
  //

  while (residual) {
    nRead = read(fd, pDest, residual);
    if (nRead == 0)		// EOF
    {
      return nBytes - residual;
    }
    if ((nRead < 0) && badError(errno) )
    {
      throw errno;
    }
    // If we got here and nread < 0, we need to set it to zero.
    
    if (nRead < 0)
    {
      nRead = 0;
    }

    // Adjust all the pointers and counts for what we read:

    residual -= nRead;
    pDest  += nRead;
  }
  // If we get here the read worked:

  return nBytes;		// Complete read.
}


/**
 * closeUnusedFiles
 *     This function is intended for use by forked/execed processes..
 *     It is linux specific.  It uses /proc/pid/fd to
 *     close all file descriptors that are not in the set provided.
 *     Normally a forked/exece'd program will ask that all file descriptors
 *     other than STDIN_FILENO, STDOUT_FILENO and STDERR_FILENO be
 *     closed, but this method is policy agnostic.
 *
 *  @paraqm keepOpen - Set of file descriptors to keep open.
 */
void
closeUnusedFiles(std::set<int> keepOpen)
{
  CriticalSection c(csGaurd);
  errno =0;
  // construct the directory name /proc/mypid/fd.

  std::stringstream dirnameStr;
  dirnameStr << "/proc/" << getpid() << "/fd";
  std::string dirname = dirnameStr.str();
  
  // Construct the vector of fds to close.  Note that we use
  // readdir_r in the unlikely case that we're running in a threaded program
  // that also reads directories:
  
  struct dirent *nextEntry, *status;
  DIR* pDir = opendir(dirname.c_str());
  if (!pDir) throw errno;                 // Open failed.
  
  // We need to add the fd open on the dir to the
  // keepOpen set otherwise we may yank the directory stream out from underneath
  // us.
  
  keepOpen.insert(dirfd(pDir));
  do {
    nextEntry =  readdir(pDir);           // daqdev/NSCLDAQ#972
    if (!nextEntry && (errno != 0)) throw errno;               // Failure
    if (nextEntry) {                        // have a dir.
      int fd = atoi(nextEntry->d_name);       // FD the process has open.
      if (!keepOpen.count(fd)) {         // not in the list to keep open.
        close(fd);
      }
    }
  } while (nextEntry);
  
  closedir(pDir);                       // Also closes our dir fd.
}
/**
 * writeDataV
 *    This version of writeData implements a gather write using the writev(2)
 *    method. As with the other writeData, retries for signal interruption
 *    and incomplete data  transfer are handled transparently for the caller.
 *
 *  @param fd - file descriptor to which the data will be written (could be
 *             a socket, or pipe).
 *  @param iov - The vector of I/O descriptors.  See writev(2) for information
 *            about the form of these. Note that in the event the write must
 *            be continued from an incomplete write (e.g. write bigger than
 *            OS buffers), the contents of this vector can be modified in the
 *            course of resuming the write operation.
 *  @param iovcnt - number of iovec structs pointed to by iov.
 *  @throw std::system_error encapsulating the errno that caused the
 *         failure
 *  @note Not all write 'failures' cause this method to fail.
 *        - EAGAIN results in a retry.
 *        - EWOULDBLOCK results in a retry
 *        - EINTR results in a retry.
 * @note the number of elements in the iovcnt may be limited by the underlying
 *      OS  - see writeDataVUnlimited which, if needed calls writeDataV
 *            repetitively.
*/
void
writeDataV(int fd, struct iovec* iov, int iovcnt)
{
  while (iovcnt) {
    ssize_t nBytes = writev(fd, iov, iovcnt);
    if (nBytes < 0) {
      int error = errno;
      if (badError(error)) {
        throw std::system_error(
          error, std::generic_category(), "vectored io::writeData failed."
        );
      } else {
        // retriable error - zero bytes written:
        
        nBytes = 0;
      }
    }
    iov = updateIov(iov, iovcnt, nBytes);
  }
}
/**
 * writeDataVUnlimited
 *    Repeatedly calls WriteDataV with at most the system limit of
 *    I/O vectors until a potentially unlimited iov is written.
 *
 *
 *  @param fd - file descriptor to which the data will be written (could be
 *             a socket, or pipe).
 *  @param iov - The vector of I/O descriptors.  See writev(2) for information
 *            about the form of these. Note that in the event the write must
 *            be continued from an incomplete write (e.g. write bigger than
 *            OS buffers), the contents of this vector can be modified in the
 *            course of resuming the write operation.
 *  @param iovcnt - number of iovec structs pointed to by iov.
 *  @throw std::system_error encapsulating the errno that caused the
 *         failure
 *
 *  @note - the first time sysconf is called to get the maximum iovec count.
 */
void
writeDataVUnlimited(int fd, struct iovec* iov, int iovcnt)
{
  if (maxIovecSize < 0) {
    maxIovecSize = sysconf(_SC_IOV_MAX);
  }
  while (iovcnt) {
    int n = iovcnt;
    if (n > maxIovecSize) n = maxIovecSize;
    writeDataV(fd, iov, n);
    
    iov += n;
    iovcnt -= n;
  }
}
//  These are factorizations of code fragments
//   from various places daqdev/NSCLDAQ#700
/**
 * getReadableEnvFile
 *    Translate an environment variable to a file and
 *    return the path to the file.
 * @param envName - name of the environment variable.
 * @return std::string - Full path to file verified as readable.
 * @retval "" - file either does not exist or is not readable.
 */
std::string
getReadableEnvFile(const char* envName)
{
  std::string result;
  const char* path = getenv(envName);
  if (path && (access(path, R_OK) == 0) ) {

    result = path;
  }
  return result;
}
/**
 * getReadableFileFromHome
 *    Return the full path to a readable file relative
 *    to the home directory.  Note that the environment
 *    variable "HOME" is used to determine the home dir
 *    rather than asking the account system.
 *
 *  @param filename  file path relative to home
 *  @return std::string  - full filename path
 *  @retval "" - if the filename either does not exist or
 *               is not readable.
 */
std::string
getReadableFileFromHome(const char* file)
{
  std::string result;
  const char* home = getenv("HOME");
  if (home) {
    std::string path(home);
    path += file;
    if (access(path.c_str(), R_OK) == 0) {
      result = path;
    }
  }
  return result;
}
/**
 * getReadableFileFromWd
 *    Get a readable file from the working directory
 * @param filename - filename without the directory path.
 * @return std::string - full file path.
 * @retval "" if the file either does not exist or is not readable.
 */
std::string
getReadableFileFromWd(const char* file)
{
  std::string result;
  char here[PATH_MAX];
  char* pResult = getcwd(here, PATH_MAX);
  if (pResult) {
    std::string path(here);
    path += file;
    if (access(path.c_str(), R_OK) == 0) {
      result =  path;
    }
  }
  
  return result;
}

/**
 * getReadableFileFromEnvdir
 *   Given an environment variable and a filename
 *   if there's a readable file in that directory
 *   return the full path.
 * @param env - environment name.
 * @param file  name of the file we're looking for in that dir.
 * @return std::string - full path to file.
 * @retval "" if there is no file or it's not readable...
 *            or for that matter if env doesn't translate.
 *            
 */
std::string getReadableFileFromEnvdir(const char* env, const char* file)
{
  std::string result;
  const char* pDir = getenv(env);
  if (pDir) {
    std::string path = pDir;
    path += "/";
    path += file;
    if (access(path.c_str(), R_OK) == 0) {
      result = path;
    }
  }
  
  return result;
}

}
