/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine - NSCL display program.
** Abstract:
**   trackcursor.cc:
**      This file contains code that is associated with the
**      tracking cursor operations.  The main entry is an event handler
**      for pointer motion hints which updates the display of the
**      cursor position.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files:
*/
#include <config.h>

#include "XMWidget.h"
#include "XMManagers.h"
#include "trackcursor.h"
#include "buttonsetup.h"
#include "location.h"
#include "panemgr.h"
#include "dispwind.h"
#include "dispshare.h"
#include "convert.h"
#include "spcdisplay.h"
#include "dispshare.h"
#include "mapcoord.h"
#include "axes.h"


#include <iostream>
#include <math.h>

/*
** External references:
*/
extern spec_shared *xamine_shared;
/*
** Static storage:
*/
static Xamine_Location *location_panel;



/* 
** Functional Description:
**   Xamine_PointerMotionCallback:
**      This callback is installed to respond to pointer motion hints.
**      We read the pointer position, convert it to world coordinates in the
**      pane and display those in the tracking cursor labels.
** Formal Parameters:
**    Widget wid:
**      Widget which contains the pane widget.
**    XtPointer userd:
**      In this case the pointer to the widget object which called us.
**    Xevent *evt:
**      Event which called us.
**    Boolean *continue:
**     Set FALSE to avoid further processing of this event by other callbacks.
*/
void Xamine_PointerMotionCallback(Widget wid, XtPointer userd, XEvent *evt,
				  Boolean *cont)
{
  XMWidget *w = (XMWidget *)userd;
  *cont = True;			/* Never stop event chaining. */
#ifdef DEBUG
  fprintf(stderr,"Xamine_PointerMotionCallback\n");
#endif

  Display *disp;
  Window  widgetwin;
  Window root, child;
  int rx,ry;
  int wx,wy;
  unsigned int keys_buttons;
  XWindowAttributes wattrib;

  disp = XtDisplay(wid);
  widgetwin = XtWindow(wid);

  /* IF the window associated with the widget is not mapped or visible
  ** then we just forget the whole thing:
  */

  if(widgetwin == (Window)NULL) return;
  XGetWindowAttributes(disp, widgetwin, &wattrib);
  if(wattrib.map_state != IsViewable) return;
 
  /* Get the current cursor position relative to the window origin */

  XQueryPointer(disp, widgetwin, &root, &child, &rx, &ry, 
		&wx, &wy, &keys_buttons);


#ifdef DEBUG
  fprintf(stderr, "  Pointer raw position is (%d,%d)\n", wx, wy);
#endif

  /* Next get enough information to build a converter and convert the */
  /* raw position into the real position:                             */

  struct spec_location locdata;
  long                  index;
  int          row, col;
  pane_db     *pdb;
  win_attributed *atts;

  /* Get the attributes and make sure they're still defined */

  w->GetAttribute(XmNuserData, &index);	/* Panes store pane index as userd. */
  row = index / WINDOW_MAXAXIS;
  col = index % WINDOW_MAXAXIS;
  pdb = Xamine_GetPaneDb();
  atts= pdb->getdef(col, row); 
  if(atts == NULL) return;	/* Spectrum disappeared before we got there */
	
	// This rectangle defines the pixel coordinate window in which we draw.
	
	Rectangle drawingRegion = Xamine_GetSpectrumDrawingRegion(w, atts);
	
	// We need to do two mappings:
	//  pixel -> channel
	//  channel-> world coords -- if mapped.
	//  Note that the Y value depends on the 1/2 d-dness.
	//  For 1d, Y is the height in counts.  For 2-d the y coordinate
	//
	
	

  /* If the spectrum has become undefined, then we remove it which will	    */
  /* also update it and clear the pane.					    */

   int sid = atts->spectrum();
   if (Xamine_SpectrumType(sid) == undefined)
   {
       Xamine_ClearPane(row, col);
       return;
   }
  /* The remainder of the operation depends on the dimensionality of the
  ** Underlying spectrum
  */
  if(atts->is1d()) {
		// Get the channel:
		
		int chan = Xamine_XPixelToChannel(
					sid, row, col, drawingRegion.xbase, drawingRegion.xmax, wx
		);
		int value= xamine_shared->getchannel(sid, chan);
		if (atts->ismapped()) {
			chan = Xamine_XChanToMapped(sid, chan);
		}
		
		// Have to transform the Y coordinate to an height; figure out
		// the Y range and do it for linear:
		
		double base = 0;			/* Assume zero bias... */
		if(atts->hasfloor()) base = atts->getfloor();
		double maximum  = atts->getfsval();	/* Assume no ceiling. */
		if(atts->hasceiling()  && (atts->getceiling() < maximum)) {
			maximum = atts->getceiling();
		}
		// Correct if log:
		
		if (atts->islog()) {
			if (base > 0) {
				base = log10(base);
			}
			if (maximum > 0) {
				maximum = log10(maximum);
			}
		}
		
		double height = Transform(drawingRegion.ybase, 0, base, maximum, wy);
		
		if (atts->islog()) {
			height = exp10(height);
		}
    // Now get the locator and set it's values:
		
		Xamine_Location* l = Xamine_GetCursorLocator();
		l->Xpos(chan);
		l->Ypos(height);
		l->Counts(value);
		return;
  }
  else {
    Xamine_Convert2d cvt(w, atts, xamine_shared);
    cvt.ScreenToSpec(&locdata, wx, wy);
  }
  /* Get the location object pointer and set new values: */

  Xamine_Location *l = Xamine_GetCursorLocator();
  l->Spectrum(atts->spectrum());

  /* Display the center of the bin as the cursor location */
  float fx, ffy;
  float fy = 0, ffx = 0;
  fx = (Xamine_XChanToMapped(sid, locdata.xpos));
  fy = (Xamine_YChanToMapped(sid, locdata.ypos));

  // These only make sense if we are 2-d, or flipped
  if((atts->is1d()) || (atts->isflipped())) {
    ffy = (Xamine_YChanToMapped(sid, locdata.ypos));
    ffx = (Xamine_XChanToMapped(sid, locdata.xpos));
  }

  if(atts->ismapped()) {
    if(atts->is1d()) {
      if(atts->isflipped()) {
				l->Xpos(locdata.xpos);
				l->YMappedPos(ffy);
      }
      else {
				l->XMappedPos(fx);
      	l->Ypos(locdata.ypos);
      }
    }
    else {
      if(atts->isflipped()) {
				l->XMappedPos(ffx);
				l->YMappedPos(ffy);
      }
      else {
				l->XMappedPos(fx);
				l->YMappedPos(fy);
      }
    }
  }
  else {
    l->Xpos(locdata.xpos);
    l->Ypos(locdata.ypos);
  }
  l->Counts(locdata.counts);
}


/*
** Functional Description:
**    Xamine_ToggleStatusBar:
**      This function is called when the toggle button corresponding to
**      the status bar on/off toggle changes state.
** Formal Parameters:
**   XMWidget *wid:
**     The toggle button widget.
**   XtPointer ud:
**     User data (not used).
**   XtPointer cd:
**     Callback data which is actually an XmToggleButtonCallbackStruct.
*/
void Xamine_ToggleStatusBar(XMWidget *wid, XtPointer ud, XtPointer cd)
{
  XmToggleButtonCallbackStruct *why = (XmToggleButtonCallbackStruct *)cd;

  if(why->set) {
    Xamine_EnableStatusBar();
  }
  else {
    Xamine_DisableStatusBar();
  }
    
}

/*
** Functional Description:
**   Xamine_GetCursorLocator()
**     This function returns a pointer to the object which displays the
**     current cursor location.
*/
Xamine_Location *Xamine_GetCursorLocator()
{
  return location_panel;
}

/*
** Functional Description:
**   Xamine_SetupLocator:
**     This function sets up Xamine's locator status bar.
** Formal Parameters:
**     XMWidget *parent:
**        The parent widget.
** Returns:
**    Pointer to the top of the created widget tree.
*/
XMWidget *Xamine_SetupLocator(XMWidget *parent)
{
  XMFrame *loc_frame;
  Arg frame_attribs;

  /* Create the bounding frame: */

  XtSetArg(frame_attribs, XmNshadowType, XmSHADOW_ETCHED_IN);
  loc_frame = new XMFrame("Loc_f",   *parent, &frame_attribs, 1);

  /* Create the locator widget.  */

  location_panel = new Xamine_Location(const_cast<char*>("Location"), *loc_frame);

  location_panel->Manage();
  loc_frame->Manage();

  return loc_frame;
}
