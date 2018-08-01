/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#ifndef CSTANDARDSCALERCALLBACK_H
#define CSTANDARDSCALERCALLBACK_H

#include "CBufferCallback.h"
#include "SRunContext.h"

/*!
   This class provides standard behavior for scaler buffers.
   The CBufferProcessor class registers one of these to process caler buffers.
   When a scaler buffer arrives, the callback object maintains the interval times,
   the increments and the run totals.
*/
class CStandardScalerCallback : public CBufferCallback
{
private:
  SRunContext* m_pRunData;
public:
  CStandardScalerCallback(SRunContext& RunData);
  CStandardScalerCallback(const CStandardScalerCallback& rhs);
  virtual ~CStandardScalerCallback();

  CStandardScalerCallback& operator=(const CStandardScalerCallback& rhs);
  int operator==(const CStandardScalerCallback& rhs) const;
  int operator!=(const CStandardScalerCallback& rhs) const;

  // Class functions:

  virtual void operator()(unsigned int nBufferType, const void* pBuffer);
};



#endif
