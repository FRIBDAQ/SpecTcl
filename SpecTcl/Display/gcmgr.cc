//
// Facility:
//   Xamine - NSCL Display program.
// Abstract:
//   gcmgr.cc:
//     This file implements a graphical context manager.  
//     This is a performance twiddle on Xamine to try to reduce the number
//     of graphical context deletions and creations incurred by various
//     Xamine operations (we found out that Motif is bad enough there).
//     Xamine will create and cache three graphical contexts. Each one is suitable
//     for use for one aspect of drawing in Xamine's Drawing Area widgets.
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   @(#)gcmgr.cc	1.1 10/3/95 
//
static char *sccsinfo = "@(#)gcmgr.cc	1.1 10/3/95 ";

#include <assert.h>
#include "xaminegc.h"
#include "grobjdisplay.h"

static XamineSpectrumGC *Spectrum = NULL;
static XamineTextGC     *Text     = NULL;
static XamineGrobjGC    *Grobj    = NULL;


//
// Functional Description:
//    Xamine_GetSpectrumGC:
//      This function returns a graphical context suitable for drawing
//      the spectrum block of a pane.  If the Spectrum GC is not yet
//      in existence, then it will be made.
// Formal Parameters:
//    XMWidget &wid:
//      Widget used to create the graphical context if necessary.
//
XamineSpectrumGC *Xamine_GetSpectrumGC(XMWidget &wid)
{
  if(!Spectrum) {
    Spectrum = new XamineSpectrumGC(wid);
  }
  assert(Spectrum);
  return Spectrum;

}

//
// Functional Description:
//   Xamine_GetTextGC:
//     This function returns a graphical context object suitable
//     for use in drawing text items in Xamine's title blocks.
// Formal Parameters:
//   XMWidget &wid:
//     Widget used to create the gc if necessary.
//
XamineTextGC *Xamine_GetTextGC(XMWidget &wid)
{
  if(!Text) {
    Text = new XamineTextGC(wid);
  }
  assert(Text);
  return Text;
}

//
// Functional Description:
//    Xamine_GetGrobjGC:
//       Returns a pointer to a graphical context suitable for
//       drawing graphical objects.
// Formal Parameters:
//   XMWidget &wid:
//      Widget used to instantiate the object if needed.
//
XamineGrobjGC *Xamine_GetGrobjGC(XMWidget &wid)
{
  int findex = Xamine_GetObjectLabelIndex(XtDisplay(wid.getid()));
  if(!Grobj) {
    Grobj = new XamineGrobjGC(wid);
  }
  assert(Grobj);
  Grobj->SetFont(findex);
  return Grobj;
}
