/*
** Facility:
**   Xamine --  NSCL Display program.
** Abstract:
**   grafinput.h:
**     This header file contains a definition of the GraphicalInput class.
**     That class is responsible for setting up the common behavior for 
**     Xamine functions which require graphical input.  These functions
**     include accepting expansion regions, summing regions, markers,
**     cuts, bands and contours.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**    @(#)grafinput.h	8.1 6/23/95 
*/

#ifndef _GRAFINPUT_H_INSTALLED
#define _GRAFINPUT_H_INSTALLED

#include "XMDialogs.h"
#include "helpmenu.h"
#include "refreshctl.h"

struct point			/* Define a graphical input point. */
{
  int x;
  int y;
};

class GraphicalInput : public XMCustomDialog
{
 protected:
  Xamine_help_client_data help;	     /* Controls the help display */
  int                     row, col;  /* the pane row/column we're working in */
 public:

  /* Constructors and destructors: */

  GraphicalInput(XMWidget *parent, char *name, 
		 char **help_text);
  virtual ~GraphicalInput();


  /* Manage the standard callbacks */

  void ClearStandardCallbacks();
  void SetStandardCallbacks();

  /* The following functions exist for all instances, but actual behavior
  ** varies:
  */
  virtual void ClearState() = 0;	  /* Set state back to null. */
  virtual void DrawPoints(XMWidget *pane,
			  Xamine_RefreshContext *ctx = NULL) = 0; /* Draw the points. */
  virtual void ClearPoints(XMWidget *pane,
			   Xamine_RefreshContext *ctx = NULL)= 0; /* Clear points from screen. */
  virtual void ClearDialog() = 0;	  /* all points from prompter */
  virtual void AddPoint(point &pt) = 0;   /* Add last point in list   */
  virtual void DelPoint() = 0;            /* Remove last point in list */
  virtual Boolean DoAccept() = 0;         /* Perform accept function.  */
  /*
  ** Functions below have mostly common functionality, but the user might
  ** replace  them if necessary and call the parent class functions
  ** as needed.
  */
  virtual void MouseHit(XMWidget *wid,	         /* Process mouse events. */
		        XtPointer call_d);
  virtual void KeyboardInput(XMWidget *wid, XtPointer call_d);
  virtual void SelectChanged(int oldc, int oldr, /* Process selection  */
		     int newc, int newr);         /* changed callbacks  */
  virtual void Refresh(Xamine_RefreshContext *ctx); /* Redraw callback  */

  virtual void CancelCallback(XtPointer call_d); /* Cancel callback.  */
  virtual void ApplyCallback(XtPointer call_d);  /* Apply callback    */
  virtual void OkCallback(XtPointer call_d);     /* Ok Callback.      */
  /*
  ** Interrogators:
  */
  int inputrow()  { return row; }
  int inputcol()  { return col; }
  /*
  ** Modifiers:
  */
  void setrow(int r) { row = r; }
  void setcol(int c) { col = c; }
};
#endif



