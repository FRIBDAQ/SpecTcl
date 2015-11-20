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

#ifndef _CDATASOURCE_FACTORY_H
#define _CDATASOURCE_FACTORY_H

#ifndef __STD_STRING
#include <string>
#ifndef __STD_STRING
#define __STD_STRING
#endif
#endif

#ifndef __STD_MAP
#include <map>
#ifndef __STD_MAP
#define __STD_MAP
#endif
#endif

#ifndef __STD_EXCEPTION
#include <exception>
#ifndef __EXCEPTION
#define __EXCEPTION
#endif
#endif

/**
 * @file CDataSourceFactory.h
 * @brief Define factory that builds data sources from URIs.
 */

class CDataSource;
class CDataSourceCreator;


/**
 * @class  CDataSourceFactory
 *
 *   This class represents an extensible factory that builds
 *   data sources given a protocol selector.  You can imagine that a 
 *   caller gets a URI like:  file:///user/fox/test.evt
 *   Parses the URI, uses the protocol (file) to create a data source
 *   and the hierarhy part (/user/fox/test.evt) to attach the data source.
 *
 *
 */
class CDataSourceFactory
{
  // private data types:

private:
  typedef std::map<std::string, CDataSourceCreator*> Creators;
  typedef Creators::iterator CreatorIterator;

  // private data:
private:
  static Creators m_creators;

  /** Public methods.
   */
public:
  virtual ~CDataSourceFactory() {}

  // Manage the creators:

  void addCreator(const char* protocol, CDataSourceCreator* pCreator);
  void removeCreator(const char* protocol);
  bool creatorExists(const char* protocol);

  // Creation:

  CDataSource* create(const char* protocol);

  // this is to make tests idempotent with respect to the creator set:

private:
  static void clearCreators();  
};

class data_source_factory_exception : public std::exception
{
  std::string m_msg;
public:
  data_source_factory_exception(const char* base_msg, const char* aux_msg);
  virtual ~data_source_factory_exception() throw() {}

  virtual const char* what() const throw();
};

#endif
