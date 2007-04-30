///////////////////////////////////////////////////////////
//  SetChannelsVisitor.cpp
//  Implementation of the Class SetChannelsVisitor
//  Created on:      01-Apr-2005 03:11:04 PM
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

#include <config.h>
#include "SetChannelsVisitor.h"
#include "CTreeParameter.h"
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



SetChannelsVisitor::~SetChannelsVisitor()
{

}


/**
 * Constructor
 * @param pattern
 *        pattern to match.
 * @param channels
 *        New channels value.
 * 
 */
SetChannelsVisitor::SetChannelsVisitor(std::string pattern, UInt_t channels) :
  CMatchingVisitor(pattern),
  m_nChannels(channels)
{

}


/**
 * Set the channels of a matching item.
 * @param pParam
 *        Pointer to the tree parameter to modify.
 * 
 */
void SetChannelsVisitor::OnMatch(CTreeParameter* pParam)
{
  pParam->setBins(m_nChannels);
}



