/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   titles.h:
**      Header file intended for clients of the titles drawing module.
**      The titles drawing module is responsible for drawing all of the
**      titles on a spectrum.
** Author:
**      Ron FOx
**      NSCL
**      Michigan State University
**      East Lansing, MI 48824-1321
**      @(#)titles.h	8.1 6/23/95 
*/

#ifndef _TITLES_H

#define _TITLES_H
#include "XMWidget.h"
#include "dispwind.h"
#include "titles.h"
#include "refreshctl.h" 

void Xamine_DrawTitles(Xamine_RefreshContext *ctx, win_attributed *def);

#endif
