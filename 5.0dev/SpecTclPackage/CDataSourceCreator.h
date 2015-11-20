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
#ifndef _CDATASOURCECREATOR_H
#define _CDATASOURCECREATOR_H

/**
 * @file CDataSourceCreator.h
 * @brief Defines the ABC for creating data sources, helper class for CDataSourceFactory
 */

class CDataSource;

/**
 * @class CDataSourceCreator
 *
 *  Abstract base class for creating data sources.
 *  The data source factory has a collection of these.
 *  Data sources are associated with 'protocols'.
 *  In SpecTcl usually a protocol is the part of a URI prior to the :
 *  e.g. file:///user/fox/test.evt identifies a file protocol.
 *
 */

class CDataSourceCreator
{
public:
  ~CDataSourceCreator() {}	// So subclasses can declare virtual destructors.

  virtual CDataSource* create() = 0;
};

#endif
