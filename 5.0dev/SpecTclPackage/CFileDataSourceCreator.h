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
#ifndef _CFILEDATASOURCECREATOR_H
#define _CFILEDATASOURCECREATOR_H

/**
 * @file CFileDataSourceCreator.h
 * @brief CFileDataSourceCreator creates data sources for file: URI's.
 */

#ifndef _CDATASOURCECREATOR_H
#include "CDataSourceCreator.h"	// Base class.
#endif

class CFileDataSource;

/**
 * @class CFileDataSourceCreator
 * 
 *  Creates data sources for file: URIs.  We're just going to create
 *  a CFileDataSource and hand that back to the caller.
 */
class CFileDataSourceCreator : public CDataSourceCreator
{
public:
  virtual CDataSource* create();
};

#endif
