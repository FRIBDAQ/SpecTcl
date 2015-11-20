/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include "CFileDataSourceCreator.h"
#include "CFileDataSource.h"

/**
 * @file CFileDataSource.cpp
 * @brief Implementation of the extensible factory creator for file data sources.
 */

/**
 ** create
 *
 *  Create one of the file data sources.
 */
CDataSource*
CFileDataSourceCreator::create()
{
  return reinterpret_cast<CDataSource*>(new CFileDataSource);
}
