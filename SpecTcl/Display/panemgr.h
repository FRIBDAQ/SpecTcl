/*
** Facility:
**   Xamine -- NSCL histogram display subsystem.
** Abstract:
**   panemgr.h  - This include file describes the objects, and externals
**                for clients of the pane manager class.  A pane manager
**                does the display part of the window management.
** Author:
**   Ron Fox
**   July 8, 1993
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   0 5/26/94 
*/

#ifndef _PANEMGR_H
#define _PANEMGR_H
#include "XMManagers.h"
#include "dispwind.h"

#define XAMINE_PANES_XPIXELS  640
#define XAMINE_PANES_YPIXELS  600

#define XAMINE_PANES_MINXPIXELS 300
#define XAMINE_PANES_MINYPIXELS 310

#define XAMINE_PANEMGR_XPIXELS 512
#define XAMINE_PANEMGR_YPIXELS 512

typedef void (*panemgr_input_callback)(XMWidget *, XtPointer, XtPointer);
typedef void (*panemgr_select_callback)(int,int,int,int, XtPointer);
enum Xamine_refreshstate {
                          rfsh_idle,
			  rfsh_queued,
			  rfsh_beginning,
			  rfsh_titles,
			  rfsh_axes,
			  rfsh_contents,
 			  rfsh_grobjs,
			  rfsh_callback,
			  rfsh_aborting
			  };
		      
			
class pane_db : public win_db {
 protected:
  int selected_row;
  int selected_column;
  Boolean status_bar;
  XMFrame *frames[WINDOW_MAXAXIS][WINDOW_MAXAXIS];
  XMWidget *panes[WINDOW_MAXAXIS][WINDOW_MAXAXIS];
  XMForm *manager_widget;
  Xamine_refreshstate update_state[WINDOW_MAXAXIS][WINDOW_MAXAXIS];
  XtIntervalId        update_tid[WINDOW_MAXAXIS][WINDOW_MAXAXIS];
  Drawable  backing_store[WINDOW_MAXAXIS][WINDOW_MAXAXIS];
  void init_panedb(XMForm *parent, win_title title);

 public:
  /*
  ** Constructors and destructors 
  */

  pane_db(XMForm *parent, win_title title) :
    win_db(1,1, title) { init_panedb(parent, title);
		         status_bar  = True;
		       }
   
  ~pane_db();

  /* Additional methods above win_db */

  Boolean StatusBar()   { return status_bar; }
  void    StatusBar(Boolean newval)   { status_bar = newval; }

  void setgeometry(int nrow, int ncol); /* Reset the geometry */
  void select(int row, int column);	    /* Select a row/column */
  void settitle(char *title);	/* Set the database/window title.  */
  void zoom();			/* Zoom picture to current spectrum. */
  void unzoom();		/* Unzoom picture to all spectra.    */

  XMFrame *frame(int row, int col) {return frames[col][row]; }
  XMWidget *pane(int row, int col)  {return panes[col][row];   }
  XMForm *manager()           { return manager_widget;   }


  int current_row() { return selected_row; }
  int current_col() { return selected_column; }

  /* Handle the refresh status: */

  Xamine_refreshstate refresh_state(int row, int col) 
    { return update_state[col][row]; }
  void                refresh_state(int row, int col, 
				    Xamine_refreshstate state) 
    { update_state[col][row] = state; }
  
  XtIntervalId updatetimer(int row, int col) {return update_tid[col][row]; }
  void updatetimer(int row, int col, XtIntervalId tid) 
    { update_tid[col][row] = tid; }

  void DeleteBackingStore(int row, int col);
  void SetBackingStore(int row, int col, Drawable img) {
    backing_store[col][row] = img;
  }
  Drawable GetBackingStore(int row, int col) 
    { return backing_store[col][row]; }
};

/* Pane database manager functions: */

void SetWindowLabel(Widget w, char *string); /* Set a X window label. */
void Xamine_Initialize_panedb(XMForm *parent); /* Init database. */
void Xamine_SetPaneGeometry(int ncol, int nrow); /* Set the geometry. */
void Xamine_SelectPane(int row, int col); /* Select a pane.  */

XMWidget *Xamine_Getpanemgr();	/* Get the pane manager widget. */

int Xamine_Panerows();		/* get the pane geometry. */
int Xamine_Panecols();
int Xamine_PaneSelectedrow();	/* Get the selected pane coordinates. */
int Xamine_PaneSelectedcol(); 

XMWidget *Xamine_GetSelectedDrawingArea(); /* Return selected drawing area */
XMFrame *Xamine_GetSelectedFrame();        /* Return selected frame */
XMWidget *Xamine_GetDrawingArea(int row, int col); /* Ret specific D.A. */
XMFrame *Xamine_GetFrame(int row, int col); /* Return specific frame. */
void    Xamine_ToggleZoom(XMWidget *toggle, 
			  XtPointer cd, 
			  XtPointer cbd);	/* Zoom toggle callback */
void    Xamine_ZoomNext(XMWidget *buton,
			XtPointer cd,
			XtPointer cbd);	/* zoom to next pane. */
void   Xamine_ZoomNext(XMWidget *button,
		       XtPointer cd,
		       XtPointer cbd);

void Xamine_SetPaneInputCallback(void (*callback)(XMWidget *, /* Set input */
						  XtPointer, /* callback on */
						  XtPointer),
				 XtPointer client_data = NULL); /* pane. */
void Xamine_ClearPaneInputCallback(); /* Clear callback on selected pane. */
void Xamine_GetPaneInputCallback(void *panemgr_input_callback,
				 XtPointer *ud);

void Xamine_SetDisplay(int row, int col, int spno);
void Xamine_SetDisplayAttributes(int row, int col, win_attributed *attribs);

void Xamine_SetSelectedDisplay(int spno);
void Xamine_SetSelectedDisplayAttributes(win_attributed *attribs);
win_attributed *Xamine_GetDisplayAttributes(int row, int col);
win_attributed *Xamine_GetSelectedDisplayAttributes();

pane_db *Xamine_GetPaneDb();

void Xamine_ResetShellSize(XMWidget *wid);

void Xamine_SetSelectCallback(void (*callback)(int, int, int, int, XtPointer),
			      XtPointer client_data = NULL);
void Xamine_ClearSelectCallback();
void Xamine_GetSelectCallback(panemgr_select_callback *cb,
			      XtPointer *client_data);
void Xamine_SetShellSize(Dimension xsize, Dimension ysize);

Boolean Xamine_EnableStatusBar();
Boolean Xamine_DisableStatusBar();
Boolean Xamine_GetStatusBarState();

void Xamine_SetBackingStore(int row, int col, Drawable img);
void Xamine_DeleteBackingStore(int row, int col);
Drawable Xamine_GetBackingStore(int row, int col);

#endif
