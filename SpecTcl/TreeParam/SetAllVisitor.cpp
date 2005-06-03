///////////////////////////////////////////////////////////
//  SetAllVisitor.cpp
//  Implementation of the Class SetAllVisitor
//  Created on:      01-Apr-2005 02:56:43 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
#include <config.h>
#include "SetAllVisitor.h"
#include "CTreeParameter.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


SetAllVisitor::~SetAllVisitor()
{
}


/**
 * Constructor
 * @param pattern
 *         Pattern that names are matched against to set.
 * @param channels
 *        New channel count.
 * @param low
 *        New low limit.
 * @param high
 *        New high limit
 * @param units
 *        New units string.
 * 
 */

SetAllVisitor::SetAllVisitor(std::string pattern, int channels,
			     double low, double high, std::string units) :
  CMatchingVisitor(pattern),
  m_units(units),
  m_nChannels(channels),
  m_fLow(low),
  m_fHigh(high)
{

}


/**
 * Set the values for a matching parameter.
 * @param parameter    The parameter to process.
 * 
 */
void 
SetAllVisitor::OnMatch(CTreeParameter* parameter)
{
  parameter->setBins(m_nChannels);
  parameter->setStart(m_fLow);
  parameter->setStop(m_fHigh);
  parameter->setUnit(m_units);
  parameter->setChanged();
}



