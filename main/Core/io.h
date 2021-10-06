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
#ifndef IO_H
#define IO_H

#include <unistd.h>
#include <stdint.h>

#include<sys/uio.h>

#include <set>
#include <string>

/**
 * @file io.h
 * @brief Commonly used I/O method definitions.
 * @author Ron Fox
 */

namespace io {
  void writeData (int fd, const void* pData , size_t size);
  size_t readData (int fd, void* pBuffer,  size_t nBytes);
  void closeUnusedFiles(std::set<int> keepOpen);
  void writeDataV(int fd, struct iovec* iov, int iovcnt);
  void writeDataVUnlimited(int fd, struct iovec* iov, int iovcnt);
  std::string getReadableEnvFile(const char* envName);
  std::string getReadableFileFromHome(const char* file);
  std::string getReadableFileFromWd(const char* file);
  std::string getReadableFileFromEnvdir(const char* env, const char* file);

}


#endif
