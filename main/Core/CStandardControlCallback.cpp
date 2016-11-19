/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#include <config.h>
#include "CStandardControlCallback.h"
#include "CBufferProcessor.h"

#include <buftypes.h>
#include <buffer.h>

#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Implementation of the CStandardControlCallback.   It is created and isntantiated
// then linked into the buffer call back chains for begin/ end/ pause/ resume buffers.


// Static tables:

static const char* monthnames[] = {
  "NOSUCHMONTH",
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};

/*!
    Construct a standard control callback object.  This involves saving
    a pointer to the the context block, as this object will be publishing
    data to it.
    \param RunData (SRunContext& [modified]):
        A reference to the run context data.
*/
CStandardControlCallback::CStandardControlCallback(SRunContext& RunData) :
  m_pRunData(&RunData)
{
}
/*!
   Copy construction just copies the context pointer.
   \param rhs
      The object we are constructing a copy of.
*/
CStandardControlCallback::CStandardControlCallback(const CStandardControlCallback& rhs) :
  m_pRunData(rhs.m_pRunData)
{
}
/*!
    Destructor; Doesn't need to do anything.
*/
CStandardControlCallback::~CStandardControlCallback()
{
}
/*!
  Assignment: Just assigns the context from the rhs:
 */
CStandardControlCallback& 
CStandardControlCallback::operator=(const CStandardControlCallback& rhs)
{
  if(this != &rhs) {
    CBufferCallback::operator=(rhs);
    m_pRunData = rhs.m_pRunData;
  }
  return *this;
}
/*!
   Comparison:  The only thing we have a handle on is the context data
*/
int
CStandardControlCallback::operator==(const CStandardControlCallback& rhs) const
{
  return ((CBufferCallback::operator==(rhs))     &&
          (m_pRunData == rhs.m_pRunData));
}
/*!
   Inequality comparison.  just the logical inverse of equality:
*/
int
CStandardControlCallback::operator!=(const CStandardControlCallback& rhs) const
{
  return !(*this == rhs);
}
/*!
    Callback.  This is invoked when an appropriately typed buffer is
    seen.  First we create a translating buffer pointer and then
    we do the 'standard stuff'  Finally, depending on type we do other stuff.

    \param nBuffertype [unsigned int (in)]:
        The type of buffer that called us.  See buftypes.h for more information.
    \param pBuffer (const void* [in]):
        The buffer itself.

   Note that the buffer may have any byte order. we therefore use a translating
   buffer pointer to deal with this.

 */
void
CStandardControlCallback::operator()(unsigned int nBufferType,
				     const void* pBuffer)
{
  BufferTranslator* bt = CBufferProcessor::getTranslatingPointer(pBuffer);
  TranslatorPointer<unsigned short> p(*bt);
  
  // The following struct is useful for pulling out byte data:

  struct Buffer {
    bheader s_Header;
    ctlbody s_Body;
  };
  Buffer* pB((Buffer*)pBuffer);

  // Do the buffer type independent stuff.

  m_pRunData->s_nRunNumber = p[3]; // Fill in the run number.
  string title;
  for (int i = 0; i < 80; i++) {
    if(pB->s_Body.title[i] == '\0') {
      break;
    }
    title += pB->s_Body.title[i];
  }
  m_pRunData->s_Title = title;



  // Do buffer dependent stuff:

  if (nBufferType == BEGRUNBF) {
    m_pRunData->s_fScalersThisRun = false;
    for(int i = 0; i < m_pRunData->s_Increments.size(); i++) {
      m_pRunData->s_Increments[i] = 0;
    }
    for(int i = 0; i < m_pRunData->s_Totals.size(); i++) {
      m_pRunData->s_Totals[i] = 0;
    }
    m_pRunData->s_IntervalStartTime = 0;
    m_pRunData->s_RunStarted = StringTime(p);
  }
  if (nBufferType == ENDRUNBF) {
    m_pRunData->s_RunEnded  = StringTime(p);
  }
  delete bt;
}
/*!
   Utility function to turn a control body time into a text string.
*/
string
CStandardControlCallback::StringTime(TranslatorPointer<unsigned short> p)
{
  p += sizeof(bheader)/sizeof(short);
  ctlbody* zerobased((ctlbody*)0);
  unsigned long todOffset = (unsigned long)&(zerobased->tod);

  p += todOffset/sizeof(unsigned short);	// Now points to time of day:

  bftime t;
  t.month = *p++;
  t.day   = *p++;
  t.year  = *p++;
  t.hours = *p++;
  t.min   = *p++;
  t.sec   = *p++;


  char timebuffer[1000];
  sprintf(timebuffer, "%s %d, %d, %02d:%02d:%02d",
	  monthnames[t.month], t.day, t.year, t.hours, t.min, t.sec);
  return string(timebuffer);
}
