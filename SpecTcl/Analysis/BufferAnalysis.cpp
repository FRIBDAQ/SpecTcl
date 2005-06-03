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

//  This is the implementation of the C language bindings.  
//  we implement in c++ however as unbound, unmangled functions:

#include <config.h>
#include "CBufferProcessor.h"
#include "CLanguageCallback.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
  The list below is used to keep track of the callbacks that have been
  registered.  Each callback is uniquely identified by the triple:
  Callback function pointer.
  Buffer type
  Client data
*/
struct CallbackInfo {
  NSCLBufferCallback* s_pCallback;
  unsigned int        s_nBufferType;
  void*               s_pClientData;
  CLanguageCallback*  s_pCallbackObject;
};
typedef list<CallbackInfo*>  CallbackList;

static CallbackList Callbacks;

static bool
MatchingCallback(CallbackInfo* p, NSCLBufferCallback* cb, unsigned int type,
		 void* userdata)
{
  return ((p->s_pCallback   == cb)      &&
          (p->s_nBufferType == type)    &&
	  (p->s_pClientData == userdata));
}

/*!
   The buffer processor below contains the underlying
   implementation for the jackets in  the following
   code.
*/
static CBufferProcessor Processor;


extern "C" {

  
/*!
  Process an NSCL buffer. Call this once for each buffer gotten from whatever
  event source your application is using.

  \param pBuffer (const void*)
     The buffer to process.
 */
void
ProcessNSCLBuffer(const void* pBuffer)
{
  Processor(pBuffer);
}
  

/*!
  Check if any scaler buffers have been seen yet this run.
  \return bool
  \retval true  - At least one scaler buffer has been seen this run.
  \retval false - No scaler buffers have been seen yet this run.

 */
bool
ScalersSeen()
{
  return Processor.scalersSeen();
}

/*!
    Return the number of scalers in this run.
    \return int
    \retval >= 0   Number of scalers seen this run.
    \retval <  0   No scaler buffers seen yet this run.
*/
int
ScalerCount()
{
  if(ScalersSeen()) {
    return Processor.scalerCount();
  } 
  else {			// No scaler buffers yet.
    return -1;
  }

}
/*!
   Return the increments for a channel from the most recent
   scaler buffer.

   \param nchan (unsigned int [in]):
      Selects which channel (numbered from 0) to return.

   \return long   
   \retval >= 0   - The increment value.
   \retval -1     - No scaler buffers seen yet.
   \retval -2     - Bad scaler channel number.
*/
long
LastIncrement(unsigned int nchan)
{
  if(ScalersSeen()) {
    if(nchan < ScalerCount()) {
      return Processor.lastIncrement(nchan);
    } 
    else {			// Bad channel #
      return -2;
    }
  }
  else {			// No scalers yet
    return -1;
  }
}
/*!
   Return the total counts for a specific scaler channel:
   \param nchan (unsigned int [in]):
      Selects which channel (numbered from 0) to return.
   \return float
   \retval >= 0.0 - Total counts so far.
   \retval -1.0   - No scalers seen yet.
   \retval -2.0   - Channel number out of range.
*/
float
Total(unsigned int nchan)
{
  if(ScalersSeen()) {
    if(nchan < ScalerCount()) {
      return Processor.Total(nchan);
    } 
    else {			// Bad channel #
      return -2.0;
    }
  }
  else {			// No scalers yet
    return -1.0;
  }
}
/*!
   Return the interval start time from the most recently
   processed scaler buffer.
 
  \return long
  \retval >= 0  - Start time in seconds relative to the begin run.
  \retval -1    - No scaler buffers seen yet.
*/
long
LastIntervalStart()
{
  if(ScalersSeen()) {
    return Processor.lastIntervalStart();
  } 
  else {
    return -1;
  }
}
/*!
   Return the interval end time from the most recently processed scaler buffer.
   \return long
   \retval >= 0  - end time in seconds relative to the begin run.
   \retval -1    - No scaler buffers seen yet.
*/
long
LastIntervalEnd()
{
  if(ScalersSeen()) {
    return Processor.lastIntervalEnd();
  }
  else {
    return -1;
  }
}
/*!
    Retrieve the current run title.
    \return const char*
*/
const char*
Title()
{
  return Processor.Title().c_str();
}

/*!
  Return the current run number.  
  
  \return unsigned int
 
 */
unsigned int
RunNumber()
{
  return Processor.runNumber();
}
/*!
  Return the run start time
*/
const char* 
RunStartTime()
{
  return Processor.runStartTime().c_str();
}
/*!
  Return the run end time
*/
const char*
RunEndTime()
{
  return Processor.runEndTime().c_str();
}
/*!
  Add a buffer processing callback.
  \param pCallback (NSCLBufferCallback* [in]):
     Pointer to the callback function.
  \param type (unsigned int [in]):
     Type of buffer to trigger the callback.
  \param userdata (void* [in]):
     User data that will be passed to the callback function.


 */
void
AddBufferCallback(NSCLBufferCallback* cb, unsigned int type, void* userdata)
{
  // We must create a CallbackInfo struct and fill it in:

  CallbackInfo* pInfo  = new CallbackInfo;
  pInfo->s_pCallback   = cb;
  pInfo->s_nBufferType = type;
  pInfo->s_pClientData = userdata;
  pInfo->s_pCallbackObject = new CLanguageCallback(cb, userdata);

  // Register the new callback object:

  Processor.addCallback(type, *(pInfo->s_pCallbackObject));

  // Remember this for when we delete:

  Callbacks.push_back(pInfo);
}

/*!
   Unregister a callback.  To do this we must:
   - Locate the callback in our list.
   - Remove the callback from the list.
   - Delete the callback object.
   - Delete the list object.
   - Remove the deleted list object from the list.
   \param cb (NSCLBufferCallback* [in]):
      Pointer to the callback function.
   \param type (unsigned int [in]):
      Type of buffer the callback is currently registered on.
   \param userdata (void* [in]):
      user data associated with the callback.
*/
void
RemoveBufferCallback(NSCLBufferCallback* cb, unsigned int type, void* userdata)
{
  // Try to locate the callback in the list (No-op if can't find):

  CallbackList::iterator p = Callbacks.begin();
  while(p != Callbacks.end()) {
    CallbackInfo* pInfo = *p;
    if(MatchingCallback(pInfo, cb, type, userdata)) {
      Processor.removeCallback(type, *(pInfo->s_pCallbackObject));
      delete pInfo->s_pCallbackObject;
      delete pInfo;
      Callbacks.erase(p);
      return;			// Since my iterator got invalidated during the erase
    }
    p++;
  }
}

// End de-mangle disable.

}
