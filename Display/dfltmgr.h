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

#ifndef _DFLTMGR_H
#define _DFLTMGR_H

#include "dispwind.h"

/*
** Constant definitions:
*/

#define XAMINE_DEFAULTS_DIRECTORY     "HOME" /* Env. var where defaults go. */
#ifdef unix
#define XAMINE_DEFAULTS_FILE "/Xamine.Defaults" /* Filename part of path. */
#endif
#ifdef VMS
#define XAMINE_DEFAULTS_FILE "XAMINE.DEFAULTS"  /* Filename part of path */
#endif

/*
** Exported functions.
*/

win_attributed *Xamine_GetDefaultGenericAttributes();
void            Xamine_SetDefault1DRendition(rendition_1d rendition);
void            Xamine_SetDefault2DRendition(rendition_2d rendition);
void            Xamine_Construct1dDefaultProperties(win_1d *properties);
void            Xamine_Construct2dDefaultProperties(win_2d *properties);
int             Xamine_SaveDefaultProperties();
int             Xamine_ReadDefaultProperties();
void            Xamine_ApplyDefaultsEverywhere();
#endif
