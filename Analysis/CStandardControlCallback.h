/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/
#ifndef __CSTANDARDCONTROLCALLBACK_H
#define __CSTANDARDCONTROLCALLBACK_H

#ifndef __CBUFFERCALLBACK_H
#include "CBufferCallback.h"
#endif

#ifndef __SRUNCONTEXT_H
#include "SRunContext.h"
#endif

#ifndef __TRANSLATORPOINTER_H
#include <TranslatorPointer.h>
#endif

/*!
  This class is registered by the buffer processor at the front of the
  list of callbacks for control buffers (begin, end, pause, resume).
  The class's action is partly independent of the buffer type: the run context
  is updated to set
  - s_nRunNumber
  - s_Title
  On a begin run:
  - s_RunStarted is set.
  - s_fScalersThisRun <- false.
  - s_Increments <- 0.
  - s_Totals     <- 0.
  - s_IntervalStartTime <-  0
  - s_IntervalEndTime   <- 0
  On an end run:
  - s_RunEnded is set.

*/
class CStandardControlCallback : public CBufferCallback
{
private:
  SRunContext* m_pRunData;
public:
  // constructors and other canonicals:

  CStandardControlCallback(SRunContext& RunData);
  CStandardControlCallback(const CStandardControlCallback& rhs);

  virtual ~CStandardControlCallback();
  
  CStandardControlCallback& operator=(const CStandardControlCallback& rhs);
  int operator==(const CStandardControlCallback& rhs) const;
  int operator!=(const CStandardControlCallback& rhs) const;
  

  // Class operations:

  virtual void operator()(unsigned int nType, const void* pBuffer);

  // Utilities:

  static STD(string) StringTime(TranslatorPointer<unsigned short> p);
};

#endif
