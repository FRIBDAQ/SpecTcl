/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/
#include <config.h>
#include "CBufferProcessor.h"
#include "CBufferCallbackList.h"
#include "CBufferCallback.h"
#include "CStandardControlCallback.h"
#include "CStandardScalerCallback.h"
#include "SRunContext.h"

#include <TranslatorPointer.h>
#include <BufferTranslator.h>

#include <buftypes.h>
#include <buffer.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Implements the CBufferProcessor class.  See the header for more information.

/*!
   Constructs a CBufferProcessor.  Wee will initialize the callback list
   with a set of empty lists To handle the MAXSYSBUFTYPE buffer number.  Note
   that the 0'th callback entry should be permanently empty as there is no buffer 
   type 0.
   We also create and initialize the run context block,
   and define/register the stock callbacks.
*/
CBufferProcessor::CBufferProcessor() :
  m_pRunContext(new SRunContext)
{
  // Extend the callbacks:

  extendCallbacks(MAXSYSBUFTYPE);

  // Initialize the context block:

  m_pRunContext->s_fScalersThisRun   = false;
  m_pRunContext->s_Title      = "";
  m_pRunContext->s_nRunNumber  = -1;
  m_pRunContext->s_RunStarted = "";
  m_pRunContext->s_RunEnded   = "";

  // Create and register the control and scaler callbacks:


  m_pControlCallback = new CStandardControlCallback(*m_pRunContext);
  addCallback(BEGRUNBF, *m_pControlCallback);
  addCallback(ENDRUNBF, *m_pControlCallback);
  addCallback(PAUSEBF,  *m_pControlCallback);
  addCallback(RESUMEBF, *m_pControlCallback);


  m_pScalerCallback = new CStandardScalerCallback(*m_pRunContext);
  addCallback(SCALERBF, *m_pScalerCallback);


}
/*!  
   Destroy a buffer processor.  The lists take care of themselves.
   We need to destroy the context block and our standard collection of buffer
   buffer processors:
*/
CBufferProcessor::~CBufferProcessor()
{
  delete m_pScalerCallback;
  delete m_pControlCallback;
  delete m_pRunContext;
}


// Selectors from the context block follow These prevent
// uncontrolled modification of the context block... we only hand out
// this block's pointer to standard callbacks we trust.

/*!
   Determine if a scaler buffer has been seen this run yet.
   \return bool
   \retval  true   - At least one scaler buffer has been seen.
   \retval  false  - No scaler buffers have ben seen.
*/
bool
CBufferProcessor::scalersSeen() const
{
  return m_pRunContext->s_fScalersThisRun;
}
/*!
   Return the number of scalers being read this run.
   If there have not yet been any scalers this run, we throw a string
   exception.  The string exception makes it a bit harsh on the C programmers.
   Oh well, maybe they should learn our language.

   \return unsigned int
   \retval number of scalers that are being read.

   \throw string  - if no scalers have been read yet.
*/
unsigned int
CBufferProcessor::scalerCount() const
{
  if(!scalersSeen()) {
    throw string("No scaler buffers seen so I don't know how many scalers there are");
  }
  return m_pRunContext->s_nScalerCount;
}
/*!
   Return a scaler channel's most recent increment.
   \param channel (unsigned int [in]):
      The number of the channel to fetch the increment for.  Channel numbers
      start from 0.

   \return unsigned long
   \retval the most recent increment value for the channel
   \throw string  
   - If the channel number is out of range.
   - If scalers have not yet been seen.
*/
unsigned long
CBufferProcessor::lastIncrement(unsigned int channel) const  
{
  if(channel < scalerCount()) {
    return m_pRunContext->s_Increments[channel];
  }
  else {
    throw string("Channel number out of range");
  }
}
/*!
  Return a scaler channel's current total.
  \param channel (unsigned int [in]):
      The number of the channel whose totals will be fetched.
      Channel numbers start from 0.
   \return float
   \retval The current total.
   \throw string
    - If the channel number is out of range.
    - If scalers have not been seen.
*/
float
CBufferProcessor::Total(unsigned int channel) const
{
  if (channel < scalerCount()) {
    return m_pRunContext->s_Totals[channel];
  } 
  else {
    throw string("Invalid channel to fetch totals");
  }
}
/*!
   Return the start time of the last scaler interval.
   \return unsigned long
   \retval    number of seconds since the start of the run.
   \throw string
   - If there have not yet been scaler buffers in this run.
*/
unsigned long
CBufferProcessor::lastIntervalStart() const
{
  if(m_pRunContext->s_fScalersThisRun) {
    return m_pRunContext->s_IntervalStartTime;
  }
  else {
    throw string("Scalers have not yet been seen, scaler start interval invalid");
  }
}
/*!
  Return the end time of the last scaler interval.
  \return unsigned long
  \retval number of seconds since the start of the run.
  \throw string
  - If there have been no scalers buffers for this run todate.
*/
unsigned long
CBufferProcessor::lastIntervalEnd() const
{
  if(m_pRunContext->s_fScalersThisRun) {
    return m_pRunContext->s_IntervalEndTime;
  }
  else {
    throw string("Scalers have not yet been seen, scaler start interval invalid");
  }
}
/*!
  Return the title of the most recent run.  If we have not seen any control buffers
  (possible for e.g. online processing of a run joined in progress), the title
  will be blank.
  \return string
  \retval the most recent title string seen.

*/
string
CBufferProcessor::Title() const
{
  return m_pRunContext->s_Title;
}
/*!
  Return the run number of the current run.  If control buffers have not yet
  been seen, this could be -1.
  \return int
  \retval most recent run number seen.
*/
int
CBufferProcessor::runNumber() const
{
  return m_pRunContext->s_nRunNumber;
}
/*!
  Get the absolute time at which the run started.  This is a string of 
  the form:

  month-name dd,yyyy hh:mm:ss

  e.g:

  February 11, 2005 11:03:15

  \return string
  \retval  Timestamp see above for format.
   - blank if not control buffers have been seen.

 */
string
CBufferProcessor::runStartTime() const
{
  return  m_pRunContext->s_RunStarted;
}
/*!
   Get the absolute time at which the most recent run ended.  Se runStartTime
   for more information about the return value of this function.
*/
string
CBufferProcessor::runEndTime() const
{
  return m_pRunContext->s_RunEnded;
}
/*!
   Add a callback for a buffer type to the list.
   \param nBufferType (unsigned int [in]):
       The type of buffer to add.
   \param callback (CBufferCallback& [in]):
       The callback object to add.
*/
void
CBufferProcessor::addCallback(unsigned int nBufferType, CBufferCallback& callback)
{
  extendCallbacks(nBufferType);	// Be sure there are enough entries and then:
  m_Callbacks[nBufferType].addCallback(callback); // Add to the list.

}
/*!
   Remove a callback given its reference, from a specific callback list.
   Since it's perfectly legitimate to have a callback object registered for more
   than one buffer type we force the user to specify which list to remove it from.

   \param nBufferType (unsigned int [in]):
      The type of buffer on which the callback was registered.
   \param callback (CBufferCallback& [in]):
      A reference to the callback that was added.

   \throw string 
      - nBufferType has no callback list associated with it.
      - The callback does not exist in the specified list.
*/
void
CBufferProcessor::removeCallback(unsigned int nBufferType, 
				 CBufferCallback& callback)
{
  if(nBufferType < m_Callbacks.size()) {
    CBufferCallbackList::CallbackIterator p = m_Callbacks[nBufferType].begin();
    CBufferCallbackList::CallbackIterator e = m_Callbacks[nBufferType].end();
    while(p != e) {
      if (*p == &callback) {
	m_Callbacks[nBufferType].removeCallback(p);
	return;
      }
      p++;
    }
    throw string("No such callback in list");
  }
  else {
    throw string("Buffer type has no callback lists");
  }
}
/*!
  Users should call this function to process a buffer.  
  - An appropriate translator pointer is created for the buffer.
  - The buffer type is extracted.
  - invokeCallbacks is called to process the buffer.

  Everything else is done by the callbacks (if any).
  \param pBuffer (const void* [in]):
    Pointer to the buffer.
*/
void
CBufferProcessor::operator()(const void* pBuffer)
{
  TranslatorPointer<unsigned short> p = getTranslatingPointer(pBuffer);
  unsigned int nBufferType = p[1];


  invokeCallbacks(nBufferType, pBuffer);
}

//     Utility(ies) for buffer callbacks:

/*!
   Create a translator pointer for a buffer.
   This TranslatorPointer<unsigned short> can be used to get byte order
   independent access to the contents of an NSCL data buffer.
   \param pBuffer (const void*):
       A pointer to an NSCL data acquisition system buffer.
   \return TranslatorPointer<unsigned short>
   \retval a translating pointer to the first word of the buffer.
*/
TranslatorPointer<unsigned short>
CBufferProcessor::getTranslatingPointer(const void* pBuffer)
{
  bheader *pHeader = (bheader*)pBuffer;

  return TranslatorPointer<unsigned short>(*BufferFactory::CreateBuffer(pHeader,
					      		       pHeader->lsignature));
}

//   -- private utility functions these by defintion don't merit doxygenized
//      headers.

/*
   Invoke the callbacks associated with a specific buffer type:
   Parameters:
     nBufferType (unsigned int [in]):
           The buffer type for which we want to callback.
     pBuffer (const void* [in]):
           The buffer itself.
*/
void
CBufferProcessor::invokeCallbacks(unsigned int nBufferType,
				  const void*  pBuffer)
{
  if(nBufferType < m_Callbacks.size()) {
    m_Callbacks[nBufferType](nBufferType, pBuffer); // This iterates the list.
  }
}
/*
   Extend the callback list vector as needed so that there will be an entry
   for nBufferType
   \param nBufferType (unsigned int [in]):
      The buffer type for which we want a callback.
*/
void
CBufferProcessor::extendCallbacks(unsigned int nBufferType)
{
  int nType = m_Callbacks.size();
  while(m_Callbacks.size() <=  nBufferType) {
    CBufferCallbackList temp(nType);
    m_Callbacks.push_back(temp);	// This will be copy constructed in.
    nType++;
  }
}
