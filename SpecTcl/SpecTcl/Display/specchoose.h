/*
** Facility:
**    Xamine  - NSCL display program.
** Abstract:
**    specchoose.h  - This include file defines clients of the specchoose.h
**                    module.  This module puts up spectrum chooser modules.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**    @(#)specchoose.h	8.1 6/23/95 
*/

#ifndef _SPECCHOOSE_H
#define _SPECCHOOSE_H

#include "XMDialogs.h"
/*
** This structure is the call back data for the following functions.
*/
struct Xamine_ChooserCbData {
                              int default_ok; /* True to default OK button. */
			      void (*displayfunc)(int);
                            };

void Xamine_ChooseSpectrum(XMWidget *w, XtPointer clientd, XtPointer calld);

int  Xamine_MatchSpecName(char *name);

#endif
