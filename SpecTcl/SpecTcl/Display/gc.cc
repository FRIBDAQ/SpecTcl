//
// Facility:
//   X11/Motif C++ class level support.
// Abstract:
//   gc.cc:
//     This function implements the class methods of the
//     graphical context class defined in gc.h
//     This provides an object encapsulation of the X11 graphical context
//     object.   All known X11 GC manipulators are supplied along with
//     access to the graphical context handle (gc attribute), and
//     a wide variety of constructors including a copy constructor which
//     creates a new GC from an existing one.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    August 17, 1995
//
//    @(#)gc.cc	1.1 10/3/95 
//
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "gc.h"

static char *sccsinfo = "@(#)gc.cc	1.1 10/3/95 ";


//
// Functional Description:
//    XGC::XGC
//       Default graphical object constructor.
//       This function leaves the display member NULL and the gc member
//       zero which will hopefully prevent use of the GC.
//
XGC::XGC()
{
  gc = 0;
  disp = NULL;
  draw = 0;
}

//
// Functional Description:
//   XGC::XGC
//     Constructor which copies the default graphical context into a new
//     GC.  Not usually used, but useable.
// Formal Parameters:
//   Display *display:
//     Pointer to the display connection structure.
//   Drawable d:
//     Drawable representative of where the gc will be used.
//
XGC::XGC(Display *display, Drawable d)
{
  XGCValues vals;

  disp = display;		// Save the connection id info.
  draw = d;
  gc   = XCreateGC(disp, d, 0, &vals); // Make default gc.

}
//
// Functional Description:
//   XGC::XGC:
//     Constructor which potentially completely specifies the values of the
//     graphical context.
// Formal Parameters:
//   Display *disp:
//      Display identifier.
//   Drawable d:
//     Drawable representing where the gc will be used.
//   XGCValues *values:
//     Graphical context values.
//   unsigned long valmask:
//     Set of values to apply from the values argument
//
XGC::XGC(Display *display, Drawable d, 
	 XGCValues *values, unsigned long valmask)
{
  disp = display;
  draw = d;
  gc   = XCreateGC(disp, d, valmask, values);
}
//
// Functional Description:
//   XGC::XGC:
//     Copy constructor.  A graphical context (default) is made and all
//     of the values of the source are copied to us.
// Formal Parameters:
//    XGC &original:
//      GC to make a copy of.
// Restriction:
//    The original drawable must be alive.
//
XGC::XGC(XGC &original)
{
  XGCValues vals;

  disp = original.disp;
  draw = original.draw;
  gc   = XCreateGC(disp, draw, 0, &vals); // Make default gc.
  XCopyGC(disp, original.gc, 0xffffffff, gc); // Copy coriginal's state.

}
//
// Functional Description:
//   XGC::XGC:
//     Create a graphical context object from an X11 graphical context.
//
// Formal Parameters:
//    Display *di:
//      pointer to dispaly id.
//    Drawable d:
//      Window or widget which gc was made on.
//    GC gc:
//      Graphical context handle.

XGC::XGC(Display *di, Drawable d, GC g)
{
  gc = g;
  disp = di;
  draw = d;
}
// Functional Description:
//   XGC::~XGC
//     Destroy a graphical context.
//

XGC::~XGC()
{
  XFreeGC(disp, gc);
}

//
// Functional Description:
//  XGC::SetValues
//    Modifies (possibly completely a graphical context.
// Formal Parameters:
//   XGCValues *vals:
//     Pointer to the new values.
//   unsigned long valmask:
//     Mask of values to apply.
//
void XGC::SetValues(XGCValues *vals, unsigned long valmask)
{
  XChangeGC(disp, gc, valmask, vals);
}
//
// Functional Description:
//   XGC::SetArcMode
//     Sets the mode used for arc/chord filling.
// Formal Parameters:
//    int arc_mode:
//      One of ArcChord, or ArcPieSlice
//
void XGC::SetArcMode(int arc_mode)
{
  XSetArcMode(disp, gc, arc_mode);
}
//
// Functional Description:
//   XGC::SetBackground
//      Set a new background color pixel value.
// Formal Parameters:
//   unsigned long bg
//
void XGC::SetBackground(unsigned long bg)
{
  XSetBackground(disp, gc, bg);
}
//
// Functional Description:
//   XGC::SetClipMask
//      Set a pixmap as the clipping region.
//  Formal Parameters:
//    Pixmap clip_mask

void XGC::SetClipMask(Pixmap clip_mask)
{
  XSetClipMask(disp, gc, clip_mask);
}

//
// Functional Description:
//    XGC::SetClipOrigin:
//      Set origing relative to which the clip region is defined.
// Formal Parameters:
//   int clipx, clipy:
//     X and Y coordinates of clip origin in windwo to be drawn on.
//
void XGC::SetClipOrigin(int clipx, int clipy)
{
  XSetClipOrigin(disp, gc, clipx, clipy);
}

//
// Functional Description:
//   XGC::SetClipRectangles:
//     Defines a clipping region composed of the overlap of a set of 
//     rectangles. 
// Formal Parameters:
//   int clipx, clipy:
//      The x and y coordinates of the clip origin.
//   XRectangle rects[]:
//      The set of clipping recangles.
//   int nrects:
//      Number of rectangles in rects
//   int ordering:
//      Ordering (if known), of the rectangles.
//
void XGC::SetClipRectangles(int clipx, int clipy, XRectangle rects[],
			    int nrects, int ordering)
{
  XSetClipRectangles(disp, gc, clipx, clipy, rects, nrects, ordering);
}
//
// Functional Description:
//   SetDashes:
//     Set dashed line parameters.
// Formal Parameters:
//    int dash_offset:
//       Indicates which pixel into the pattern is the start point.
//    char dash_list[]:
//       Array of dash patterns.
//    int dcount:
//       Number of dash patterns.
//
void XGC::SetDashes(int dash_offset, char dash_list[], int dcount)
{
  XSetDashes(disp, gc, dash_offset, dash_list, dcount);
}

//
// Functional Description:
//    XGC::SetFillRule:
//      Selects the rule used to determine what the interior of a filled
//      polygon is.
//  Formal Parameters:
//    int file_rule:
//      EvenOddRule - Specifies that odd boundary crossings are interior. 
//     WindingRule  - Fills all overlapping regions of the polygon.
//
void XGC::SetFillRule(int fill_rule)
{
  XSetFillRule(disp, gc, fill_rule);
}
//
// Functional Description:
//   XGC::SetFillStyle:
//      Determines which style of fill will be used in the filled region
//      primitives.
// Formal Parameters:
//   int fill_style:
//     FillSolid   - Performes a solid fill.
//     FillTiled   - fill using the tile pattern.
//     FillStippled- Fill using the stipple pattern 0's undrawn.
//     FillOpaqueStippled - Fill using stipple pattern 0's drawn as bg.
//
void XGC::SetFillStyle(int fill_style)
{
  XSetFillStyle(disp, gc, fill_style);
}
//
// Functional Description:
//   XGC::SetForeground
//      Set the foreground color.
// Formal Parameters:
//   long fg:
//     New foreground color pixel value.
//
void XGC::SetForeground(long fg)
{
  XSetForeground(disp, gc, fg);
}
//
// Functional Description:
//   SetFunction:
//     Sets the logical function used to combine drawn pixels with the
//     pixels already in the drawable.
// Formal Parameter:
//    int function:
//       One of the Logical functions below:
//      GXclear  - Drawn pixel -> 0
//      GXand    - Drawn pixel anded with current value.
//      GXandReverse - Drawn pixel anded with current, result complemented.
//      GXcopy   - Drawn pixel value set.
//      GXandInverted - Drawn pixel inverted then anded with current.
//      GXnoop   - No action 
//      GXxor    - Drawn pixel XOR'd with current value.
//      GXor     - Drawn pixel Or'd with current value.
//      GXnor    - Drawn pixel complemented,Anded with complement of current.
//      GXequiv  - Drawn pixel complemented and XORd with dest.
//      GXInvert - Destination complemented.
//      GXorReverse - Drawn pixel or'd with complement of current.
//      GXcopyInverted - Complement of drawn pixel value.
//      GXorInverted - Drawn pixel complemented then OR'd with current.
//      GXnand    - Complement of drawn value or'd with copmlemento f current.
//      GXset     - Destination set to 1.
//
void XGC::SetFunction(int function)
{
  XSetFunction(disp, gc, function);
}
//
// Functional Description:
//   SetGraphicsExposures:
//      Enable or disable exposure events if XCopyArea or XCopyPlanes can't
//      be completely satisfied due to occlusion.
//  Bool on:
//     New state (True enables).
//
void XGC::SetGraphicsExposure(Bool on)
{
  XSetGraphicsExposures(disp, gc, on);
}

//
// Functional Description:
//   SetLineAttributes:
//     Set the line drawing components.
// Formal Parameters:
//    unsigned int wid:
//      New line width.
//    int style:
//      Set the new line style:
//         LineSolid, LineOnOffDash or LineDoubleDash
//    int cap_style:
//       Set the style for the end cap of lines:
//       CapNotLast, CapButt, CapRound, CapProjecting
//    int join_style:
//       Set the manner in which lines are joined:
//        JoinRound, JoinMiter, JoinBevel
//
void XGC::SetLineAttributes(unsigned int wid, int style, int cap_style,
			    int join_style)
{
  XSetLineAttributes(disp, gc, wid, style, cap_style, join_style);
}
//
// Functional Description:
//    SetPlaneMask:
//       Set the plane mask in a graphics context.
// Formal Parameters:
//    unsigned long plane_mask:
//        New plane selection mask.
//
void XGC::SetPlaneMask(unsigned long plane_mask)

{
  XSetPlaneMask(disp, gc, plane_mask);
}
//
// Functional Description:
//    SetState:
//      Set the drawing colors, logical function and plane mask
// Formal Parameters:
//   unsigned long fg, bg:
//     Foreground and background pixel values.
//   int function:
//     Drawing function.
//   unsigned long plane_mask:
//     Plane mask.
//

void XGC::SetState(unsigned long fg, unsigned long bg,
		   int function, unsigned long plane_mask)
{
  XSetState(disp, gc, fg, bg, function, plane_mask);
}
//
// Functional Description:
//   XGC::SetStipple:
//     Set the stippling pattern.
// Formal Parameters:
//   Pixmap stipple:
//      New stipple pattern.

void XGC::SetStipple(Pixmap stipple)
{
  XSetStipple(disp, gc, stipple);
}

//
// Functional Description:
//   XGC::SetSubwindowMode:
//      SEt a new subwindow mode:
// Formal Parameters:
//   int subwindow_mode:
//     New subwindow clip mode:
//       ClipByChildren or IncludeInferiors
//
void XGC::SetSubwindowMode(int subwindow_mode)
{
  XSetSubwindowMode(disp, gc, subwindow_mode);
}

//
// Functional Description:
//   XGC::SetTSOrigin:
//      Set the tile and stipple origin.
// Formal Parameters:
//   int tsx, tsy:
//      Tile/stipple x/y origins.
//
void XGC::SetTSOrigin(int tsx, int tsy)
{
  XSetTSOrigin(disp, gc, tsx, tsy);
}

//
// Functional Description:
//   XGC::SetFont:
//     Selects a font into the GC.
// Formal Parameters:
//   Font font:
//     New font to select.
//

void XGC::SetFont(Font font)
{
  XSetFont(disp, gc, font);
}

//
// Functional Description:
//  XGC::GetValues:
//    Gets the current settings in the graphical context.
//  XGCValues &values:
//    Buffer to hold the contenes of the query... 
//  unsigned long mask:
//    Determines which items will be fetched.
//
void XGC::GetValues(XGCValues &values, unsigned long mask)
{
  XGetGCValues(disp, gc, mask, &values);
}
