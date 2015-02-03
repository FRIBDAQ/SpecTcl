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

/* 
   Change log:
   $Log$
   Revision 5.3  2006/11/17 16:08:38  ron-fox
   Defect 228 fix: Printer defaults could wind up only partially
   initialized.. this resulted in bad Xamine.Default files and
   could cause printing to fail or crash Xamine.

   Revision 5.2.2.1  2006/11/17 14:41:55  ron-fox
   Defect 228 - Xamine can make bad Xamine.Default files which can cause printing
   to fail because the print defauts can be set to whacky values.

   Revision 5.2  2005/06/03 15:18:55  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.1  2004/12/21 17:51:13  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:55:35  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.3.4.1  2004/04/12 16:37:31  ron-fox
   - Use etc for etc stuff with link named Etc rather than the other way around.
   - Extract all Makefile definitions into separate include files so the user makefile
     becomes less variable with time.

   Revision 4.3  2003/08/25 16:25:30  ron-fox
   Initial starting point for merge with filtering -- this probably does not
   generate a goo spectcl build.

   Revision 4.2  2003/04/02 18:35:25  ron-fox
   Added support for central Xamine.Default files that live in any of:
   $SpecTclHome/etc or $SpecTclHome/etc as well as the user's $HOME/Xamine.Defaults.  The effect of having multiple files is cumulative.

*/

#ifndef _DFLTMGR_H
#define _DFLTMGR_H

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














