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

/**
 * @file CDataSource.cpp
 * @brief implementation of classes defined in CDataSource.h
 */

#include "CDataSource.h"
#include <string.h>

/**
 * Implementation of the CDataSource methods that are not pure virtual.
 */

/**
 * Constructor
 */
CDataSource::CDataSource()   :
  m_pHandler(0)
{

}
/**
 * Destructor
 *
 */
CDataSource::~CDataSource() {
  m_pHandler = 0;
}

/**
 * setChannelHandler
 *   Save the channel handler for later invocation:
 *
 * @param handler pointer to the handler function.
 * @param pData   Dat passed without interpretation to the handler,
 */
void
CDataSource::setChannelHandler(CDataSource::ChannelHandler* handler, void* pClientData)
{
  m_pHandler = handler;
  m_pClientData = pClientData;
}

/**
 * invokeHandler
 *
 *  If the handler is defined invoke it.  Otherwise it's a no-op.
 */
void
CDataSource::invokeHandler()
{
  (m_pHandler)(this, m_pClientData);
}


/**
 *  Implemtation of the data_source_exception class.
 *  Class for giving errors from data sources that have associated errnos.
 */

/**
 * constructor
 *
 *  @param err       - errno value at the time of the error.
 *  @param msg       - The base error message.
 */
data_source_exception::data_source_exception(int err, const char* msg) :
  full_msg(msg)
{
  full_msg += " : ";
  full_msg += strerror(err);
}
/**
 * constructor
 *
 * @param base_msg - Base error message.
 * @param aux_msg  - Auxiliary error message (stuff after " : ".
 */
data_source_exception::data_source_exception(const char* base_msg, const char* aux_msg) :
  full_msg(base_msg)
{
  full_msg += " : ";
  full_msg += aux_msg;

}
/**
 * what
 *   Builds and returns the full error string as:
 *     base_msg + " : " + strerror(error)
 *
 * @return const char*
 */
const char*
data_source_exception::what() const throw()
{

  return full_msg.c_str();
}


