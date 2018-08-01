///////////////////////////////////////////////////////////
//  SetChannelsVisitor.h
//  Implementation of the Class SetChannelsVisitor
//  Created on:      01-Apr-2005 03:11:03 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//



#ifndef SETCHANNELSVISITOR_H
#define SETCHANNELSVISITOR_H

#include "CMatchingVisitor.h"

#include <string>
#include <histotypes.h>


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
  SetChannelsVisitor(std::string pattern, UInt_t channels);
  virtual void OnMatch(CTreeParameter* pParam);
  
};




#endif
