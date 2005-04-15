///////////////////////////////////////////////////////////
//  SetChannelsVisitor.h
//  Implementation of the Class SetChannelsVisitor
//  Created on:      01-Apr-2005 03:11:03 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__SETCHANNELSVISITOR_H)
#define __SETCHANNELSVISITOR_H

#include "CMatchingVisitor.h"

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
// Forward defs:

class CTreeParameter;

/**
 * Visitor object intended to be visited by for_each on the tree parameter
 * dictionary.  The visitor modifies the channel count of all matching tree
 * parameters.
 * @author Ron Fox
 * @version 1.0
 * @created 01-Apr-2005 03:11:03 PM
 */
class SetChannelsVisitor : public CMatchingVisitor
{

private:
  /**
   * new number of channels.
   */
  UInt_t m_nChannels;
  
public:
  virtual ~SetChannelsVisitor();
  SetChannelsVisitor(STD(string) pattern, UInt_t channels);
  virtual void OnMatch(CTreeParameter* pParam);
  
};




#endif
