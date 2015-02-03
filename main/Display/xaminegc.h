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

/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   xaminegc.h
**      Defines classes derived from the XGC class.  These are
**      graphical contexts which are specialized for particular uses
**      in the Xamine program.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)xaminegc.h	1.2 10/23/95 
*/
#ifndef _xaminegc_h
#define _xaminegc_h
#include "dispwind.h"
#include "gc.h"

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

//
//  XamineSpectrumGC:
//     This represents a graphical context used to draw 1-d and 2-d
//     spectra.  No special functions are needed... just the range of
//     contructors and a pass through destructor.
//
class XamineSpectrumGC : public XGC
{
public:
  XamineSpectrumGC();	// Default constructor (null GC).
  XamineSpectrumGC(XamineSpectrumGC &src); // Copy constructor.
  XamineSpectrumGC(XMWidget &wid); // Make GC from widget.
  XamineSpectrumGC(Display *disp, Drawable d,  GC gc); // Make from X-11 GC.
  virtual ~XamineSpectrumGC() {}		  // Pass thru destructor.
};

//
//   XamineTextGC:
//      This represents a graphical context used to draw text (e.g.
//      axis labels and grobj labels.
//
class XamineTextGC : public XGC
{
public:
  XamineTextGC();
  XamineTextGC(XamineTextGC &src);
  XamineTextGC(XMWidget &wid);
  XamineTextGC(Display *disp,Drawable d,  GC gc);
  virtual ~XamineTextGC() {}

//   Font manipulation stuff.

  void SetFont(int index);
  void SetFont(char *string, int xsize, int ysize);
  void SetSmallestFont();
};

//
// XamineGrobjGC:
//    This represents a graphical context used to draw graphical
//    objects.  It's derived from XamineTextGC because
//    grobjs might have labels.
//
class XamineGrobjGC : public XamineTextGC
{
public:
  XamineGrobjGC();
  XamineGrobjGC(XamineGrobjGC &src);
  XamineGrobjGC(XMWidget &wid);
  XamineGrobjGC(Display *disp,Drawable d, GC gc);
  virtual ~XamineGrobjGC() {}

  void SetProvisional();	// Set characteristics while accepting.
  void SetSumRegion();		// Set summing region attributes.
  void SetPermanent();		// Set characteristics of permanent obj.
  void Set2DColors(XMWidget &wid); // Set color attributes for 2-d.
  void Set1DColors(XMWidget &wid); // Set color attributes for 1-d.
  void ClipToSpectrum(XMWidget &pane, win_attributed &att);
};
#endif
