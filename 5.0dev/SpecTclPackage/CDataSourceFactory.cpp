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
 * @file CDataSourceFactory.cpp
 * @brief Implementation of the data source factory.
 */

#include "CDataSourceFactory.h"
#include "CDataSourceCreator.h"
// Static class data:



CDataSourceFactory::Creators CDataSourceFactory::m_creators;




/**
 * addCreator
 *
 *  Add a new data source to the creator set.
 *
 * @param protocol - The protocol the creator handles.
 * @param pCreator - A pointer to a dynamically allocated creator that handles this protocol.
 *                   Note that the creator must be dynamically allocated as remove,
 *                   destruction etc.  will delete the pointee.
 *
 * @throw data_source_factory_exception - if the protocol is already handled.
 */
void
CDataSourceFactory::addCreator(const char* protocol, CDataSourceCreator* pCreator)
{
  if (creatorExists(protocol)) {
    data_source_factory_exception e("CDataSourceFactory::addCreator failed", "duplicate  protocol");
    throw e;
  }

  m_creators[protocol] = pCreator;
}

/**
 * removeCreator
 *
 *  Removes a creator from the list of creators.
 *
 *  @param protocol - Name of the protocol that is being removed from play.
 *
 *  @throw data_source_factory_exception - if there is not a creator registered for
 *                    this protocol.
 */
void
CDataSourceFactory::removeCreator(const char* protocol)
{
  CreatorIterator p = m_creators.find(protocol);
  if (p == m_creators.end()) {
    throw data_source_factory_exception(
       "CDataSourceFactory::removeCreator failed", "no creator exists for this protocol");
  }

  delete p->second;
  m_creators.erase(p);

}

/**
 * creatorExists
 *
 *   @param protocol - Name of the protocol we're checking
 *
 *   @return bool    - True if the protocol has a creator false otherwise.
 */
bool
CDataSourceFactory::creatorExists(const char* protocol)
{
  return (m_creators.find(protocol) != m_creators.end());
}


/**
 * create
 *
 *  *  Locates the creator associated with a protocol
 *  *  Asks the creator to instantiate a data source.
 *
 * @param protocol
 *
 * @return CDataSource* - Pointer to the created data source.
 * @retval NULL         - There is no matching creator.
 * 
 */
CDataSource*
CDataSourceFactory::create(const char* protocol)
{
  if (creatorExists(protocol)) {
    return m_creators[protocol]->create();
  } else {
    return reinterpret_cast<CDataSource*>(0);
  }
}

/*-----------------------------------------------------------------------------*
 *  Testing support.
 */

/**
 * clearCreators
 *   Foreach creator delete it
 *   erase the map.
 */
void
CDataSourceFactory::clearCreators()
{
  CreatorIterator p = m_creators.begin();
  while (p != m_creators.end()) {

    delete p->second;

    p++;
  }
  m_creators.clear();
}
/*----------------------------------------------------------------------
** data_source_factory_exception implementation.
*/

/**
 * constructor:
 *
 * @param base_msg - the initial part of the message.
 * @param aux_msg  - The tail of the message.
 */
data_source_factory_exception::data_source_factory_exception(const char* base_msg, const char* aux_msg) : 
  m_msg(base_msg)
{
  m_msg += " :  ";
  m_msg += aux_msg;
}
/**
 * what - reports the exception.
 */
const char* data_source_factory_exception::what() const throw() {
  return m_msg.c_str();
}
