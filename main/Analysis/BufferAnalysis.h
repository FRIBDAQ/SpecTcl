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

/*!
   This module provides a C compatible interface to the NSCL buffer analysis
   package.  Note that under the hood this is really just a C callbable front
   end to a single static CBufferProcessor object.


*/
#ifndef __BUFFERANALYSIS_H
#define __BUFFERANALYSIS_H

#ifdef __cplusplus
extern "C" {
#endif
/*  NSCLBufferCallback types represent buffer specific callbacks the user
      can add to the processing chain.
*/
  
  typedef void   (NSCLBufferCallback)(unsigned int, const void*, void*);

  void ProcessNSCLBuffer(const void* pBuffer); /*!< Process a single buffer */
  int  ScalersSeen();		               /*!< scalers present this run? */
  int  ScalerCount();		               /*!< # scalers this run */
  unsigned long LastIncrement(unsigned int n); /*!< increment for a channel */
  float Total(unsigned int n);                 /*!< Totals for a channel    */
  long LastIntervalStart();	               /*!< scaler interval start time   */
  long LastIntervalEnd();                       /*!< Scaler interval end time */
  const char*  Title();                        /*!< Run title. */
  unsigned int RunNumber();                    /*!< Number of current run   */
  const char*  RunStartTime();                 /*!< When the run started.  */
  const char*  RunEndTime();                   /*!< When the run ended.    */

  void AddBufferCallback(NSCLBufferCallback* cb,
			 unsigned int        type,
			 void*               userdata); /*!< add a callback */
  void RemoveBufferCallback(NSCLBufferCallback* cb,
			    unsigned int        type,
			    void*               userdata); /*!< get rid of a callback */



#ifdef __cplusplus
}
#endif

#endif
