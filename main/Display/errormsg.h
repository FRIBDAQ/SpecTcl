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
/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   errormsg.h   - this file contains definitions for software
**                  that generates error messgaes of various sorts.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**   @(#)errormsg.h	8.1 6/23/95 
*/

#ifndef ERRORMSG_H
#define ERRORMSG_H

#include "XMWidget.h"
#include "XMDialogs.h"

void Xamine_error_msg (XMWidget *parent, const char *msg);

#endif
