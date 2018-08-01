/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   logging.h   - This file contains definitions for classes, and externals
**                 which handle the logging of events etc. in the NSCL display
**                 program.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   June 21, 1993
*/

#ifndef LOGGING_H
#define LOGGING_H
#include <X11/Intrinsic.h>

#include "XMWidget.h"

class LogFile
      {
      protected:
      public:
	int LogMessage(const char *txt);
	int ContinueMessage(const char *txt);	/* Continue a message. */
      }; 

void Xamine_SetupLogFile(XMWidget *w, XtPointer userd, XtPointer clientd);
#ifndef _LOGGING_CC
extern Boolean Xamine_logging;
extern char    *Xamine_logfile;
extern LogFile Xamine_log;
#endif

#endif











