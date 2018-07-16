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

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
//
// Facility:
//  Xamine - NSCL Display program.
// Abstract:
//  xaminegc.cc:
//    This file implements classes defined in xainegc.cc  These classes
//    define Xamine specific graphical contexts derived from the XGC
//    class.
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    @(#)xaminegc.cc	1.2 10/23/95 
//
#include <config.h>
#include "gc.h"
#include "xaminegc.h"
#include "text.h"
#include "colormgr.h"
#include "chanplot.h"
#include "dispwind.h"

//
//   First we'll supply the stuff associated with XamineSpectrumGC
//   This is a graphical context used to draw spectra.  It has the following
//   characteristics.
//      Solid linestyle.
//      if in widget, the fg and bg colors are that of the widget otherwise
//      these take default values.
//      function is GXcopy
//
XamineSpectrumGC::XamineSpectrumGC() : XGC() {}
XamineSpectrumGC::XamineSpectrumGC(XamineSpectrumGC &src) : XGC(src) {}

XamineSpectrumGC::XamineSpectrumGC(XMWidget &wid) :
    XGC()			// We're on our own.
{
  Widget id = wid.getid();
  Display *d = XtDisplay(id);
  Window   w = XtWindow(id);

  XGCValues vals;

  wid.GetAttribute(XmNbackground, &vals.background);
  wid.GetAttribute(XmNforeground, &vals.foreground);
  vals.line_style = LineSolid;
  vals.function   = GXcopy;

  draw = w;
  disp = d;
  gc   = XCreateGC(disp, draw,
		   GCFunction | GCForeground | GCBackground | GCLineStyle,
		   &vals);
  
}
XamineSpectrumGC::XamineSpectrumGC(Display *disp,Drawable d, GC gc) : XGC(disp, d, gc) {}


//
// Stuff on this page relates to XamineTextGC.
// This class is used to create GC's which will be used to write text on the
// Xamine panes.  The Widget based constructor differes from default
// values as follows:
//    function   = GXcopy
//    foreground = Widget foreground.
//    background = Widget background.
//    font       = smallest font from text.h
//

XamineTextGC::XamineTextGC() : XGC() {}
XamineTextGC::XamineTextGC(XamineTextGC &src) : XGC(src) {}
XamineTextGC::XamineTextGC(XMWidget &wid) : XGC() // We're on our own
{
  Widget id = wid.getid();
  disp      = XtDisplay(id);
  draw      = XtWindow(id);

  XGCValues  vals;

  vals.function = GXcopy;
  wid.GetAttribute(XmNforeground, &vals.foreground);
  wid.GetAttribute(XmNbackground, &vals.background);

  gc = XCreateGC(disp, draw, 
		 GCFunction | GCForeground | GCBackground,
		 &vals);
  SetSmallestFont();
}
XamineTextGC::XamineTextGC(Display *disp, Drawable d, GC gc) : XGC(disp, d, gc) {}

//
// Below are overloads for SetFont which serve Xamine's needs:
//
void XamineTextGC::SetFont(int index)
{
  XFontStruct *f = Xamine_GetFontByIndex(disp, index);

  if(f != NULL) XGC::SetFont(f->fid); // Use base class method to do the set.
}

void XamineTextGC::SetFont(char *string, int xsize, int ysize)
{
  XFontStruct *f = Xamine_SelectFont(disp, string, xsize, ysize);
  if(f != NULL) XGC::SetFont(f->fid);
}

void XamineTextGC::SetSmallestFont()
{
  XFontStruct *f = Xamine_SelectSmallestFont(disp);
  if(f != NULL) XGC::SetFont(f->fid);
}

//
// The functions below are methods for the XamienGrobjGC class.
//  This class is used to draw graphical objects and their associated 
//  Labels.  The construction is identical to that of a the XamineTextGC,
//  however the function is then modified to GXxor to allow easy erasure.
//  In addition, fg  is set to widget fg ^ bg.
//
XamineGrobjGC::XamineGrobjGC() : XamineTextGC() {}
XamineGrobjGC::XamineGrobjGC(XamineGrobjGC &src) : XamineTextGC(src) {}
XamineGrobjGC::XamineGrobjGC(XMWidget &wid) : XamineTextGC(wid) 
{
  XSetFunction(disp, gc, GXxor);
  long fg, bg;
  wid.GetAttribute(XmNforeground , &fg);
  wid.GetAttribute(XmNbackground, &bg);

  SetForeground(fg ^ bg);
  SetBackground(fg ^ bg);

}
XamineGrobjGC::XamineGrobjGC(Display *disp, Drawable d, GC gc) : XamineTextGC(disp, d , gc) {}
//
// SetProvisional --
//    This sets the line style appropriate to tentative input objects:
//
void XamineGrobjGC::SetProvisional()
{
  char dash_list[2] = {2, 2};
  SetDashes(0, dash_list, 2);
  SetLineAttributes( 0, LineOnOffDash, CapButt, JoinBevel);
}
//
//   SetSumRegion
//      Sets line attributes appropriately for drawing permanent summing 
//      region.
void XamineGrobjGC::SetSumRegion()
{
  char dashes[] = {2, 2};
  SetDashes(0, dashes, 2);
  SetLineAttributes(1, LineOnOffDash, CapButt, JoinMiter);

}
//
// SetPermanent
//    Sets the line drawing attributes for a permanent gate graphical
//    object.
//
void XamineGrobjGC::SetPermanent()
{
  SetLineAttributes(1, LineSolid, CapButt, JoinBevel);

}
//
// 2-d displays require special colors to make sure the drawing is (almost)
// always readable.
//
void XamineGrobjGC::Set2DColors(XMWidget &wid)
{
  Window            w = wid.getWindow();
  XVisualInfo       v;
  XStandardColormap m;
  unsigned long bg, fg;
  
  wid.GetAttribute(XmNbackground, &bg);
  Xamine_GetVisualInfo(disp, w, &v);
  Xamine_GetX11ColorMap(m, disp, w, &v);
  fg = Xamine_ComputeDirectColor(&m, 255, 0, 0);
  
  SetPlaneMask(Xamine_GetColorPlaneMask());
  SetForeground(fg ^ bg);

}

void XamineGrobjGC::Set1DColors(XMWidget &wid)
{
    long fg, bg;
  
    SetPlaneMask(Xamine_GetColorPlaneMask());
    //wid.GetAttribute(XmNforeground, &fg);
    
    // Get a nice visible red color for provisional drawing:
    
    Window            w= wid.getWindow();
    XVisualInfo       v;
    XStandardColormap map;
    Xamine_GetVisualInfo(disp, w, &v);
    Xamine_GetX11ColorMap(map, disp, w, &v);
    fg = Xamine_ComputeDirectColor(&map, 255, 0, 0);
    
    wid.GetAttribute(XmNbackground, &bg);
    SetForeground(fg ^ bg);
}
//
// ClipToSpectrum -- This sets the clipping region to the spectrum part of
//                   the specified widget.
// Formal Parameters:
//     XMWidget &pane      - The widget that's being clipped in.
//     win_attributed &att - Attributes block of the spectrum.
//
void XamineGrobjGC::ClipToSpectrum(XMWidget &pane, win_attributed &att)
{
  XRectangle rect;
  int        orgx, orgy;
  Dimension  nx, ny;

  Xamine_getsubwindow(&pane, &att, &orgx, &orgy, &nx, &ny);

  rect.x = orgx;
  rect.y = 0;
  rect.width = nx;
  rect.height = ny;

  SetClipRectangles(0,0, &rect, 1, Unsorted);
}
