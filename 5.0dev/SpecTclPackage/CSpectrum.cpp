/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2013.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include "CSpectrum.h"
#include "CAxis.h"
#include "CSpectrumIncrementer.h"
#include "CSpectrumAllocator.h"
#include "CCondition.h"
/**
 * @file CSpectrum.cpp
 * @brief Implementation of CSpectrum.  See the header for more details.
 */

/**
 * constructor
 *  - Fill in the member data with the components of the spectrum.
 *  - Call the allocator to create the spectrum in the underlying
 *    histogramming kernel.
 *
 * @param name        - name to give to the spectrum.
 * @param pAllocator  - Pointer to the allocator for the underlying kernel.
 * @param pIncrementer- Pointer to the incrementer object for the underlying kernel.
 * @param pCondition  - Pointer to the condition to apply to the spectrum.
 * @param axes        - vector of axis definitions..
 */
CSpectrum::CSpectrum(const char* pName, CSpectrumAllocator* pAllocator, 
		     CSpectrumIncrementer* pIncrementer, CCondition* pCondition, 
		     std::vector<CAxis*> axes) :
  m_allocationHandle(0), m_name(pName),  m_axes(axes), m_condition(pCondition),
  m_incrementer(pIncrementer), m_allocator(pAllocator)
{
  m_allocationHandle = m_allocator->allocate(*this);
  m_incrementer->clear(*this);	// start out with a cleared spectrum.y
}
/**
 * destructor
 *
 *  Ask the allocator to destroy the underlying object.
 */
CSpectrum::~CSpectrum()
{
  m_allocator->deallocate(*this);
  delete m_condition;
  delete m_incrementer;
  for (int i = 0; i < m_axes.size(); i++) {
    delete m_axes[i];
  }
}
/**
 * getHandle
 *
 *  Selector to return the spectrum handle.  
 *
 * @return void*
 */
void*
CSpectrum::getHandle() {
  return m_allocationHandle;
}
/**
 * getName
 *   Selector to get spectrum name.
 *
 * @return std::string
 */
std::string
CSpectrum::getName() const
{
  return m_name;
}
/**
 * getAxis
 *
 *  Selector for spectum axes.
 *
 * @param which - Axis index (generally, 0 for x, 1 for Y)
 * 
 * @return Pointer to the requested axis or null if it does not exist.
 */
CAxis*
CSpectrum::getAxis(unsigned which)
{
  CAxis* result(0);
  if (which < m_axes.size()) {
    result = m_axes[which];
  }
  return result;
}
/**
 * getCondition
 *
 *  Return a pointer to the spectrum condition.
 *
 * @return CCondition*
 */
CCondition*
CSpectrum::getCondition()
{
  return m_condition;
}

/**
 * getIncrementer
 *
 *  Return a pointer to the spectrum's incrementer component.
 *
 * @return CSpectrumIncrementer*
 */
CSpectrumIncrementer*
CSpectrum::getIncrementer()
{
  return m_incrementer;
}
/**
 * getAllocator
 *
 *  Return a pointer to the spectrum's allocator component.
 *
 * @return CSpectrumAllocator*
 */
CSpectrumAllocator*
CSpectrum::getAllocator()
{
  return m_allocator;
}
/**
 * setCondition
 *
 *   Modifies the condition on the current spectrum.
 *
 * @param pCondition - Pointer to the new condition.
 */
void
CSpectrum::setCondition(CCondition* pCondition)
{
  delete m_condition;
  m_condition = pCondition;
}
/**
 * clear
 *
 *  Request that the incrementer clear the underlying spectrum
 *  (set all channels to zero).
 */
void
CSpectrum::clear()
{
  m_incrementer->clear(*this);
}
/**
 * operator()
 *
 *  Called to ask the spectrum to increment itself for the
 *  current event.  The incrementer's function call operator
 *  really does the dirty work.
 */
void
CSpectrum::operator() ()
{
  (*m_incrementer)(*this);
}
/**
 * get
 *   Retrieve a channel value from the incrementer.
 *
 * @param x - Xcoordinate of the channel.
 * @param y - Ycoordinate of the channel (ignored for 1-D).
 * @return uint32_t value of the channel.
 */
uint32_t
CSpectrum::get(unsigned x, unsigned y) 
{
  return m_incrementer->get(*this, x, y);
}
/**
 * set
 *   Set the value of a channel to a specific value.
 *
 * @param x - Xcoordinate of channel.
 * @param y - Y coordinate of channel (ignored for 1d).
 * @param value - New value.
 */
void
CSpectrum::set(unsigned x, unsigned y, uint32_t value)
{
  m_incrementer->set(*this, x, y, value);
}
/**
 * checkValidity
 *
 *   Requests that the spectrum's incrementer and condition
 *   check that they are still valid.
 */
void
CSpectrum::checkValidity()
{
  m_incrementer->validate(*this);
  m_condition->validate(*this);
}
