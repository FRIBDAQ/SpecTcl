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

#ifndef _LOGGING_H
#define _LOGGING_H
#include <X11/Intrinsic.h>
#include "XMWidget.h"

class LogFile
      {
      protected:
      public:
	int LogMessage(char *txt);
	int ContinueMessage(char *txt);	/* Continue a message. */
      }; 

void Xamine_SetupLogFile(XMWidget *w, XtPointer userd, XtPointer clientd);
#ifndef _LOGGING_CC
extern Boolean Xamine_logging;
extern char    *Xamine_logfile;
extern LogFile Xamine_log;
#endif

#endif











