/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   panemgr.cc  - This file contains two sections of code.  The first
**                 implements the non-inline methods for the pane_db.
**                 The second set, closely related to this, is a set of
**                 functions which manipulate a single instance of a 
**                 pane database.  This module is structured in this
**                 way to allow future versions to support secondary pane
**                 configurations either for a pane cache or for multiple
**                 visible windows.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files required:
*/
#include <X11/cursorfont.h>
#include <Xm/DrawingA.h>
#include <stdio.h>

#include "XMManagers.h"
#include "XMWidget.h"
#include "menusetup.h"
#include "panemgr.h"
#include "buttonsetup.h"
#include "pkgmgr.h"
#include "dispshare.h"
#include "refreshctl.h"
#include "trackcursor.h"
#include "colormgr.h"
#include "superpos.h"
/*
** Defines:
*/
#define TF(log) ((log) ? "True" : "False")

#define DBLCLICK_TIME 500			/* What is a double click */
/*
**   Module wide visible storage:
*/
static pane_db *Xamine_panedb;		/* The pane database. */

static void (*Pane_callback)(XMWidget *, /* Callback function for */
			     XtPointer, /* pane if input */
			     XtPointer) = NULL;
static void (*Select_callback)(int, int,   /* Selection changed callback */
			       int, int,   /* Passes old, new pane, client. */
			       XtPointer) = NULL;

static XtPointer pane_client_data = NULL; /* User data passed to pane_callback */
static XtPointer select_client_data = NULL; /* User data for Select_Callback */

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      TimeDifference - Determines how many ms. elapsed between two
**	event timestamps.   
**
**  FORMAL PARAMETERS:
**
**      Time t1,t2:
**	  The two times, t1 is assumed to be chronologically later than t2.
**
**  RETURN VALUE:
**
**      Time difference between t1,t2.
**
**--
*/
Time TimeDifference(Time t1, Time t2)
{
    unsigned long allbits = (unsigned long)-1l;
    if (t1 > t2	)
    {
    	return t1-t2;
    }
    else	    /* T1 wrapped.  */
    {
    	return t2  + 1 + (allbits - t1);
    }
}

/*
** Functional Description:
**   SetWindowLabel:
**     This function modifies the label of a window given any widget which is
**     a descendant of that shell.
** Formal Parameters:
**  Widget w:
**   Descendent widget id.
** char *string:
**  New window title
*/
void SetWindowLabel(Widget w, char *s)
{
  /* First hunt for the application shell.  The widget could be a dialog
  ** widget.  In that case it might have a vendor shell between it and
  ** the application anscestors. 
  */
  while(!XtIsApplicationShell(w)) w = XtParent(w);
  /*
  ** The application shell's nearest vendor shell anscestor is
  ** going to be the one with the application's title string
  ** in it's window banner.
  */
  while(!XtIsVendorShell(w)) w = XtParent(w);

  XtVaSetValues(w, 
		XmNtitle, s,
		NULL);
  
}


/*
** The code which follows fleshes out the methods for the pane_db class
** The pane_db class is a specialized application class which 
** keeps the database of panes information and widgets associated with them
*/

/*
** Method description:
**  init_panedb        - Pane database constructor.  At this point the 
**                   window database has been constructed and initialized.
**                   All we need to do is set up the widgets and the window
**                   banner.
** Formal Parameters:
**     XMForm *parent:
**       Parent widget for the widget sub-hierarchy that we'll be building...
**       Must be a form widget.
**     win_title title:
**       The title of the window/pane-database.  The title of the
**       top level shell widget will be set from this.
*/
void pane_db::init_panedb(XMForm *parent, win_title title)
{
  char widget_name[80];
  

  /* Make the Manager widget: */

  manager_widget = parent;
  manager_widget->SetAttribute(XmNrubberPositioning, True);
  XtVaSetValues(XtParent(manager_widget->getid()),
		XmNwidth, (Dimension)XAMINE_PANEMGR_XPIXELS,
		XmNheight, (Dimension)XAMINE_PANEMGR_YPIXELS,
		NULL);
  
  int k = 0;
  Arg pane_args[2];
  Arg frame_args[4];

  XtSetArg(frame_args[0], XmNtopAttachment,   XmATTACH_NONE);
  XtSetArg(frame_args[1], XmNbottomAttachment,XmATTACH_NONE);
  XtSetArg(frame_args[2], XmNleftAttachment, XmATTACH_NONE);
  XtSetArg(frame_args[3], XmNrightAttachment, XmATTACH_NONE);

  for(int j = 0; j < WINDOW_MAXAXIS; j++)
    for(int i = 0; i < WINDOW_MAXAXIS; i++) {
      XtSetArg(pane_args[0], XmNuserData, k);
      sprintf(widget_name,"Frame_%d_%d",i,j);
      frames[i][j] = new XMFrame(widget_name, *manager_widget,
				 frame_args, 4);
      sprintf(widget_name,"Pane_%d_%d", i,j);
      panes[i][j]  = new XMWidget(widget_name, xmDrawingAreaWidgetClass,
				*(frames[i][j]), pane_args, 1);
      k++;
      update_state[i][j]  =  rfsh_idle;
      update_tid[i][j]    =  (XtIntervalId)NULL;
      backing_store[i][j] = (Drawable) NULL;
    }
  selected_row = 0;
  selected_column = 0;		/* Initialize to legitimate values. */

  XtSetArg(frame_args[0], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(frame_args[1], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(frame_args[2], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(frame_args[3], XmNbottomAttachment, XmATTACH_FORM);

  XtSetValues(panes[0][0]->getid(), frame_args, 4);

  panes[0][0]->Manage();
  frames[0][0]->Manage();
  manager_widget->Manage();
  XtManageChild(XtParent(manager_widget->getid()));

  settitle(title);		/* Set the database/window titles. */
}

/*
** Method Description:
**   ~pane_db:     Destructor for the pane database.  We must unmanage
**                 and free all of the widgets.
*/
pane_db::~pane_db()
{
  manager_widget->UnManage();		/* Make everything invisible. */

  /* Kill off the panes and their frames */

  for(int j = 0; j < WINDOW_MAXAXIS; j++)
    for(int i = 0; i < WINDOW_MAXAXIS; i++) {
       delete panes[i][j]; panes[i][j] = NULL;
       delete frames[i][j]; frames[i][j] = NULL;
    }
  /* Kill off the row/column manager */

  delete manager_widget;
}

/*
** Method Description:
**   setgeometry:    This method sets a new pane geometry.  This is typically
**                   the result of a Set Window Geometry function.
**                   We must unmanage all the widgets in the old geometry,
**                   unmanage the manager.  Then we need to recalculate and
**                   set the drawing widget sizes appropriately, Finally,
**                   we select the 1,1 widget and manage the widgets:
** Formal Parameters:
**    int nrow:
**      Number of rows.
**    int ncol:
**      Number of columns.
*/
void pane_db::setgeometry(int nrow, int ncol) 
{
  int i,j;
  Dimension height, width;
  Widget f;
  Widget p;
  Widget shell;

  /* Some window managers will go and change some shell sizes for the
  ** hell of it when we do what we're about to do.  Therefore, we will
  ** Save the shell dimensions (enforcing minima) and reset the shell
  ** dimensions at the end of this method:
  */

  shell = manager_widget->getid();
  while(!XtIsVendorShell(shell)) shell = XtParent(shell);
  Window shell_win = XtWindow(shell);
  Display *disp    = XtDisplay(shell);
  XWindowAttributes shell_attribs;

  if(XGetWindowAttributes(disp, shell_win, &shell_attribs) == 0) {
    fprintf(stderr, "Could not get shell window attributes\n");
    exit(-1);
  }

  /* Now we procede with the business of setting the attributes of the
  ** widgets in the shell
  */

  manager_widget->GetAttribute(XmNwidth, &width);
  manager_widget->GetAttribute(XmNheight, &height);

  manager_widget->UnManage();

  /* Remove any excess and frames from the display by unmanaging them: */
  /* Also undefine the cursor so that we go back to default.           */

  for(j = 0; j < WINDOW_MAXAXIS; j++) {
    for(i = 0; i < WINDOW_MAXAXIS; i++) {
      Display *display = XtDisplay(panes[i][j]->getid());
      Window  w        = XtWindow(panes[i][j]->getid());
      XUndefineCursor(display, w);
      f = frames[i][j]->getid();
      frames[i][j]->UnManage();
      frames[i][j]->SetShadowType(XmSHADOW_OUT);
      if(backing_store[i][j] != (Drawable)NULL) 
	DeleteBackingStore(j,i);

     
    }
  }
  
  /* Next set the geometry of the underlying ancestor type: */

  cleardb();			/* Clear the database entries. */
  win_geometry::setgeometry(ncol, nrow); /* Reset the configuration. */
  

  /* Setup the window geometry.						    */

  height = (height / nrow) - 1;
  width  = (width  / ncol) - 1;
  int row = 0;
  int col = 0;
  
  manager_widget->SetFractionBase(nrow*ncol);
  for(j = 0; j < nrow; j++) {
     for(i = 0; i < ncol; i++) {
	f = frames[i][j]->getid();
	p = panes[i][j]->getid();
        XtVaSetValues(f,
	        XmNtopAttachment, XmATTACH_POSITION,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNtopPosition,      row,
		XmNleftPosition,     col,
		XmNrightPosition,    col+nrow,
		XmNbottomPosition,   row+ncol,
		NULL);
        col += nrow;
	panes[i][j]->Manage();
	frames[i][j]->Manage();
     }
     col = 0;
     row += ncol;
  }

  /* Select the first frame */

  select(0,0);			/* a select on 0,0 */
  manager_widget->Manage();

  /* Force the shell to the prior geometry: */

  XtVaSetValues(shell,
		XmNwidth, (Dimension)shell_attribs.width,
		XmNheight, (Dimension)shell_attribs.height,
		NULL);

}

/*
** Method Description:
**   select   - This method selects a pane.  This involves updating
**              the selected_row and selected_column fields and
**              altering the visual appearance of the associated frame.
** Formal Parameters:
**   int row:
**     New seletion row.
**   int col:
**     New selection column.
*/
void pane_db::select(int row, int column)
{
  if(frames[selected_column][selected_row] != NULL) {
     frames[selected_column][selected_row]->SetShadowType(XmSHADOW_OUT);
  }
  selected_row = row;
  selected_column = column;
  frames[column][row]->SetShadowType(XmSHADOW_IN);
  XmProcessTraversal(panes[selected_column][selected_row]->getid(), 
		     XmTRAVERSE_CURRENT);

}

/*
** Method Description:
**   settitle   - Set the title of the database.  This also sets the
**                title on the banner of the window at the top of
**                the pane hierarchy.
** Formal Parameters:
**    char *title:
**      Title for the window/database.
*/
void pane_db::settitle(char *title)
{
   win_db::settitle(title);
   SetWindowLabel(manager_widget->getid(), title);      
}

/*
** Method Description:
**    zoom    - This method zooms the display to the currently selected
**              spectrum pane.  This involves first zooming the underlying
**              window data base object then reconfiguring the row column
**              parent widget so that only the zoomed pane is visible.
*/
void pane_db::zoom()
{
  Widget f,p;
  Widget shell;

  shell = manager_widget->getid();
  while(!XtIsVendorShell(shell)) shell = XtParent(shell);
  Window shell_win = XtWindow(shell);
  Display *disp    = XtDisplay(shell);
  XWindowAttributes  shell_attribs;
  if(XGetWindowAttributes(disp, shell_win, &shell_attribs) == 0) {
    fprintf(stderr, "Could not get shell window attributes\n");
    exit(-1);
  }


  win_db::zoom(selected_column, selected_row);
   
   /* Un manage the row column widget for the duration of the fiddling */


  manager_widget->UnManage();
//  for(int j = 0; j < WINDOW_MAXAXIS; j++) {
//    for(int i = 0; i < WINDOW_MAXAXIS; i++) {
//	frames[i][j]->UnManage();
//    }
//  }
  /* Set the selected pane to sit on the entire form: */

  f=frames[selected_column][selected_row]->getid();
  p=panes[selected_column][selected_column]->getid();
  XRaiseWindow(XtDisplay(p), XtWindow(p));
  XRaiseWindow(XtDisplay(f), XtWindow(f));
  XtVaSetValues(f,
	        XmNtopAttachment, XmATTACH_POSITION,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNbottomAttachment, XmATTACH_POSITION,
		XmNtopPosition,      0,
		XmNleftPosition,     0,
		XmNbottomPosition,           nx()*ny(),
		XmNrightPosition,            nx()*ny(),
		NULL);

  frames[selected_column][selected_row]->SetShadowType(XmSHADOW_IN);

  /* Now re-manage the manager widget */

  manager_widget->Manage();
  panes[selected_column][selected_row]->Manage();

  frames[selected_column][selected_row]->Manage();

  /* Set the size of the shell correctly: */

  XtVaSetValues(shell,
		XmNwidth, (Dimension)shell_attribs.width,
		XmNheight, (Dimension)shell_attribs.height,
		NULL);
}

/*
** Method Description:
**   unzoom   - Unzooms a pane restoring the picture to that of the 
**              complete set of panes described by the most recent geometry
**              setting.
*/
void pane_db::unzoom()
{
   Widget wf;
   Dimension width, height;
   int row, column;
   Widget shell;
   
   XMFrame  *f = frames[selected_column][selected_row];
   
   win_db::unzoom();		/* Un zoom the database too. */
   
   /* Get the current shell geometry so it can be restored on balky suns */

   shell = manager_widget->getid();
   while(!XtIsVendorShell(shell)) shell = XtParent(shell);
   Window   shell_win = XtWindow(shell);
   Display *display   = XtDisplay(shell);
   XWindowAttributes shell_attribs;
   if(XGetWindowAttributes(display, shell_win, &shell_attribs) == 0) {
     fprintf(stderr, "Could not get shell window attributes\n");
     exit(-1);
   }

   /* Put the widget back into the boxes where it belongs */
   



   f->UnManage();
   manager_widget->UnManage();


  height = height / ny() - 1;
  width  = width  / nx() - 1;

  row = column =  0;
  for(int j = 0; j < ny(); j++) {
    for(int i = 0; i < nx(); i++) {
	wf = frames[i][j]->getid();
	XtVaSetValues(wf,
		      XmNtopAttachment, XmATTACH_POSITION,
		      XmNtopPosition,   row,
		      XmNleftAttachment, XmATTACH_POSITION,
		      XmNleftPosition,  column,
		      XmNbottomAttachment, XmATTACH_POSITION,
		      XmNrightAttachment, XmATTACH_POSITION,
		      XmNbottomPosition, row+nx(),
		      XmNrightPosition,  column+ny(),
		      NULL);
        column += ny();
	panes[i][j]->Manage();
        frames[i][j]->Manage();
     }
     column = 0;
     row += nx();
  }
	
  manager_widget->Manage();
 
  XtVaSetValues(shell, 
		XmNwidth,   (Dimension)shell_attribs.width,
		XmNheight,  (Dimension)shell_attribs.height,
		NULL);
}

/*
** Manage the backing store for a pane:
*/
/*
** Functional Description:
**   panedb::DeleteBackingStore:
**     Delete the backing store associated with a pane.
** Formal Parameters:
**    int row, col:
**      Pane coordinates.
*/
static void FreePixmap(Display *d, Pixmap p)
{
  XFreePixmap(d,p);
}

void pane_db::DeleteBackingStore(int row, int col)
{
  if(backing_store[col][row] != (Drawable)NULL) {
    FreePixmap(XtDisplay(manager_widget->getid()), backing_store[col][row]);
    backing_store[col][row] = (Drawable)NULL;
  }

}

/*
** The code which follows is a database manager for a single database of
<** panes.  For the intial implementation of Xamine, this should be sufficient.
** For later enhancements which might deal with multple pane databases,
** a more complex database manager might be required
*/

/*
** Functional Description:
**   Xamine_PaneInputCallback:
**     This function is called whenever there is input on a pane.  If there
**     is a mouse hit on the unselected pane, then it becomes selected.
**     If there is any input event on the selected spectrum and there is a
**     non null Pane_callback function, then it is called.
** Formal Parameters:
**   XMWidget *pane:
**    Pointer to the pane widget.
**   XtPointer client_data:
**    The client data (currently null).
**   XtPointer callback_data:
**     Actually a pointer to an XmDrawingAreaCallbackStruct which describes the
**     event that triggered the callback.
*/
static Time last_click = 0;		/* Timestamp of prior click */

void Xamine_PaneInputCallback(XMWidget *pane, XtPointer client_data, 
			      XtPointer callback_data) 
{
  XmDrawingAreaCallbackStruct *event_info = 
    (XmDrawingAreaCallbackStruct *)callback_data;
  XButtonPressedEvent *event = (XButtonPressedEvent *)event_info->event;

  /* If this is the current input and there's a defined pane callback, then */
  /* call it and return.                                                    */

  int row, col, index;

  XmProcessTraversal(pane->getid(), XmTRAVERSE_CURRENT);
  pane->GetAttribute(XmNuserData, &index); /* Get the pane index. */


  row = index / WINDOW_MAXAXIS;
  col = index % WINDOW_MAXAXIS;
  
    
  if( (col == Xamine_panedb->current_col()) &&
      (row == Xamine_panedb->current_row())) {
    if(Pane_callback) {
      Pane_callback(pane, pane_client_data, callback_data);
    }
    else {				/* Possible double click zoom: */
	if(event_info->reason != XmCR_INPUT) return; /* Mouse hits are input events. */
	if(event_info->event->type != ButtonRelease) return; /* key on the upstroke */
	/*
	** If the pane was double clicked, then the zoom is toggled. 
	*/
	Time clicktime = event->time;
        if(TimeDifference(clicktime, last_click) < DBLCLICK_TIME) {
	       Xamine_ToggleZoom(pane, client_data, callback_data);
	}
        last_click = clicktime;

    }
    return;
  }

  /* Otherwise, this is treated as an attempt to set a new selection, 
  ** but only if the event is an input event associated with a mouse click:
  */

  if(event_info->reason != XmCR_INPUT) return; /* Mouse hits are input events. */
  if(event_info->event->type != ButtonRelease) return; /* key on the upstroke */

  Xamine_SelectPane(row, col);			/* Change selection... */
  last_click = event->time;
  

}

/*									    */
/* Functional Description:						    */
/*    SetupInitialGeometry:						    */
/*      One shot callback function called when the manager widget is	    */
/*	mapped. We set the initial geometry to 1,1 and remove ourselves	    */
/*	from the callback list.						    */
/* Formal Parameters:							    */
/*    XtPointer ud:							    */
/*      Unused user data.						    */
/*    XtIntervalid id:							    */
/*       Unused interval id.						    */
/*									    */
void SetupInitialGeometry(XtPointer ud, XtIntervalId *id) 
{
   
   Xamine_SetPaneGeometry(1,1);
   Xamine_SetPaneGeometry(1,1);

}
void SetInitialSize(XtPointer ud, XtIntervalId *id)
{
  XMWidget *wid = (XMWidget *)ud;
  Xamine_ResetShellSize(wid);
}

/* 
** Functional Description:
**   Xamine_ResetShellSize:
**     This function resets the shell size to it's initial value.
** Formal Parameters:
**    XMWidget *wid:
**      Some widget in the shell's hierarchy.
*/
void Xamine_ResetShellSize(XMWidget *wid)
{
  Widget w = wid->getid();
  while(!XtIsVendorShell(w)) w = XtParent(w); /* Find the shell. */

  XtVaSetValues(w,
		XmNwidth, XAMINE_PANES_XPIXELS,
		XmNheight, XAMINE_PANES_YPIXELS,
		NULL);
}

/*
** Functional Description:
**   Xamine_Initialize_panedb:
**     This function instantiates and initializes the pane database.
**     An instance of a pane_db is created.  The geometry is set to 1,1
**     and pane 0,0 is selected.
** Formal Parameters:
**    XMWidget *parent:
**      The parent of the widget hierarchy.
** NOTE:
**    The database name is set to "Xamine -- Unnamed"
*/
void Xamine_Initialize_panedb(XMForm *parent)
{
  Widget w;


  /* Set the minimum size of the shell: */

  w = parent->getid();
  while(!XtIsVendorShell(w)) w = XtParent(w);
  XtVaSetValues(w,
		XmNwidth, XAMINE_PANES_XPIXELS,
		XmNheight, XAMINE_PANES_YPIXELS,
		XmNminWidth, XAMINE_PANES_MINXPIXELS,
		XmNminHeight, XAMINE_PANES_MINYPIXELS,
		NULL);

    /* Instantiate and initialize the pane database */

  Xamine_panedb = new pane_db(parent, "Xamine -- Unnamed");

  /*  Set Callbacks on all the newly created drawing area widgets to */
  /* point to Xamine_PaneInputCallback which contains common code    */
  /* to select widgets.                                              */

  for(int j = 0; j < WINDOW_MAXAXIS ; j++) {
     for(int i = 0; i < WINDOW_MAXAXIS; i++) {
	XMWidget *p;
	p = Xamine_panedb->pane(j,i);
	p->AddCallback(XmNinputCallback, Xamine_PaneInputCallback);
	/*
	**  Set up the callbacks to draw the contents of the pane
        */
	p->AddCallback(XmNresizeCallback, Xamine_PaneRedrawCallback);
	p->AddCallback(XmNexposeCallback, Xamine_PaneRedrawCallback);
     }
  }
  /* Put a timer callback on to set the geometry */

  XMWidget *mgr = Xamine_panedb->manager();
  Widget   mgr_w = mgr->getid();
  
  XtAppAddTimeOut(XtWidgetToApplicationContext(mgr_w),
		  1000, SetupInitialGeometry, NULL);
  XtAppAddTimeOut(XtWidgetToApplicationContext(mgr_w),
		  1100, SetInitialSize, (XtPointer)mgr);

  /*
  ** Set the state of the menus to allow just the base package: 
  */

  Xamine_EnableBasePackage();
  Xamine_DisableMultiwindowPackage();
  Xamine_DisableSpectrumPresentPackage();
  Xamine_DisableSpectrumSelectedPackage();
  Xamine_Disable1dSelectedPackage();
  Xamine_Disable2dSelectedPackage();
  
}

/*
** Functional Description:
**   Xamine_SetPaneGeometry:
**     This function sets the pane geometry for the pane database
**  NOTE:
**    The resize events and expose events should take care of redraws.    
** Formal Parameters:
**   int ncol, nrow:
**     Number of rows and columns.
*/
void Xamine_SetPaneGeometry(int ncol, int nrow)
{
  Xamine_CancelUpdates();
  Display *display = XtDisplay(Xamine_panedb->pane(0,0)->getid());
  XFlush(display);		// Flush any pending output on resources we'll
                                // be destroying. 
  /* For all defined panes in the old geometry, remove the pointer motion
  ** event handler:
  */
  if(Xamine_panedb->StatusBar()) {
    for(int j = 0; j < Xamine_panedb->ny(); j++)
      for(int i = 0; i < Xamine_panedb->nx(); i++) {
	if(Xamine_panedb->defined(i,j)) {
	  XMWidget *pane = Xamine_panedb->pane(i,j);
	  XtRemoveEventHandler(pane->getid(), 
			       PointerMotionMask | PointerMotionHintMask,
			       False, Xamine_PointerMotionCallback, pane);
	}
      }
  }

  /* Set the new geometry. */

  Xamine_panedb->setgeometry(nrow, ncol);


  /* Turn off the zoom indicator */
  
  Xamine_SetZoomToggleButtonState(False);
  Xamine_SetButtonBarZoom(False);

  /* Reset the function ghosting to match the current state: */

  Xamine_EnableBasePackage();
  if( ncol*nrow > 1) Xamine_EnableMultiwindowPackage();	/* More than 1 window */
  else               Xamine_DisableMultiwindowPackage(); /* only 1 window */
  Xamine_DisableSpectrumPresentPackage();
  Xamine_DisableSpectrumSelectedPackage();
  Xamine_Disable1dSelectedPackage();
  Xamine_Disable2dSelectedPackage();
  Xamine_DisableUnsuperimpose();	// Empty spec has no superpositions.

  /* If there's a selection changed callback, then we call it with prior */
  /* row/column of -1,-1 to indicate that there is no longer an existing */
  /* prior row/column window and 0,0 as the new row, col */

  if(Select_callback)
    Select_callback(-1,-1, 0,0, select_client_data);
  XFlush(display);		// Flush any pending output on resources we'll
                                // be destroying.
}

/*
** Functional Description:
**    Xamine_Getpanemgr   - Returns the manager widget of the current set
**                          of panes.
*/
XMWidget *Xamine_Getpanemgr()
{
  return Xamine_panedb->manager();
}

/*
** Functional Descriptions:
**    Xamine_Panerows    - Returns the number of rows in the current
**                         pane configuration.
**    Xamine_Panecols    - Returns the number of columns in the current
**                         pane configuration.
*/
int Xamine_Panerows()
{
  return Xamine_panedb->ny();
}

int Xamine_Panecols()
{
  return Xamine_panedb->nx();
}


/* 
** Functional Descriptions:
**   Xamine_PaneSelectedrow  - Returns the row number of the selected pane
**   Xamine_PaneSelectedcol  - Returns the column number of the selected
**                             pane.
*/

int Xamine_PaneSelectedrow()
{
  return Xamine_panedb->current_row();
}

int Xamine_PaneSelectedcol()
{
  return Xamine_panedb->current_col();
}

/*
** Functional Description:
**    Xamine_SelectPane    - Set which pane is the currently selected
**                           pane.
** Formal Parameters:
**    int row:
**      The row coordinate of the pane to select.
**    int col:
**      The column coordinate of the pane to select.
*/
void Xamine_SelectPane(int row, int col)
{
  int oldr, oldc;

  /* Save the old row and column in case there's a callback */

  oldr = Xamine_panedb->current_row();
  oldc = Xamine_panedb->current_col();

  /* Change the selection: */

  Xamine_panedb->select(row, col);

  /* Now Alter the state of the function ghosting depending on what's in */
  /* the selected pane.                                                  */
  if(Xamine_panedb->defined(col, row) && 
     (Xamine_panedb->getdef(col,row) != NULL)) { 
    Xamine_EnableSpectrumSelectedPackage(); /* The pane has a spectrum and... */
    if(Xamine_panedb->getdef(col,row)->is1d()) {
      Xamine_Enable1dSelectedPackage();	    /* It's a 1-d. */
      Xamine_Disable2dSelectedPackage();
      //
      // If there are superpositions, then unsupserimpose is enabled.
      // otherwise, disabled.
      win_1d *att  = (win_1d *)Xamine_panedb->getdef(col,row);
      if(att->GetSuperpositions().Count() == 0) {
         Xamine_DisableUnsuperimpose();
      }
      else {
         Xamine_EnableUnsuperimpose();
      }
    }
    else {
      Xamine_DisableUnsuperimpose();
      Xamine_Disable1dSelectedPackage();    /* It's a 2-d. */
      Xamine_Enable2dSelectedPackage();
    }
  }
  else {
    Xamine_DisableSpectrumSelectedPackage(); /* The pane has no spectrum. */
    Xamine_Disable1dSelectedPackage();
    Xamine_Disable2dSelectedPackage();
  }

  /* Set the state of the log toggle button: */

  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att != (win_attributed *)NULL) 
    Xamine_SetButtonBarLog(att->islog());

  /* If there's a selection change callback, then call it: */

  if(Select_callback)
    Select_callback(oldc, oldr, col, row, select_client_data);

}

/*
** Functional Descriptions:
**   Xamine_GetDrawingArea   - Returns the widget handle to the
**                             desired drawing area widget.
**   Xamine_GetSelectedDrawingArea
**                           - Returns the widget handle to the drawing
**                             area widget which is currently selected.
** Formal Parameters:
**    int row:
**      The row number of the desired widget.
**    int col:
**      The column number of the desired widget.
** Returns:
**   XMWidget * Pointer to the desired drawing area widget.
*/
XMWidget *Xamine_GetDrawingArea(int row, int col)
{
  return Xamine_panedb->pane(row, col);
}

XMWidget *Xamine_GetSelectedDrawingArea()
{
  return Xamine_panedb->pane(Xamine_panedb->current_row(),
			     Xamine_panedb->current_col());
}

/*
** Functional Description:
**    Xamine_GetFrame         - Returns the widget handle of an arbitrary frame.
**    Xamine_GetSelectedFrame - Returns the widget handle of the selected
**                              frame.
** Formal Parameters:
**    int row:
**      Row of the desired widget.
**    int col:
**      Column of the desired widget.
** Returns:
**   A pointer to the widget object.
*/
XMFrame *Xamine_GetFrame(int row, int col)
{
  return Xamine_panedb->frame(row, col);
}
XMFrame *Xamine_GetSelectedFrame()
{
  return Xamine_panedb->frame(Xamine_panedb->current_row(),
			      Xamine_panedb->current_col());
}

/*
** Functional Description:
**   Xamine_ToggleZoom   - This callback function processes the zoom
**                         toggle button state changes.
**  NOTE:
**    the resize request should take care of ensuring the redraw.
** Formal Parameters:
**   XMWidget *toggle:
**     Widget Pointer of the toggle button.
**   XtPointer cd:
**     Unused call data.
**   XtPointer cbd:
**     Callback data used to determine the state of the widget.
*/
void Xamine_ToggleZoom(XMWidget *toggle, XtPointer cd, XtPointer cbd)
{
  if(!Xamine_panedb->iszoomed()) {		/* Swap states. */ 
    
    Xamine_panedb->zoom();
    Xamine_DisableMultiwindowPackage();	/* Zoom treated like single window */
  }
  else {
    Xamine_panedb->unzoom();
    if((Xamine_panedb->nx() * Xamine_panedb->ny()) > 1) 
      Xamine_EnableMultiwindowPackage(); /* If multiple windows enable fcns. */
  }
  Xamine_SetButtonBarZoom(Xamine_panedb->iszoomed());
  Xamine_SetZoomToggleButtonState(Xamine_panedb->iszoomed());
}
/*
** Functional Description:
**   Xamine_ZoomNext:
**      This function does the following:
**         If the pane state is not zoomed, the selected pane is zoomed.
**         If the pane state is zoomed, then the next pane to the right is
**         zoomed into.  The next pane to the right wraps down to the next row
**         and, if necessary up to the first row too.
** Formal Parameters:
**    XMWidget *button:
**        Putton pressed to cause the zoom next.
**    XtPointer cd:
**        Button callback data.
**    XtPointer ud:
**        Pointer to any user data associated with the callback
*/
void Xamine_ZoomNext(XMWidget *button, XtPointer cd, XtPointer ud)
{
  if(!Xamine_panedb->iszoomed()) {
    Xamine_ToggleZoom(button, cd, ud); // just turn on zoom if off.
  }
  else {
    int sc, sr;
    sc = Xamine_panedb->current_col();
    sr = Xamine_panedb->current_row();
    sc++;
    if(sc >= Xamine_Panecols()) {
      sc = 0;
      sr++;
      if(sr >= Xamine_Panerows()) sr = 0;
    }
    Xamine_SelectPane(sr, sc);	// Select the new pane.. do callbacks etc.
    Xamine_panedb->zoom();	// Zoom in on the new selected columns.
  }
}

/*
** Functional Description:
**   Xamine_ClearPaneInputCallback:
**     This function removes callback functions which are set on a pane.
**     The use of a selected pane callback function allows the rest of the
**     code to transparently hook into mouse hit and keystroke callbacks which
**     occur on the selected spectrum without knowing about selected spectrum
**     changes or the selection process.
*/
void Xamine_ClearPaneInputCallback()
{
  Pane_callback = NULL;
  pane_client_data = NULL;

}

/*
** Functional Description:
**   Xamine_SetPaneInputCallback:
**     This function allows the user to set a callback function on the 
**     *SELECTED* pane only... even if the selected pane moves around 
**     due to mouse hits or spectrum placement in non selected panes.
** Formal Parameters:
**   void (*callback)(XMWidget *, XtPointer, XtPointer):
**      Pointer to the callback function.
**   XtPointer client_data:
**      Data which will be passed without interpretation to the callback on
**      activation.
*/
void Xamine_SetPaneInputCallback(void (*callback)(XMWidget *, 
						  XtPointer, XtPointer),
				 XtPointer client_data)
{
  Pane_callback    = callback;
  pane_client_data = client_data;
}
/*
** Functional Description:
**   Xamine_GetPaneInputCallback:
**     This function returns the current values of the pane input callback
**      variables.  This allows a caller to determine if there is currently
**      a pane input callback as if there is, then the callback handle will
**      be non zero.
** Formal Parameters:
**    void *((*cb)(XMWidget *, XtPointer, XtPointer)):
**       Pointer to a cell to hold the current callback.
**    XtPointer *ud:
**       Pointer to a cell to hold the user data.
*/
void Xamine_GetPaneInputCallback(panemgr_input_callback *cb,
				 XtPointer *ud)
{
  *cb = Pane_callback;
  *ud = pane_client_data;
}

/* Functional Description:
**   Xamine_SetDisplay:
**     Set a display pane contents to contain a specified spectrum.
**  Formal Parameters:
**    int row, col:
**      Row and column of the pane.
**    int spno:
**      The spectrum number to display.
*/
void Xamine_SetDisplay(int row, int col, int spno)
{
  // The first spectrum in a pane has no superimpositions.

  Xamine_CancelTimedUpdate(row, col);
  Xamine_DisableUnsuperimpose();

  switch(Xamine_SpectrumType(spno)) {
  case onedlong:
  case onedword:
    Xamine_panedb->define1d(col,
			    row,spno);
    Xamine_EnableSpectrumPresentPackage();
    break;
  case twodword:
  case twodbyte:
    Xamine_panedb->define2d(col,
			    row, spno);
    Xamine_EnableSpectrumPresentPackage();
    break;
  default:
    fprintf(stderr, "Xamine_SetDisplay - spectrum undefined %d\n",
	    spno);
    fprintf(stderr, "Ignoring request\n");
    return;
    break;
  }
  /* Now take the pane and modify the cursor to the default cursor type. */

  Display *display;
  Window  w;
  XMWidget *pane;
  Cursor   cursor;

  pane = Xamine_GetDrawingArea(row, col);
  display = XtDisplay(pane->getid());
  w       = XtWindow(pane->getid());
  cursor  = XCreateFontCursor(display, XC_crosshair);
  XDefineCursor(display, w, cursor);

  /* Add the tracking cursor handler: */

  if(Xamine_panedb->StatusBar()) {
    XtAddEventHandler(pane->getid(), 
		      PointerMotionMask | PointerMotionHintMask, False,
		      Xamine_PointerMotionCallback, (XtPointer)pane);
  }


/* Adjust the menus if this is the selected one: */

  if( (row == Xamine_PaneSelectedrow())  &&
      (col == Xamine_PaneSelectedcol())  ) {

    switch(Xamine_SpectrumType(spno)) {
    case onedlong:
    case onedword:
      Xamine_EnableSpectrumSelectedPackage();
      Xamine_Enable1dSelectedPackage();
      Xamine_Disable2dSelectedPackage();
      break;
    case twodword:
    case twodbyte:
      Xamine_EnableSpectrumSelectedPackage();
      Xamine_Enable2dSelectedPackage();
      Xamine_Disable1dSelectedPackage();
      break;
    default:
      fprintf(stderr, "Xamine_SetSelectedDisplay - spectrum undefined %d\n",
  	    spno);
      fprintf(stderr, "Ignoring request\n");
      return;
      break;
    }
  }
  /* If the display is the selected one, then we call the selection callback
  ** if there is one. 
  */

  if( (row == Xamine_panedb->current_row()) &&
      (row == Xamine_panedb->current_col()) &&
      (Select_callback)) {
    (*Select_callback)(col, row, col,row, select_client_data);
  }

}
/*
** Functional Description:
**   Xamine_SetSelectedDisplay:
**     Defines the selected pane to hold a particular spectrum number.
** Formal Parameters:
**    int spno:
**      The spectrum to hold.
*/
void Xamine_SetSelectedDisplay(int spno)
{
  int row = Xamine_panedb->current_row();
  int col = Xamine_panedb->current_col();
  Xamine_SetDisplay(row,
		    col, spno);


}


/*
** Functional Description:
**   Xamine_SetDisplayAttributes:
**      Sets the display attributes of a spectrum in a specified pane.
** Formal Parameters:
**   int row, col:
**     Specifies which row/column to modify.
**   win_attributed *attribs:
**     Attributes of the spectrum.
*/
void Xamine_SetDisplayAttributes(int row, int col, win_attributed *attribs)
{
  win_attributed *def;
  int is1d;

  def  = Xamine_panedb->getdef(col,row);
  if(def == NULL) return;	/* No longer a spectrum there. */
  is1d = def->is1d();

  /* Stop any timed update that might be in progress */

  Xamine_CancelTimedUpdate(col,row);


  if(Xamine_panedb->defined(col,
			    row)) {
    if(attribs->is1d() != def->is1d()) {
      fprintf(stderr,
	      "Xamine_SetDisplayAttributes %s\n%s\n",
	      "Mismatch between dimensionality of setting and current spectrum",
	      "Ignoring the request and attempting to continue\n");
      return;
    } 
    def->setattribs(*attribs);
    if(def->is1d()) {
      win_1d *def1 = (win_1d *)def;
      win_1d *at1  = (win_1d *)attribs;


      def1->setrend(at1->getrend());
      if(at1->isexpanded())	/* Copy the expansion state... */
	def1->expand(at1->lowlimit(), at1->highlimit());
      
      SuperpositionList &sl = def1->GetSuperpositions();
      SuperpositionList &slat = at1->GetSuperpositions();
      sl = slat;

	      
    }
    else {
      win_2d *def2 = (win_2d *)def;
      win_2d *at2  = (win_2d *)attribs;
   
      def2->setrend(at2->getrend());
      if(at2->isexpanded())	/* Copy the expansion state. */
	def2->expand(at2->xlowlim(), at2->xhilim(),
		     at2->ylowlim(), at2->yhilim());

    }
  }
  else {
    fprintf(stderr,
	    "Xamine_SetDisplayAttributes %s\n%s\n",
	    "Attempted to set attributes on undefined spectrum pane",
	    "Ignoring the request and attempting to continue");
  }
  /*
  ** If the attributes specify a timed update, then start that:
  */
  if(def->autoupdate_enabled()) 
    Xamine_ScheduleTimedUpdate(Xamine_panedb->pane(row,col),
			       col, row, def->update_interval());
}
/*
** Functional Description:
**   Xamine_SetSelectedDisplayAttributes:
**     Sets the display attributes of a spectrum.
** Formal Parameters:
**   win_attribute *attribs:
**     Pointer to an attribute block to use to define the display attributes.
*/
void Xamine_SetSelectedDisplayAttributes(win_attributed *attribs)
{
  int col = Xamine_panedb->current_col();
  int row = Xamine_panedb->current_row();
  
  Xamine_SetDisplayAttributes(row, col, attribs);
}

/*
** Functional Description:
**   Xamine_GetSelectedDisplayAttributes:
**     Retrieves the display attributes of the selected spectrum.
** Returns:
**   win_attributed * pointer to the display attributes of the spectrum.
**                    note that this is the in-place pointer so therefore
**                    it's possible to get and then modify the attributes
**                    If no spectrum is associated with the pane, then
**                    NULL Is returned.
*/
win_attributed *Xamine_GetSelectedDisplayAttributes()
{

  return Xamine_panedb->getdef(Xamine_panedb->current_col(),
			       Xamine_panedb->current_row());
}
/*
** Functional Description:
**   Xamine_GetDisplayAttributes:
**     Gets the display attributes of an arbitrary pane.
**   Formal Parameters:
**     int row, col:
**        The pane coordinates.
** Returns:
**   win_attributed * pointer to the display attributes desired.
**                    If NULL< then no spectrum is present.
*/
win_attributed *Xamine_GetDisplayAttributes(int row, int col)
{
  return Xamine_panedb->getdef(col, row);
}


/*
** Functional Description:
**   Xamine_GetPaneDb:
**     Escape function to return the pointer to the Xamine database.
*/
pane_db *Xamine_GetPaneDb()
{
  return Xamine_panedb;
}

/*
**  The functions on this page manipulate the selection changed callback.
**  The Selection changed callback is called each time the selection frame
**  is changed by the user.
*/

/*
** Functional Description:
**    Xamine_SetSelectCallback:
**      This function sets up a selection changed callback.
** Formal Parameters:
**   void (*callback)(int, int, int, int, XtPointer):
**      Pointer to the callback function. The first two integers are the
**      prior selection column and row respectively, the last two integers
**      are the current selection column and row respectively.
**   XtPointer client_data:
**      Data that will be passed to the callback as the last argument
*/
void Xamine_SetSelectCallback(void (*callback)(int, int, int, int, XtPointer),
			      XtPointer client_data)
{
  Select_callback    = callback;
  select_client_data = client_data;

}
/*
** Functional Description:
**    Xamine_ClearSelectCallback:
**      Clears the current selection callback.
** Formal Parameters:
**    NONE:
*/
void Xamine_ClearSelectCallback()
{
  Select_callback = NULL;
  select_client_data = NULL;
}
/*
** Functional Description:
**   Xamine_GetSelectCallback:
**     Returns the selection callback information to the caller.
**     This allows callback chaining as well as the determination of
**     prior callback existence for partially modal dialogs.
** Formal Parameters:
**    void *((*callback)(int, int, int, int, XtPointer))
**       Pointer to a buffer to hold the current callback function pointer.
**    XtPointer *client_data:
**       Pointer to the buffer to hold the current client data.
*/
void Xamine_GetSelectCallback(panemgr_select_callback *callback,
			      XtPointer *client_data)
{
  *callback = Select_callback;
  *client_data = select_client_data;
}

/*
** Functional Description:
**   Xamine_SetShellSize:
**     This function sets the size of the Xamine shell.
** Formal Parameters:
**     Dimension xsize:
**       no. of X pixels (width).
**     Dimension ysize:
**       no. of Y pixels (height).
*/
void Xamine_SetShellSize(Dimension xsize, Dimension ysize)
{
  XMWidget *w = Xamine_panedb->manager();
  Widget wid  = w->getid();

  while(!XtIsApplicationShell(wid)) wid = XtParent(wid);

  XtVaSetValues(wid,
		XmNwidth, xsize,
		XmNheight, ysize,
		NULL);
  Xamine_UpdateAll(Xamine_panedb->manager(), NULL, NULL);
}

/*
**  Functional Descriptions:
**    Xamine_EnableStatusBar     - Turn on the status bar.
**    Xamine_DisableStatusBar    - Turn off the status bar.
**    Xamine_GetStatusBarState   - Get current on/offedness of status bar.
**      These functions manipulate the state of Xamine's status bar.
**      The status bar is a widget coupled with the presence of mouse motion
**      hit event handers on each window which cause the values displayed
**      in the widget to change as the cursor is swiped across the display 
**      surface.
** Returns:
**    The status bar state prior to the modification (if any).
*/
Boolean Xamine_GetStatusBarState()
{
  return Xamine_panedb->StatusBar();
}

/* ******************************* */

Boolean Xamine_EnableStatusBar()
{
  if(Xamine_panedb->StatusBar()) { /* If already enabled, just return state. */
    return True;
  }
  else {			    /* Not yet enabled, so enable. */
    Xamine_panedb->StatusBar(True);      /* Set the new state. */
    Xamine_GetCursorLocator()->Manage(); /* Make the bar visible. */
    for(int j = 0; j < Xamine_panedb->ny(); j++) /* Add pane event handlers */
      for(int i = 0; i < Xamine_panedb->nx(); i++) { /* For defined panes */
	if(Xamine_panedb->defined(i,j)) {
	  XMWidget *pane = Xamine_panedb->pane(i,j);
	  XtAddEventHandler(pane->getid(),
			    PointerMotionMask | PointerMotionHintMask, False,
			    Xamine_PointerMotionCallback, (XtPointer)pane);
	}
      }
    return False;
  }
}

/* *********************************   */

Boolean Xamine_DisableStatusBar()
{
  if(Xamine_panedb->StatusBar()) { /* Currently enabled, so disable: */
    Xamine_GetCursorLocator()->UnManage(); /* Make the bar invisible. */
    Xamine_panedb->StatusBar(False); /* Set the new state. */
    for(int j = 0; j < Xamine_panedb->ny(); j++) { /* Remove event handlers */
      for(int i = 0; i < Xamine_panedb->nx(); i++) { /* For defined panes. */
	if(Xamine_panedb->defined(i,j)) {
	  XMWidget *pane = Xamine_panedb->pane(i,j);
	  XtRemoveEventHandler(pane->getid(),
			       PointerMotionMask | PointerMotionHintMask,
			       False, Xamine_PointerMotionCallback, pane);
	}
      }
    }
    return True;
  }
  else {			/* Currnetly disabled so just return state */
    return False; 
  }
}


/*
** Functional Description:
**   Xamine_DeleteBackingStore:
**     Deletes the backing store corresponding to a  pane.
** Formal Parameters:
**   int row, col:
**     Row and column of the pane.
*/
void Xamine_DeleteBackingStore(int row, int col)
{
  Xamine_panedb->DeleteBackingStore(row, col);
}

/*
** Functional Description:
**   Xamine_GetBackingStore:
**     Returns a pointer to the backing store that is associated with a
**     pane
** Formal Parameters:
**   int row, col:
**     The coordinates of the pane of interest.
*/
Drawable Xamine_GetBackingStore(int row, int col)
{
  return Xamine_panedb->GetBackingStore(row, col);
}
/*
** Xamine_SetBackingStore:
**   Sets the current value of the backing store.  If a backing store exists,
**   it is deleted.
** Formal Paramters:
**    int row, col:
**      Coordinates of the pane.
**    Drawable img:
**      New filled backing store.
*/
void Xamine_SetBackingStore(int row, int col, Drawable img)
{
  if(Xamine_GetBackingStore(row, col) != (Drawable)NULL) {
    Xamine_DeleteBackingStore(row, col);
  }

  Xamine_panedb->SetBackingStore(row, col, img);
}
