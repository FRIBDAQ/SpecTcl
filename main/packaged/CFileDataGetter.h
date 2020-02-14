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

/** @file:  CFileDataGetter.h
 *  @brief: Defines class that gets data from a file.
 */
#ifndef CFILEDATAGETTER_H
#define CFILEDATAGETTER_H

#include "CDataGetter.h"

class CRingBlockReader;

/**
 * @class CFileDataGetter
 *    This is a class that gets data from a data file in accordance
 *    with the CDataGetter interface.
 */
class CFileDataGetter : public CDataGetter
{
    CRingBlockReader* m_pReader;
    size_t            m_nReadSize;
public:
    CFileDataGetter(const char* path, size_t bufferSize);
    virtual ~CFileDataGetter();
    
    std::pair<size_t, void*> read();
    void free(std::pair<size_t, void*>& data);
};


#endif