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

/** @file:  CDataGetter.h
 *  @brief: Gets data from some source (ABC defines the interface).
 */
#ifndef CDATAGETTER_H
#define CDATAGETTER_H
#include <stddef.h>
#include <utility>
/**
 * @class CDataGetter - This class defines the interface needed to get data
 *                      from some data source (e.g. a file or MPI or what have you).
 *                      It's an abstract base class whose pure virtual methods
 *                      must be filled by concrete methods.  Furthermore,
 *                      Concrete classes may have non-trival constructors.
 *                      and may require some commands to set them up.
 */
class CDataGetter
{
public:
    std::pair<size_t, void*> read() = 0;    // Read data from the source.
    void free(std::pair<size_t, void*>& data);  // Do any data frees needed for a read item.
};


#endif