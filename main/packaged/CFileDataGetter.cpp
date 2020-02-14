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

/** @file:  CFileDataGetter.cpp
 *  @brief: Implement the CFileDataGetter.
 */

/**
 * constructor
 *   @param path  - Source for data.
 *   @param bufferSize - Bytes used for buffering data.
 */
#include "CFileDataGetter.h"
#include "CRingFileBlockReader.h"
#include <stdlib.h>

/**
 * constructor:
 *   @param filename  - the file to open for read.
 *   @param bufferSize - Size of each read operation.
 */
CFileDataGetter::CFileDataGetter(const char* filename, size_t buffersize) :
    m_pReader(new CRingFileBlockReader(filename)) ,
    m_nReadSize(buffersize)
{}

/**
 * destructor
*/
CFileDataGetter::~CFileDataGetter()
{
    delete m_pReader;
}

/**
 * readBlock
 *    We just need to return the pointer and the size from the
 *    data descriptor.  Note that our free is non empty
 *    as the data from m_pReader is malloced.
 *
 *  @return std::pair<size_t, void*> - Number of usable bytes, buffer pointer.
 *  @note the resulting pair is gauranteed to be only complete ring items.
 */
std::pair<size_t, void*>
CFileDataGetter::read()
{
    CRingBlockReader::DataDescriptor d = m_pReader->read(m_nReadSize);
    std::pair<size_t, void*> result;
    result.first  = d.s_nBytes;
    result.second = d.s_pData;
    return result;
}
/**
 * free
 *    Called when a block of data is no longer needed.  In our case
 *    we need to free(3) the block.
 */
void
CFileDataGetter::free(std::pair<size_t, void*>& data)
{
    ::free(data.second);
}