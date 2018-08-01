/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#ifndef SRUNCONTEXT_H
#define SUNCONTEXT_H

//  The including file must have included <config.h>!!!!

#include <vector>
#include <string>
/*!
    The Run Context structure contains elements that maintain the context of
   the run analysis between timeouts.  It is certainly possible that the
   user may extend the buffer processing class (CBufferProcessor) so that
   there are additional bits of context besides this... this however, is the
   standard context.

*/
typedef struct _SRunContext
{
  bool s_fScalersThisRun;	/*!< True if scalers were seen this run. */
  int  s_nScalerCount;		/*!< Number of scalers acquired this run.  */
  std::vector<unsigned long> s_Increments; /*!< Most recent increments. */
  std::vector<float>         s_Totals; /*!< The scaler totals as of now. */
  unsigned long s_IntervalStartTime; /*!< When this scaler interval started */
  unsigned long s_IntervalEndTime;   /*!< When this scaler interval ended. */
  std::string   s_Title;	     /*!< Run title.  */
  int           s_nRunNumber;	     /*!< Run number. */
  std::string   s_RunStarted;	     /*!< Timestamp of run beginning.  */
  std::string   s_RunEnded;	     /*!< Timestamp of run ending.     */
} SRunContext;


#endif
