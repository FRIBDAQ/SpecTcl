/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#ifndef __SRUNCONTEXT_H
#define __SRUNCONTEXT_H

//  The including file must have included <config.h>!!!!


#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

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
  STD(vector)<unsigned long> s_Increments; /*!< Most recent increments. */
  STD(vector)<float>         s_Totals; /*!< The scaler totals as of now. */
  unsigned long s_IntervalStartTime; /*!< When this scaler interval started */
  unsigned long s_IntervalEndTime;   /*!< When this scaler interval ended. */
  STD(string)   s_Title;	     /*!< Run title.  */
  int           s_nRunNumber;	     /*!< Run number. */
  STD(string)   s_RunStarted;	     /*!< Timestamp of run beginning.  */
  STD(string)   s_RunEnded;	     /*!< Timestamp of run ending.     */
} SRunContext;


#endif
