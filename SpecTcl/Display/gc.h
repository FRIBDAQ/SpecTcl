//
// Facility:
//    X11/Motif C++ support.
// Abstract:
//   gc.h:
//     This file contains support for objectifying the X11 graphical context.
//     The class defined here contains all operations on the graphical
//     object as well as copy constructors and destructors.
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   August 17, 1995
//
//    @(#)gc.h	1.1 10/3/95 
//
///////////////////////////////////////////////////////////////////////////


#include <X11/Xlib.h>
#ifndef __GC_H_
#define __GC_H_

class XGC {
public:
  // Data items.

  GC gc;

  // Methods.

  // Construction and destruction.

  XGC();		// Default constructor makes null GC.
  XGC(XGC &original);	// Copy constructor.
  XGC(Display *disp, Drawable d); // Make generic GC.
  XGC(Display *disp, Drawable d, // fully expose the XCreateGC mechanism.
      XGCValues *values, 
      unsigned long valuemask);
  XGC(Display *disp,Drawable d,  GC gc); // Make object from X-11 stuff.
  virtual ~XGC();		// Destroy the graphical context.

  // Manipulators:

  void SetValues(XGCValues *values, unsigned long valmask);
  void SetArcMode(int arc_mode);
  void SetBackground(unsigned long background);
  void SetClipMask(Pixmap clipmask);
  void SetClipOrigin(int xorg, int yorg);
  void SetClipRectangles(int xorg, int yorg, 
			 XRectangle rects[], int nrects,
			 int ordering);
  void SetDashes(int dash_offset, char dash_list[], int dashcount);
  void SetFillRule(int fill_rule);
  void SetFillStyle(int fill_style);
  void SetForeground(long fg);
  void SetFunction(int draw_function);
  void SetGraphicsExposure(Bool exposure);
  void SetLineAttributes(unsigned int width, 
			 int style, 
			 int cap, 
			 int join);
  void SetPlaneMask(unsigned long planes);
  void SetState(unsigned long fg, unsigned long bg, 
	   int function, unsigned long plane_mask);
  void SetStipple(Pixmap stipple);
  void SetSubwindowMode(int subwindow_mode);
  void SetTSOrigin(int xorg, int yorg);
  void SetFont(Font font);


  void GetValues(XGCValues &vals, unsigned long mask);
protected:

  // Data items.

  Display *disp;		// Display which GC is set on.
  Drawable draw;		// Drawable on which GC was created.
};

#endif
