/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#ifndef __CBUFFERPROCESSOR_H
#define __CBUFFERPROCESSOR_H

#ifndef __CBUFFERCALLBACKLIST_H
#include "CBufferCallbackList.h"
#endif

#ifndef __TRANSLATORPOINTER_H
#include <TranslatorPointer.h>
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif




// Forward Class definitions:

class CBufferCallbackList;
class CBufferCallback;


struct _SRunContext;
typedef _SRunContext SRunContext;


/*!
   This class forms the public C++ interface to the buffer processor.
   The model is that this top level class gets intantiated.  The client
   of the class will register callbacks to process buffers of particular types.
   The client submits buffers for processing to this class which simply pulls out the
   buffer type and dispatches to the set of callbacks that were established for
   that type.

   This simplified model is augmented by recognizing that there are some standardized
   things users will want to do.  These are accomodated by registering some 
   stock callbacks at construction time.  At present these stock callbacks 
   handle control and scaler buffers to maintain information about the run and
   the incremental and total scalers for a run.

   A sketch of a sample user program.  Suppose you want a program that reconstructs
   the sums of all scalers for a specified run.  You would:
   - Write code to select a run file.
   - Create an instance of the buffer processor.
   - Create a buffer callback that prints out the sums and
     register it to handle end of run buffers.
   - Start feeding buffers from the run file into the buffer processor until the
     end of file.

   Simple as that.

*/
class CBufferProcessor
{
private:
  typedef STD(vector)<CBufferCallbackList> CallbackVector;
  CallbackVector           m_Callbacks;
  SRunContext*             m_pRunContext;
  CBufferCallback*         m_pControlCallback;
  CBufferCallback*         m_pScalerCallback;
public:
  CBufferProcessor();
private:			//! For now copy construction illegal.
  CBufferProcessor(const CBufferProcessor& rhs);
public:
  virtual ~CBufferProcessor();
private:			//! For now assignment is illegal.
  CBufferProcessor& operator=(const CBufferProcessor);
  int operator==(const CBufferProcessor& rhs) const; //!< Comparision makes no sense
  int operator!=(const CBufferProcessor& rhs) const; //!< Comparison makes no sense.
public:
 
  // The selector here is intended for test purposes:

  SRunContext* getContext() {
    return m_pRunContext;
  }


  // class functions that access the run context data:

  bool          scalersSeen() const;
  unsigned int  scalerCount() const;
  long          lastIncrement(unsigned int channel) const;
  float         Total(unsigned int channel) const;
  unsigned long lastIntervalStart() const;
  unsigned long lastIntervalEnd() const;
  STD(string)   Title() const;
  int           runNumber() const;
  STD(string)   runStartTime() const;
  STD(string)   runEndTime() const;
  

  // Class functions that manipulate callbacks:

  void addCallback(unsigned int nBufferType, CBufferCallback& callback);
  void removeCallback(unsigned int nBufferType, CBufferCallback& callback);
  
  // Processing buffers:

  virtual void operator()(const void* pBuffer);

  // Support the callbacks can count on:

  static TranslatorPointer<unsigned short> 
                 getTranslatingPointer(const void* pBuffer);


private:
  void invokeCallbacks(unsigned int nBufferType, const void* pBuffer);
  void extendCallbacks(unsigned int nBufferType);


  

};


#endif
