/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   spcdisplay.h  - This file contains definitions for code that is in
**                   the spectrum displayer module.  This is code responsible
**                   for coordinating the display and update of a spectrum
**                   in a window.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)spcdisplay.h	2.1 12/22/93 
*/

#ifndef _SPCDISPLAY_H
#define _SPCDISPLAY_H
#include "XMWidget.h"
#include "dispwind.h"

#define XAMINE_SCALE_MULTIPLIER (2.0)      /* Scale up/down multiplier. */
void Xamine_DisplayPane(int row, int col, int specid); 
void Xamine_Display(int specid);         /* Display this spectrum. */
void Xamine_DisplayAdvance(int specid);  /* Display spectrum, advance select */

void Xamine_Superimpose(int specid);     /* Superimpose on current spectrum. */

void Xamine_SetAdvance(int advance);     /* Set the window advance. */
int  Xamine_GetAdvance();	         /* Retrieve window advance.  */

void Xamine_SetSelectedRefreshRate(int sec); /* Set the refresh rate of selected */

void Xamine_SetSelectedAxisVisibility(int onofof);
void Xamine_SetSelectedAxisTicks(int onoff);
void Xamine_SetSelectedAxisLabels(int onoff);

void Xamine_SetSelectedNameVisible(int onoff);
void Xamine_SetSelectedIdVisible(int onoff);
void Xamine_SetSelectedDescriptionVisible(int onoff);
void Xamine_SetSelectedMaxVisible(int onoff);
void Xamine_SetSelectedUpdateVisible(int onoff);
void Xamine_SetSelectedObjectsVisible(int onoff);

void Xamine_SetSelectedFlip(int flipped);

void Xamine_SetSelectedReduction(reduction_mode mode);

void Xamine_SetSelectedScaleMode(int newmode);
void Xamine_MultiplySelectedScale(float multiplier);
void Xamine_MultiplySelectedScaleCb(XMWidget *w, XtPointer ud, XtPointer cd);
void Xamine_ClearSelectedPane(XMWidget *w, XtPointer ud, XtPointer cd);
void Xamine_ClearPane(int row, int col);
#endif	
