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
**    Xamine - NSCL display program.
** Abstract:
**    dfltmgr.h  - This include file provides definitions used by clients of
**                 the pane defaults manager.  The pane defaults manager
**                 manages a defaults values database which can be applied to
**                 a pane which is having a spectrum assigned to it.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**      @(#)dfltmgr.h	8.1 6/23/95 
*/



#ifndef DFLTMGR_H
#define DFLTMGR_H

#include "dispwind.h"


/*
** Constant definitions:
*/


#define XAMINE_DEFAULTS_DIRECTORY     "HOME" /* Env. var where defaults go. */
#define XAMINE_DEFAULTS_FILE "/Xamine.Defaults" /* Filename part of path. */

/*
** Exported functions.
*/

win_attributed *Xamine_GetDefaultGenericAttributes();
win_attributed *Xamine_GetCurrentGenericAttributes();
void            Xamine_SetGenericAttributes(win_attributed* attr);
void            Xamine_SetDefault1DRendition(rendition_1d rendition);
void            Xamine_SetDefault2DRendition(rendition_2d rendition);
void            Xamine_Construct1dDefaultProperties(win_1d *properties);
void            Xamine_Construct2dDefaultProperties(win_2d *properties);
int             Xamine_SaveDefaultProperties();
int             Xamine_ReadDefaultProperties();
int             Xamine_ReadDefaultFile(const char* filename);
void            Xamine_ApplyDefaultsEverywhere();
#endif














