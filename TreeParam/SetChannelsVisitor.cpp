///////////////////////////////////////////////////////////
//  SetChannelsVisitor.cpp
//  Implementation of the Class SetChannelsVisitor
//  Created on:      01-Apr-2005 03:11:04 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
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



