/*
 ** Facility:
 **   Xamine  - NSCL display program
 ** Abstract:
 **   refreshctl.cc:
 **     This file controls the upper levels of Xamine pane updates.
 **     Since updates can be triggered for several reasons and
 **     since, in the case e.g. of 2-d spectra, they can be quite time
 **     consuming,  the actual update operations are performed in
 **     work routines.  The work routine segments the update process
 **     into several phases and, in the case where a large number of
 **     channels is present, also segments the channel update into several
 **     subregion updates.  This should keep the user interface lively during
 **     spectrum update operations.
 ** Author:
 **   Ron Fox
 **   NSCL
 **   Michigan State University
 **   East Lansing, MI 48824-1321
 */
static char *sccsinfo="@(#)refreshctl.cc	2.10 5/26/94 ";
     

/*
 ** Include files required:
 */
#include <stdio.h>

#include "XMWidget.h"
#include "panemgr.h"
#include "refreshctl.h"
#include "axes.h"
#include "scaling.h"
#include "titles.h"
#include "chanplot.h" 
#include "dfltmgr.h"
#include "colormgr.h"
#include "dispshare.h"
#include "spcdisplay.h"
#include "grobjdisplay.h"
#include "pkgmgr.h"
#include "queue.h"
#include "gc.h"
#include "xaminegc.h"
#include "gcmgr.h"
/*
 ** Macros:
 */
#define TF(log) ((log) ? "True" : "False")
#define IDLE_CHECK_INTERVAL 100
/*
 ** Local data types:
 */
     struct coords { int row, col; };

/*
 ** Static data:
 */
static RefreshCallback refresh_callback  = NULL;
static XtPointer       refresh_user_data = NULL;
#ifdef VMS
static Queue update_list;
#else
static Queue<Xamine_RefreshContext>  update_list;
#endif
/*
 ** External functions referenced:
 */

extern spec_shared *xamine_shared;
int specis1d(int spec);

static Pixmap CreatePixmap(Display *display, Drawable drawable, unsigned int width,
		    unsigned int height, unsigned int depth)
{
  Pixmap p;
  p = XCreatePixmap(display, drawable, width, height, depth);
  return p;

}
static void FreePixmap(Display *d, Pixmap p)
{
  XFreePixmap(d,p);
}

/*
** Functional Description:
**  CheckAbort:
**    This function checks to see if an abort has finished prior to 
**    restarting an update on the pane.
** Formal Parameters:
**    XtPointer userd:
**      Actually the serial number of the pane that we're checking.
**    XtIntervalid *id:
**      Our timer id... not used.
*/
static void CheckAbort(XtPointer userd, XtIntervalId *id)
{
  /*
  ** First determine which pane we're dealing with.
  */
  int index = (int)userd;
  int ncol = Xamine_Panecols();
  int column = index  % WINDOW_MAXAXIS;
  int row    = column / WINDOW_MAXAXIS;

  /* Set the timer id to null before proceding.  */

  pane_db *pdb = Xamine_GetPaneDb();
  pdb->updatetimer(row, column, 0);

  /* All we really have to do is call the redraw routine. It'll reschedule us 
  ** if the pane is still not yet idle.
  */

  Xamine_RedrawPane(column, row);

}

/*
** Functional Description:
**   ClearWindow:
**      This function clears a window whether or not the window is
**      being drawn to through a pixmap or directly.
** Formal Parameters:
**    Xamine_RefreshContext *ctx:
**       Redraw context.  If the pixmap entry is zero, then the
**       window is just cleared via XClearWindow.  Otherwise,
**       we assume the pixmap is as large as the window and
**       clear it using a call to XFillRectangle with solid fill and
**       fg set to the window default background color.
*/
static void ClearWindow(Xamine_RefreshContext *ctx)
{
  Display *d = XtDisplay(ctx->pane->getid());
  Window   w = XtWindow(ctx->pane->getid());
  XamineSpectrumGC *xgc;

  if(ctx->pixmap == 0) {
    XClearWindow(d, w);
  }
  else {
    XGCValues gcv;
    XWindowAttributes a;

    xgc = Xamine_GetSpectrumGC(*ctx->pane);
    GC gc = xgc->gc;

    ctx->pane->GetAttribute(XmNbackground, &(gcv.foreground));
    gcv.background = gcv.foreground;
    gcv.function   = GXcopy;
    XGetWindowAttributes(d,w, &a);
    
    xgc->SetValues( &gcv, GCFunction | GCForeground | GCBackground );
    XFillRectangle(d, ctx->pixmap, gc, 0,0, a.width+1, a.height+1);
  }
}

/*
 ** Functional Description:
 **   Xamine_AddRefreshCallback:
 **       Sets a refresh callback.  Refresh callbacks are called during
 **       one of the phases of drawing a spectrum.
 ** Formal Parameters:
 **   RefreshCallback callback:
 **     Pointer to the function to call on refresh.
 **   XtPointer user_d:
 **     User data passed without interpretation to the callback.
 */
void Xamine_AddRefreshCallback(RefreshCallback callback, XtPointer user_d)
{
  refresh_callback = callback;
  refresh_user_data= user_d;
}

/*
 ** Functional Description:
 **   Xamine_RemoveRefreshCallback:
 **      This function removes any present refresh callback.
 ** Formal Parameters:
 **   NONE:
 */
void Xamine_RemoveRefreshCallback()
{
  refresh_callback  = NULL;
  refresh_user_data = NULL;
}
/*
 ** Functional Description:
 **   Xamine_GetRefreshCallback:
 **     This function returns the current state of the refresh callback
 **     and refresh user data.  This can be used by the caller to either
 **     determine that a callback exists or to chain existing callbacks.
 ** Formal Parameters:
 **   RefreshCallback *cb:
 **    Points to a buffer to hold the callback.
 **   XtPointer *ud:
 **    Points to a buffer to hold the user data associated with the callback.
 */
void Xamine_GetRefreshCallback(RefreshCallback *cb, XtPointer *ud)
{
  *cb = refresh_callback;
  *ud = refresh_user_data;
}

/*
 ** Functional Description:
 **    Xamine_RedrawPane:
 **      Redraws a single pane.  This function determines if a redraw is
 **      in progress.  If it is, the state machine is reset.  If not,
 **      then a redraw operation is begun by adding a work procedure to
 **      manage the redraw.
 ** Formal Parameters:
 **    int column:
 **       Column coordinates of the pane to update.
 **    int row:
 **       Row coordinates of the pane to update.
 */
void Xamine_RedrawPane(int column, int row)
{
  pane_db *db;
  int     cellno;
  XMWidget *pane;
  
  db = Xamine_GetPaneDb();
  
  /*
  ** If the pane is not in the idle state, then action depends on actual
  ** state.
  */
  
  if(db->refresh_state(row, column) != rfsh_idle)  {
    switch(db->refresh_state(row, column)) {
    case rfsh_queued:
      return;			/* It'll get done right eventually. */
    case rfsh_titles:
    case rfsh_axes:
    case rfsh_contents:
    case rfsh_grobjs:
    case rfsh_callback:
    case rfsh_beginning:	/* Must abort then start re-try proc. */
    default:
      db->refresh_state(row, column, rfsh_aborting);
    case rfsh_aborting:		/* Need to queue a wait/ retry procedure. */
      if(db->updatetimer(row, column) != 0) /* Already a timer queued. */
	return;
      pane = Xamine_GetDrawingArea(row, column);
      pane->GetAttribute(XmNuserData, &cellno);
      db->updatetimer(row, column, 
	       XtAppAddTimeOut(XtWidgetToApplicationContext(pane->getid()),
			       1000, CheckAbort, (XtPointer)cellno));
      return;
    }
  }
  
  
  /* If idle, then queue up the redraw.
   */
  
  db->refresh_state(row, column, rfsh_queued); /* queueing update:  */
  
  Xamine_RefreshContext context;
  context.pane   = Xamine_GetDrawingArea(row, column);
  context.windb  = db;
  context.column = column;
  context.row    = row;
  context.destroyer = NULL;	/* Filled in by work procs. */
  context.ctx    = NULL;	/* Filled in by work proc. */
  context.pixmap = (Drawable)NULL;	/* Filled in by work proc. */
  
  /*
   ** If the queue is empty, then schedule the work proc to process it.
   */
  if(update_list.IsEmpty())
    XtAppAddWorkProc(XtWidgetToApplicationContext(context.pane->getid()),
		     Xamine_Refresh,
		     &update_list);
  
  /*
   ** Enter the context in the update list queue.
   */
  
  update_list.Append(context); /* Append the context entry. */
  
}

/*
 ** Functional Description:
 **    Xamine_RedrawSelectedPane:
 **      Initiates a redraw of the currently selected pane.
 **      This just translates into a call to Xamine_RedrawPane.
 */
void Xamine_RedrawSelectedPane()
{
  Xamine_RedrawPane(Xamine_PaneSelectedcol(), Xamine_PaneSelectedrow());
}

/*
 ** Functional Description:
 **   Xamine_CancelPendingUpdate:
 **      This function cancels updates that are in progress.
 **      This is done by locating the item in the update_list queue.
 **      If the item is at the front of the queue, and its state is not
 **      rfrsh_queued, then the update is actively in progress and we must
 **      request the work procedure to abort it.  If the update is anywhere
 **      else in the queue or queued, then we can just remove it from the
 **      list.
 ** Formal Parameters:
 **   int column:
 **      Column coordinates of affected pane.
 **   int row:
 **      Row coordinates of affected pane.
 */
void Xamine_CancelPendingUpdate(int column, int row)
{
  pane_db *db = Xamine_GetPaneDb();
  
  /* If no pending update just return */
  
  if(db->refresh_state(row, column) == rfsh_idle) 
    return;
  /*
   ** If actively updating, then mark for abort then return.
   */
  if(db->refresh_state(row, column) != rfsh_queued) {
    db->refresh_state(row, column, rfsh_aborting);
    if(db->updatetimer(row, column) != 0) { /* Cancel any pending timers. */
      XtRemoveTimeOut(db->updatetimer(row, column));
      db->updatetimer(row, column, 0);
    }
    return;
  }
  /* It's supposed to be queued so locate the entry in the queue, 
   ** remove it from the queue and mark the pane as idle.
   */
#ifdef VMS
  QIterator q(update_list);
#else
  QIterator<Xamine_RefreshContext>  q(update_list);
#endif
  while(!q.Last()) {
    Xamine_RefreshContext &e = q.Next(); /* Get a queue entry. */
    if((e.column == column) && (e.row == row) ) {
      q.RemoveThis();
      db->refresh_state(row, column, rfsh_idle);
    }
  }
  
}

/*
 ** Functional Description:
 **   Xamine_CancelUpdates:
 **     Runs through all of the panes in the database and cancels all pending
 **     updates.
 */
void Xamine_CancelUpdates()
{
  pane_db *db = Xamine_GetPaneDb();
  int ncol   = db->nx();
  int nrow   = db->ny();
  
  for(int row = 0; row < nrow; row++) {
    for(int col = 0; col < ncol; col++) {
      Xamine_CancelPendingUpdate(col, row);
    }
  }
  
}

/*
 ** Functional Description:
 **   Xamine_Update:
 **     This function is called whenever the Update button is hit.
 **     What we essentially do is call Xamine_RedrawSelectedPane.
 ** Formal Parameters: (All ignored)
 **   XMWidget *w:
 **     Widget which initiated the call.
 **   XtPointer userd:
 **     User data (unused).
 **   XtPointer calld:
 **     Call specific data (unused).
 */
void Xamine_Update(XMWidget *w, XtPointer userd, XtPointer calld)
{
  Xamine_RedrawSelectedPane();
}

/*
 ** Functional Description:
 **    Xamine_UpdateAll:
 **      Callback which is invoked as a result of the Update All button.
 **      This function loops over all defined spectra and calls 
 **      Xamine_RedrawPane as needed.
 ** Formal Parameters (All ignored):
 **    As for any XM++ callback.
 */
void Xamine_UpdateAll(XMWidget *w, XtPointer userd, XtPointer clientd)
{
  int ir,ic;
  pane_db *db;
  int ncol, nrow;
  
  ncol = Xamine_Panecols();
  nrow = Xamine_Panerows();
  db   = Xamine_GetPaneDb();
  
  for(ir = 0; ir < nrow; ir++)
    for(ic = 0; ic < ncol; ic++) {
      if(db->defined(ic, ir)) 
	Xamine_RedrawPane(ic, ir); /* Only redraw defined spectra */
      
    }
}

/*
 ** Functional Description:
 **   Xamine_PaneRedrawCallback:
 **     This function is a callback that can be placed on a pane widget to
 **     do a callback.  The typical use is to define it as the resize and
 **     expose callback function.
 ** Formal Parameters:
 **   XMWidget *w:
 **     Widget (pane) to redraw.  We can figure out the X/Y of it by
 **     looking at the XmNuserData resource and the geometry of the panes.
 **   XtPointer *userd:
 **     User data, unused.
 **   XtPointer *calld:
 **     Callback data, unused.
 */
void Xamine_PaneRedrawCallback(XMWidget *w, XtPointer userd, XtPointer calld)
{
  int index;
  int column, row;
  int ncol;
  XmDrawingAreaCallbackStruct *st = (XmDrawingAreaCallbackStruct *)calld;
  
  
  /* First fetch the index and figure out the coordinates of the pane that 
   ** got the event.
   */
  
  w->GetAttribute(XmNuserData, &index);
  
  ncol = Xamine_Panecols();
  
  column = index % WINDOW_MAXAXIS;
  row    = index / WINDOW_MAXAXIS;
  

  /* If called for resize, schedule a refresh: */

  if(st->reason == XmCR_RESIZE) {
    Xamine_RedrawPane(column, row);
    return;
  }
  
  /* The switch statement below is used to determine which event has
   ** been handed to us.  
   **   If the event was an expose and there's a backing
   ** store defined, then we can just directly update the window from the
   ** backing store image.
   **   If the event was a resize, then we must schedule a complete redraw
   ** of the pane from the spectrum data as we can't scale the image pixmap.
   */
  if(st->event == NULL) return;
  
  XAnyEvent  *e = (XAnyEvent *)st->event;
  XConfigureEvent *config;
  XExposeEvent    *expose;
  XamineSpectrumGC *xgc = Xamine_GetSpectrumGC(*w);
  switch(e->type) {
  case Expose:
    /*
     ** If there is a backing store image, then we redraw the segment of
     ** the image from the backing store.  Otherwise we wait until the
     ** last expose event for the window and schedule a redraw.
     */
    expose = (XExposeEvent *)e;
    {
      XGCValues gcv;
      Drawable img = Xamine_GetBackingStore(row, column);
      if((img != (Drawable)NULL) && 
	 (Xamine_GetPaneDb()->refresh_state(row,column)    == rfsh_idle)) {	 /* Redraw from image. */
	gcv.function = GXcopy;
	xgc->SetValues(&gcv, GCFunction);
	XCopyArea(expose->display, img, expose->window, xgc->gc,
		  expose->x, expose->y,     /* src */
		  expose->width, expose->height, /* shape */
		  expose->x, expose->y);     /* dst */
		  
      }
      else {			                      /* Redraw from data.   */
	if((expose->count == 0) && 
	   (Xamine_GetPaneDb()->refresh_state(row, column) == rfsh_idle)) {                      /* Wait till last evt  */
	  Xamine_RedrawPane(column, row);             /* full redraw.        */
	}
      }
    }
    
    break;
  default:			/* Ignore all other event types for now. */
    break;
  }
}

/*
 ** Functional Description:
 **   Xamine_ScheduleTimedUpdate:
 **     This function schedules a timed update.  A timed update is performed
 **     by setting an application timeout handler.  The timeout handler
 **     when it fires will schedule the work procedure and re-initiate
 **     itself.
 ** Formal Parameters:
 **    XMWidget *w:
 **      Widget which can be used to get an application context.
 **    int col:
 **      Column coordinates of the pane to update.
 **    int row:
 **      Row Coordinates of the pane to update.
 **    int seconds:
 **      Number of seconds between updates.
 */
void Xamine_ScheduleTimedUpdate(XMWidget *w, int col, int row, int seconds)
{
  XMWidget *pane;
  pane_db  *pdb;
  
  pane = Xamine_GetDrawingArea(row, col);
  pdb  = Xamine_GetPaneDb();
  
  /*  If the pane is defined then do it: */
  
  if(pdb->defined(col, row)) {
    /* Cancel any pending timers: */
    
    if( (pdb->updatetimer(row, col) != (XtIntervalId)NULL) && /* Remove any pending */
       (pdb->refresh_rate(col, row) != 0)) { /* timeouts: */
      XtRemoveTimeOut(pdb->updatetimer(row,col));
    }
    pdb->updatetimer(row,col,
		     XtAppAddTimeOut(XtWidgetToApplicationContext(pane->getid()),
				     seconds*1000,
				     Xamine_UpdateTimerRoutine,
				     (XtPointer)pane));
    pdb->refresh_rate(col, row, seconds);
  }
}

/*
 ** Functional Description:
 **     Xamine_CancelTimedUpdate:
 **       If there are pending timer updates on the pane, then this function
 **       kills them off.
 ** Formal Parameters:
 **    int column:
 **      Column coordinates of the pane to affect.
 **    int row:
 **      Row coordinates of the pane to affect.
 */
void Xamine_CancelTimedUpdate(int column, int row)
{
  pane_db  *pdb;
  
  pdb = Xamine_GetPaneDb();
  if( (pdb->updatetimer(row, column) != (XtIntervalId)NULL) &&
     (pdb->refresh_rate(column, row) != 0)) {
    pdb->refresh_rate(column, row,0);
    XtRemoveTimeOut(pdb->updatetimer(row, column));
    pdb->updatetimer(row, column, (XtIntervalId) (XtIntervalId *)NULL);
  }
}

/*
 ** Functional Description:
 **   Xamine_CancelUpdateTimers()
 **     This function cancels all pending update timers.
 */
void Xamine_CancelUpdateTimers()
{
  pane_db *pdb = Xamine_GetPaneDb();
  int      ncol = Xamine_Panerows();
  int      nrow = Xamine_Panecols();
  
  for(int row = 0; row < nrow; row++) {
    for(int col = 0; col < ncol; col++) {
      Xamine_CancelTimedUpdate(col, row);
    }
  } 
}

/*
 ** Functional Description:
 **   Xamine_UpdateTimerRoutine:
 **     This function is called each time a timeout timer expires.
 **     If the pane still contains a spectrum, we schedule a work routine
 **     to update the pane and reschedule ourselves for the next timeout
 **     interval.
 ** Formal Parameters:
 **    XtPointer wid:
 **      Actually a pointer to the widget object corresponding to the 
 **      pane to redraw.
 **    XtIntervalId *tid:
 **      Timer id we are servicing.
 */
void Xamine_UpdateTimerRoutine(XtPointer wid, XtIntervalId *tid)
{
  XMWidget *pane = (XMWidget *)wid;
  int      ncol;
  int      index;
  int       col, row;
  pane_db   *pdb;
  
  /* First get the database pointer, the user data and from that figure
   ** out the pane coordinates.
   */
  pdb = Xamine_GetPaneDb();
  pane->GetAttribute(XmNuserData, &index);
  ncol= Xamine_Panecols();
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  
  /* Now schedule the work procedure and reschedule us if the spectrum
   ** is still defined: 
   */
  if(pdb->defined(col, row)) {
    if( (pdb->refresh_rate(col,row) != 0)) {
      Xamine_RedrawPane(col, row);
      pdb->updatetimer(row, col,
		       XtAppAddTimeOut(XtWidgetToApplicationContext(pane->getid()),
				       pdb->refresh_rate(col,row)*1000,
				       Xamine_UpdateTimerRoutine,
				       (XtPointer)pane));
    }
    else {
      pdb->updatetimer(row, col, (XtIntervalId)NULL);
      (pdb->getdef(col, row))->noautoupdate();
    }
  }
  else	{			/* Kill off the timer: */
    pdb->updatetimer(row, col, (XtIntervalId)NULL);
  }
  
}

/*
 ** Functional Description:
 **   Xamine_Refresh:
 **    This function drives the refresh operation.  The refresh operation
 **    is state driven using the refreshstate method in the pane database.
 **    entry corresponding to the row and column of the thing to be refreshed.
 ** Formal Parameters:
 **    XtPointer client_data:
 **      This is a pointer to an Xamine_RefreshContext queue which contains
 **      the list of panes we should be updating.
 **      what it is that we are updating.
 ** NOTE:
 **   If we are called and the item being updated is not defined, then
 **   we immediately clear the database and declare ourselves done.
 **Modification:
 **   We order the processing so that the axes are drawn last.  This should
 **   preserve the tick marks for at least some cases of color image
 **   2-d displays.
 */
Boolean Xamine_Refresh(XtPointer client_data)
{
  
  /*
   ** Get the queue pointer from the client data structure.  If the queue
   ** is empty then we're done and can stop processing until the next time
   ** something is put in the queue.
   */
  
#ifdef VMS
  Queue *q = (Queue *)client_data;
#else
  Queue<Xamine_RefreshContext> *q = 
    (Queue<Xamine_RefreshContext> *)client_data;
#endif

  if(q->IsEmpty())
    return True;

  /* Get the context block from the queue non destructively.  */


  Xamine_RefreshContext &c = q->Peek();
  Xamine_RefreshContext *ctx = &c; /* Compatibility with old code. */

  // It's possible external forces have monkeyed with the drawable:

  ctx->pixmap = Xamine_GetBackingStore(ctx->row, ctx->column);


  pane_db *pdb = ctx->windb;
  win_attributed *def = pdb->getdef(ctx->column, ctx->row);
  Display *display = XtDisplay(ctx->pane->getid());
  Window  window   = XtWindow(ctx->pane->getid());
  XWindowAttributes  wattrib;
  int floor;

 /* If the window is null, then the widget is not realized, we 
  ** set the state to idle and return finally
  */
  if(window == (Window)NULL) {
    pdb->refresh_state(ctx->row, ctx->column, rfsh_idle);
    goto display_done;
  }
  /* If the window is not mapped then finish the update since 
  ** when it becomes mapped we'll get an exposure event to start us off 
  ** again
  */
  XGetWindowAttributes(display, window, &wattrib);
  if(wattrib.map_state != IsViewable) {
    pdb->refresh_state(ctx->row, ctx->column, rfsh_idle);
    goto display_done;
  }

  /* If the spectrum has become undefined, or has a null attributes block,
  **  clear the widget region, indicate we're done and exit.
  */
  if((!pdb->defined(ctx->column, ctx->row)) || (def == NULL)) {
    XClearWindow(display, window);
    pdb->refresh_state(ctx->row, ctx->column, /* Indicate done. */
		      rfsh_idle);
    goto display_done;		/* Don't restart this instance of the proc. */
  }

  /* If the histogrammer yanked the spectrum out from underneath us, then
  ** we abort the operation and clear the pane.
  */
  if(xamine_shared->gettype(def->spectrum()) == undefined) {
    XClearWindow(display, window);
    pdb->refresh_state(ctx->row, ctx->column, rfsh_idle); /* stop... */
    pdb->undefine(ctx->column, ctx->row);

    /* Adjust the menus/buttons   */
 
    if( (ctx->row == Xamine_PaneSelectedrow()) && 
        (ctx->column == Xamine_PaneSelectedcol())) {
          Xamine_DisableSpectrumSelectedPackage();
          Xamine_Disable1dSelectedPackage();
          Xamine_Disable2dSelectedPackage();
    }
    int nx = pdb->nx();
    int ny = pdb->ny();
    int num = 0;
    for(int j = 0; j < ny; j++)
      for(int i = 0; i < nx; i++)
        if(pdb->defined(i,j))num++;
    if(num == 0) Xamine_DisableSpectrumPresentPackage();

    goto display_done;		             /* Finish off this one. */

  }

  /* If the dimensionality of the spectrum has changed then we 
  ** Must reset the attributes to the defaults, then we re-set the spectrum
  ** contents.  That in turn will trigger an update which in turn will
  ** Abort the current update prior to starting the new one.
  */
  if(def->is1d() != specis1d(def->spectrum()) )  {
     pdb->refresh_state(ctx->row, ctx->column,
			rfsh_idle);
     goto display_done;		/* Finish the display off. */
     Xamine_DisplayPane(ctx->row, ctx->column, def->spectrum());
  }

  /* If control passes here, then we need to do the redraw.  This is divided
  ** into phases which are defined< by the refreshstate.
  ** we fan out depending on the current state and advance as needed
  ** to the next phase.
  */

  /*  If the state was queued, then transition transparently into beginngin */
  /*  We also create/defined a pixmap (if possible)
  **  into which all the drawing will be done
  */

  if(pdb->refresh_state(ctx->row, ctx->column) == rfsh_queued) {
    Xamine_DeleteBackingStore(ctx->row,		/* Destroy old */
			      ctx->column);	/* backing store */
    pdb->refresh_state(ctx->row, ctx->column, rfsh_beginning);
    def->markupdate();
    ctx->pixmap =  CreatePixmap(display, window,
  			        wattrib.width, wattrib.height, 
				wattrib.depth);
    if(ctx->pixmap == (Drawable)NULL) {
       ctx->pixmap = window;	/* If no pixmap could be created draw direct */
    }				/* to window instead.                        */
    else {
	Xamine_SetBackingStore(ctx->row, ctx->column, ctx->pixmap);
    }
    ctx->pixwid = wattrib.width;
    ctx->pixht  = wattrib.height;
    ClearWindow(ctx);
  }

  window = ctx->pixmap;	/* For compatibility. */

  switch(pdb->refresh_state(ctx->row, ctx->column)) {
  case rfsh_idle:		/* Return finally, don't alter state. */
    goto display_done;
  case rfsh_beginning:		/* Clear window, advance to titles. */
    pdb->refresh_state(ctx->row, ctx->column, rfsh_titles);
    /* If necessary set the scaling: */
    {
      if(!def->manuallyscaled())
	def->setfsval(Xamine_ComputeScaling(def, ctx->pane));
    }
    /*
    **  Ensure that if there's a floor, the full scale value never goes
    **  Below it:
    */

    floor = def->hasfloor() ? def->getfloor() : 0;
    if(def->getfsval() <= floor) def->setfsval(floor+1);

    /* If necessary, force a color display to the default monochrome rendition
    */
    {
      if(!def->is1d()) {
	win_2d *d2 = (win_2d *)def;
	if( (d2->getrend() == color) &&
	   !(Xamine_ColorDisplay())) {
	  win_2d defaults;
	  Xamine_Construct2dDefaultProperties(&defaults);
	  d2->setrend(defaults.getrend());
	}
      }
    }
    return False;
  case rfsh_titles:		/* Draw appropriate titles, advance to axes */
    Xamine_DrawTitles(ctx, pdb->getdef(ctx->column, ctx->row));
    pdb->refresh_state(ctx->row, ctx->column, rfsh_contents);
    return False;
  case rfsh_axes:		/* Draw appropriate axes, advance to contents */
    if(pdb->getdef(ctx->column, ctx->row)->showaxes()) {
      Xamine_DrawAxes(ctx, pdb->getdef(ctx->column, ctx->row));
    }
    pdb->refresh_state(ctx->row, ctx->column, rfsh_callback);
    return False;
  case rfsh_contents:		/* Draw spectrum contents. advance to grobs */
    if(Xamine_DrawChannels(ctx, pdb->getdef(ctx->column, ctx->row)))
      pdb->refresh_state(ctx->row, ctx->column, rfsh_grobjs);
    return False;
  case rfsh_grobjs:		/* Graphic objects, advance to grobj names */
    pdb->refresh_state(ctx->row, ctx->column, rfsh_axes);
    Xamine_DrawObjects(ctx, pdb->getdef(ctx->column, ctx->row));
    return False;
  case rfsh_aborting:		/* Clear the window then done. */
    ClearWindow(ctx);
    pdb->refresh_state(ctx->row, ctx->column, rfsh_idle);
    goto display_done;
  case rfsh_callback:
    if(refresh_callback) {
      (*refresh_callback)(ctx, refresh_user_data);
    }
    pdb->refresh_state(ctx->row, ctx->column, rfsh_idle);
    return False;
  }

  /* The code below is performed when we are done displaying */
  /* we clean up our context temp storage and return.  We rely on */
  /* functions that use the special added context pointer to Null on */
  /* deallocation... this is necessary to properly handle the abort case */

 display_done:
  /*
  ** The pixmap is now a correct representation of the window.
  ** we copy the pixmap to the server, and read it into the image before
  ** freeing it.
  */

  /* Delete the chained context first so that any destructors called by the
  ** context destroyer will have a chance to flush their graphics.
  */
  GC gc;
  gc = Xamine_MakeDrawingGc(display, ctx->pane, -1);

  if(ctx->ctx != NULL) {	/* If there's a linked segment context, */
    if(ctx->destroyer != NULL) { /* If a destroyer is defined then... */
      ctx->destroyer((XtPointer)ctx->ctx); /* Call the destroyer. */
    }
    else {			/* If there isn't a destroyer then */
      delete ctx->ctx;		/* Default action is to just delete it. */
    }
  }
  ctx->ctx = NULL;		/* Null out context in case not done. */

  /* If the window attributes changed, then re-start the current draw: */

  if( ((wattrib.height != ctx->pixht) || (wattrib.width != ctx->pixwid))  &&
      (ctx->pixmap != (Drawable)NULL)) {
    if(ctx->pixmap != XtWindow(ctx->pane->getid())) {
      Xamine_DeleteBackingStore(ctx->row, ctx->column);
    }
    ctx->pixmap = (Drawable) NULL;
    pdb->refresh_state(ctx->row, ctx->column, rfsh_queued);
    return False;
  }

  /* Draw the pixmap to the window: */

  if(ctx->pixmap != (Drawable)NULL) {	/* Could be null on an abort */
    XWindowAttributes att;


    if( ctx->pixmap != XtWindow(ctx->pane->getid())) {
      XGetWindowAttributes(display, XtWindow(ctx->pane->getid()), &att);
      XCopyArea(display, 
		Xamine_GetBackingStore(ctx->row, ctx->column), 
		XtWindow(ctx->pane->getid()), gc,
		0,0, att.width, att.height, 0,0);
    }
  }
  else {			/* Destroy any hanging backing store. */
    if(Xamine_GetBackingStore(ctx->row, ctx->column) != 0) {
      Xamine_DeleteBackingStore(ctx->row, ctx->column);
    }
  }
  ctx->pixmap = (Drawable)NULL;
  q->Remove();
  return q->IsEmpty();
}

/*
** Functional Description:
**   Xamine_RedrawSpectrum:
**     This function redraws any panes which are displaying the given
**     spectrum.
** Formal Parameters:
**    int specid:
**      Number of the spectrum.
*/
void
Xamine_RedrawSpectrum(int specid)
{
  int nr,nc;

  nr = Xamine_Panerows();
  nc = Xamine_Panecols();

  for(int r = 0; r < nr; r++)
    for(int c = 0; c < nc; c++) {
      win_attributed *at = Xamine_GetDisplayAttributes(r,c);
      if(at != NULL) {
        if(at->spectrum() == specid)
	  Xamine_RedrawPane(c, r);
      }
    }
}


