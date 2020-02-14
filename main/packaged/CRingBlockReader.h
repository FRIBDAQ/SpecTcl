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

/** @file:  CRingBlockReader.h
 *  @brief: Reads blocks of ring items from some source. (ABC)
 */

#ifndef CRINGBLOCKREADER_H
#define CRINGBLOCKREADER_H
#include <unistd.h>
#include <cstdint>
/**
 * @class CRingBlockReader
 *    This is an abstract base class for  a hierarchy of classes
 *    that read data from some data source in large blocks.
 *    The base class provides the functionality to fetch blocks of data
 *    and to turn them into descriptors which hold the used size of the
 *    block, the number of ring items in the block and a pointer to the
 *    data block itself (which was malloced).  Concrete classes just
 *    need to supply the proteced virtual readBlock to create a concrete
 *    class.  For a sample concrete class, see CRingFileBlockReader.
 *    which can be made to read from anything that acts like a file
 *    descriptor.
 */
class CRingBlockReader
{
private:
    std::uint32_t  m_partialItemSize;       // How much data is in the partial item.
    std::uint32_t  m_partialItemBlockSize;	// How big is the buffer pointed to by m_pPartialItem.
    char* m_pPartialItem;

  
  
public:
    // The structure that's returned from a read.
    typedef struct _DataDescriptor {
        std::uint32_t s_nBytes;
        std::uint32_t s_nItems;
        void*         s_pData;             // was malloced.
    } DataDescriptor, *pDataDescriptor;
public:
    CRingBlockReader();
    virtual ~CRingBlockReader();
    
    virtual DataDescriptor read(size_t nBytes); // for testing.
protected:
    
    virtual ssize_t readBlock(void* pBuffer, size_t maxBytes) = 0;

private:
    void savePartialItem(void* pItem, size_t nBytes);

};


#endif