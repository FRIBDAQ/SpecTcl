/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

//  Implementation of the standard scaler callback object.
#include <config.h>
#include "CStandardScalerCallback.h"
#include "CBufferProcessor.h"
#include <TranslatorPointer.h>
#include <buffer.h>

#include <vector>
#include <cstdint>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif





/*!
   Construct an object.  This is just a matter of saving the
   run context pointer.
*/
CStandardScalerCallback::CStandardScalerCallback(SRunContext& RunData) :
  m_pRunData(&RunData)
{
}
/*!
   Construct a copy of an object:
*/
CStandardScalerCallback::CStandardScalerCallback(const CStandardScalerCallback& rhs) :
  m_pRunData(rhs.m_pRunData)
{
}
/*!
    Destructor is a no-op for now
*/
CStandardScalerCallback::~CStandardScalerCallback()
{
}
/*!
   Assignment - just copy the context pointer:
*/
CStandardScalerCallback&
CStandardScalerCallback::operator=(const CStandardScalerCallback& rhs)
{
  if(this != &rhs) {
    CBufferCallback::operator=(rhs);
    m_pRunData = rhs.m_pRunData;
  }
  return *this;
}
/*!
  Equality comparison - The pointer is all we have to compare:
*/
int
CStandardScalerCallback::operator==(const CStandardScalerCallback& rhs) const
{
  return ((CBufferCallback::operator==(rhs))          &&
	  (m_pRunData == rhs.m_pRunData));
}
/*!  Inequality comparison is the logical invers of equality:
 */
int
CStandardScalerCallback::operator!=(const CStandardScalerCallback& rhs) const
{
  return !(*this == rhs);
}
/*!
   Callback entry point.  This member is called to analyze a scaler buffer.
   What we will do is decode the buffer and set the following fields in m_pRunData:
   - s_IntervalStartTime  - the interval start time from the buffer.
   - s_IntervalEndTime    - the interval end time fromt he buffer.
   - s_nScalerCount       - from the buffer entity count.  If necessary, the
                            arrays s_Increments and s_Totals are enlarged
   - s_fScalersThisRun    - true.
   - s_Increments         - from the buffered scalers.
   - s_Totals             - Summed with the buffered scalers.
*/
void
CStandardScalerCallback::operator()(unsigned int nBufferType, const void* pBuffer)
{
  BufferTranslator* bt = CBufferProcessor::getTranslatingPointer(pBuffer);
  TranslatorPointer<unsigned short> p(*bt);

  unsigned int numScalers = p[6]; // Number of entities inthe buffer.

  // Let's do the simple stuff first:

  m_pRunData->s_fScalersThisRun  = true;	        // We have seen a scaler buffer.
  m_pRunData->s_nScalerCount     = numScalers;   // It claims to have this many scalers.
  
  while(m_pRunData->s_Increments.size() < numScalers) {
      m_pRunData->s_Increments.push_back(0);
      m_pRunData->s_Totals.push_back(0.0);
    
  }

  // Now we get to the scaler buffer body:

  p += 16;			// Pointing to end time.
  TranslatorPointer<uint32_t> pend(p);
  unsigned long endTime = *pend++;
  p += 5;			// Pointing to start time:
  TranslatorPointer<uint32_t> pstart(p);
  unsigned long startTime = *pstart++;


  m_pRunData->s_IntervalStartTime = startTime;
  m_pRunData->s_IntervalEndTime   = endTime;

  p += 5;			// Pointing to the scalers:

  TranslatorPointer<uint32_t> pScalers(p);

  for(int i =0; i < numScalers; i++) {
    unsigned long increment = *pScalers++;
    m_pRunData->s_Increments[i] = increment;
    m_pRunData->s_Totals[i]     += (float)increment;
			       
  }
  delete bt;			// This is dynamically allocated.

}
