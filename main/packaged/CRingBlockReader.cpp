/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CRingBlockReader.cpp
 *  @brief: Implementation of an ABC for reading blocks of ring items.
 */
#include "CRingBlockReader.h"

#include <stdlib.h>
#include <string.h>
#include <system_error>
#include <assert.h>
#include <string>


/**
 * constructor.
 *    We just need to setup the book keeping required to
 *    handle ring items that are spanned across reads.
 *
 *    Derived constructors are probably going to attache to whatever
 *    the data source is (e.g. file, ring item, zmq...).
 */
CRingBlockReader::CRingBlockReader() :
  m_partialItemSize(0), m_partialItemBlockSize(0), m_pPartialItem(nullptr)
{}
/**
 * destructor
 *    release the dynamic strorage that might still be around for the
 *    partial item.  The derived class destructor should probably
 *    break any connection to the source of data.
 */
CRingBlockReader::~CRingBlockReader()
{
    free(m_pPartialItem);
}
/**
 *  read
 *      Read a block of data.  The size passsed in is a buffering hint
 *      but represents the maximum size block that can be read.
 *      The actual transfer of data is through the polymorphic method
 *      blockRead which must be supplied by the base class.
 *      An implementation is given just as a placeholder for documentation.
 *
 *  @param nBytes - Target for the number of bytes to try to get.
 *                  This should be chosen to optimize I/O efficiency
 *                  for the data source actually used.
 *  @return CRingBlockReader::DataDescriptior Describes the data returned to
 *      the caller.  Note that s_pData is allocated by malloc and should
 *       be free'd when the caller is done with it.
 *  @note when the data source says there's no more data, a descriptor with
 *        s_nBytes and s_nItems set to zero and s_pData set to a nullptr
 *        is returned.
 *    
 */
CRingBlockReader::DataDescriptor
CRingBlockReader::read(size_t nBytes)
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
    ssize_t nRead = readBlock(pNextBytes, numToRead);
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
//////////////////////////////////////////////////////////////////////////
// Documentation of pure virtual methods:

/**
 * readBlock
 *    Reads a block of data from wherever the data is coming from.
 *
 *   @param pBuffer - block in which to put the data.
 *   @param maxBytes - Size of the storage region pointed to by pBuffer.
 *   @return ssize_t  If > 0, the number of bytes actually transferred
 *                    into pBuffer
 *   @retval 0      - The data source has no more data.
 *   @retval -1     - An error occured.  The reason for the error must be
 *                    left in errno.
 *   @note this method is pure virtual and must be implemented by
 *         concrete derived classes.
 */
ssize_t
CRingBlockReader::readBlock(void* pBuffer, size_t maxBytes)
{}

////////////////////////////////////////////////////////////////////////////
// Private methods:

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
CRingBlockReader::savePartialItem(void* pItem, size_t nBytes)
{
  if (m_partialItemBlockSize < nBytes) {
    delete []m_pPartialItem;  	// No-op for null pointer.
    m_pPartialItem = new char[nBytes];
    m_partialItemBlockSize = nBytes;
  }
  memcpy(m_pPartialItem, pItem, nBytes);
  m_partialItemSize = nBytes;
  
}