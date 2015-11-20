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

#ifndef __CFILEDATASOURCE_H
#define __CFILEDATASOURCE_H

/**
 * @file CFileDataSource.h
 * @brief Definition of SpecTcl data source from file.
 */

// Base class include:

#ifndef __CDATASOURCE_H
#include "CDataSource.h"
#endif

#ifndef __TCL_H
#include <tcl.h>
#ifndef __TCL_H
#define __TCL_H
#endif
#endif

/**
 * CFileDataSource
 *
 *   Derived from CDataSource.  This class provides a data source that connects SpecTcl to a file.
 *   The typical URI used to create a File data source is of the form: file:///path/to/file.
 * 
 */
class CFileDataSource : public CDataSource
{
  // private data:

private:
  Tcl_Channel  m_fd;  			// File descriptor open on the data source.
  bool         m_eof;			// True if the file's hit then EOF already.
  CTCLInterpreter* m_pInterp;


  // Canonicals:

public:
  CFileDataSource();
  virtual ~CFileDataSource();

  // The CDataSource interface

  virtual void   onAttach(CTCLInterpreter& interp, const char* pSource);
  virtual void   createEvent(CTCLInterpreter& interp, ChannelHandler* pHandler, void* pClientData=0);
  virtual size_t read(void* pBuffer, size_t nBytes);
  virtual void   close();
  virtual bool   isEof();

private:
  static void channelRelay(ClientData pData, int mask);
};


#endif
