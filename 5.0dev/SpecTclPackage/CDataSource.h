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


#ifndef __CDATASOURCE_H
#define __CDATASOURCE_H

#ifndef __STDLIB_H
#include <stdlib.h>
#ifndef __STDLIB_H
#define __STDLIB_H
#endif
#endif

#ifndef __STD_EXCEPTION
#include <exception>
#ifndef __EXCEPTION
#define __EXCEPTION
#endif
#endif

#ifndef  __STD_STRING
#include <string>
#ifndef __STD_STRING
#define __STD_STRING
#endif
#endif

/**
 * @file CDataSource.h
 * @brief Abstract base class for SpecTcl data sources.
 */


class CTCLInterpreter;

/**
 * @class CDataSource
 *
 *   This class is an abstract base class for a SpecTcl data source.
 *   SpecTcl data sources provide input data for the SpecTcl analysis pipeline.
 * 
 *  The concrete ones must implement:
 *
 *  *  onAttach    - actually open a connection tothe data source.
 *  *  createEvent - Create a Tcl event that fires when the data source becomes readable.
 *  *  read        - Read some data from the data source.
 *  *  close       - Close the data source.
 *  *  isEof       - True if the data source has no more data to give.
 */
class CDataSource
{
  // The following typedef defines the read event callback:

public:
  typedef void ChannelHandler(CDataSource*, void*);

private:

  ChannelHandler* m_pHandler;
  void*           m_pClientData;


public:
  CDataSource();
  virtual ~CDataSource();

  // Interface definition.

  virtual void    onAttach(CTCLInterpreter& interp, const char* pSource) = 0;
  virtual void    createEvent(CTCLInterpreter& interp, ChannelHandler* pHandler, void* pClientData) = 0;
  virtual size_t  read(void* pBuffer, size_t nBytes) = 0;
  virtual void    close() = 0;
  virtual bool    isEof() = 0;

protected:
  void setChannelHandler(ChannelHandler* pHandler, void* pData);
  void invokeHandler();
};
 

/**
 * data_source_exception
 *
 *   Data source exceptions have errnos:
 */
class data_source_exception : public std::exception
{
  std::string full_msg;
public:
  data_source_exception(int err, const char* msg);
  data_source_exception(const char* base_msg, const char* aux_msg);
  virtual ~data_source_exception() throw() {}

  virtual const char* what() const throw();

};


#endif
