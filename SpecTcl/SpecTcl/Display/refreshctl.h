/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   refreshctl.h:
**     This file is a header file for clients of the Xamine refresh management
**     subsystem.  That subsystem is responsible for updating the contents
**     of a window to match the current values of a spectrum.
**     Since this is a time consuming process, the only public entries are
**     triggers for work procedures which will operate in the background.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**
**    @(#)refreshctl.h	8.1 6/23/95 
*/

#ifndef _REFRESHCTL_H
#define _REFRESHCTL_H
#include "XMWidget.h"
#include "panemgr.h"

/*
**  The following client data is passed to the refresh work routine to
**  allow it to store context.  It should be dynamically allocated using
**  new because when the work routine exits for the 'last time' it will
**  delete the storage.
*/
typedef void (*Xamine_ContextDestructor)(XtPointer);
struct Xamine_RefreshContext {
                              XMWidget *pane; /* Pointer to drawing area */
			      Drawable  pixmap; /* Associated pixmap.    */
			      int       pixwid, pixht;
			      pane_db  *windb; /* Pointer to window db. */
			      int       column,
			                row; /* Window coordinates. */
			      Xamine_ContextDestructor destroyer;
			      void *ctx; /* Pointer to internal context */
			    };

typedef void (*RefreshCallback)(Xamine_RefreshContext *ctx, XtPointer user_d);


/*
** The following functions are external public entries to the 
** refreshctl.cc module:
*/

/* Initiate programmatic redraw */

void Xamine_RedrawPane(int column, int row);         /* Spawn off an update. */
void Xamine_RedrawSelectedPane();                    /* Spawn updates for all */
void Xamine_RedrawSpectrum(int specid);	             /* Redraw spectrum. */
void Xamine_CancelPendingUpdate(int column, int row); /* Cancel updates */
void Xamine_CancelUpdates();	/* Cancel pending updates on all panes. */

/* Callback to handle Update buttons */

void Xamine_Update(XMWidget *wid, XtPointer clientd, XtPointer cd);
void Xamine_UpdateAll(XMWidget *wid, XtPointer clientd, XtPointer cd);

/* The following callback handles Exposure and Resize events in a pane: */

void Xamine_PaneRedrawCallback(XMWidget *w, XtPointer userd, XtPointer calld);

/*  The following routines handle timed updates set by the refresh interval */

void Xamine_ScheduleTimedUpdate(XMWidget *w, int col, int row, int seconds);
void Xamine_CancelTimedUpdate(int col, int row);
void Xamine_CancelUpdateTimers();
void Xamine_UpdateTimerRoutine(XtPointer client_data, XtIntervalId *id);

/*  The following declaration describes the work procedure that manages */
/*  the update of a screen                                              */

Boolean Xamine_Refresh(XtPointer client_data);

void Xamine_AddRefreshCallback(RefreshCallback callback, XtPointer user_d);
void Xamine_RemoveRefreshCallback();
void Xamine_GetRefreshCallback(RefreshCallback *cb, XtPointer *ud);

#endif
