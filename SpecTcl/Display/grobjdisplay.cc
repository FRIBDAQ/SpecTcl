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
/*
** Facility:
**   Xamine -- NSCL DIsplay program.
** Abstract:
**   grobjdisplay.cc
**      This module contains code to draw graphical objects from the graphical
**      object database.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include <config.h>
#include <stdio.h>
#include "XMWidget.h"
#include "XBatch.h" 
#include "panemgr.h"
#include "grobjdisplay.h"
#include "dispwind.h"
#include "text.h"
#include "dispgrob.h"
#include "dispshare.h"
#include "errormsg.h"
#include "convert.h"
#include "colormgr.h"
#include "grobjmgr.h"
#include "sumregion.h"
#include "chanplot.h"
#include "acceptgates.h" 
#include "gc.h"
#include "xaminegc.h"
#include "gcmgr.h"

/*
 ** Defined constants:
 */
#define MARKER_LABEL_DISPLACEMENT 5 /* Label displacement from marker in Pix */
#define MARKER_RADIUS             3 /* Pixel 'radius' of diamond */
/*
 ** Local storage:
 */
static Boolean typeisoned[] = { False, /* Zero is not defined */
				  True,	/* Cut */
				  True,	/* 1d summing region. */
				  False,	/* band */
				  False,	/* contour */
				  True,	/* 1-d marker */
				  False,	/* 2-d summing region. */
				  False,	/* 2-d marker. */
				  True,     // 1d peak marker. 
				  True        // Fitline
				  };
static const int max1dtypes=sizeof(typeisoned)/sizeof(Boolean);

static int LabelIndex = -1;		/* Current grobj lbl font index.  */
/*
 ** Typedefs:
 */
typedef grobj_generic *grobjptr;
/*
 ** External refs/defs.
 */
extern spec_shared *xamine_shared; /* Spectrum storage/description region. */


/*
** Functional Description:
**   Xamine_GetObjectLabelFont:
**     Returns the font structure which corresponds to the current label
**     font.
** Formal Parameters:
**  Display *d:
**   Pointer to the display connection id.
** Returns:
**   Pointer to the font description structure or NULL if there is no
**   corresponding font.  Note if the font has not yet been set, then we
**   set it to the smallest font.
*/
XFontStruct *Xamine_GetObjectLabelFont(Display *d)
{
  if(LabelIndex < 0) {
    LabelIndex = Xamine_GetSmallestFontIndex(d);
  }

  return Xamine_GetFontByIndex(d, LabelIndex);
}

/*
** Functional Description:
**   Xamine_GetObjectLabelIndex:
**      Gets the index of the current graphical object label font.
** Formal Parameters:
**   Display *d:
**     Pointer to connection id.
** Returns:
**   Font index for grobj label font.
*/
int Xamine_GetObjectLabelIndex(Display *d)
{
  if(LabelIndex < 0) {
    LabelIndex = Xamine_GetSmallestFontIndex(d);
  }
  return LabelIndex;
}
/*
** Functional description:
**   Xamine_SetObjectLabelIndex:
**    Set the index of the grobj label font.
** Formal Parameters:
**   Display *d:
**     Pointer to the display structure.
**   int i:
**     Index of the new label font to use.
*/
void Xamine_SetObjectLabelIndex(Display *d, int i)
{
  /* Make sure the index is legal... just leave things unchanged if not */

  if(i < 0) return;
  if(i >= Xamine_GetFontCount(d)) return;

  /* Set the font index: */

  LabelIndex = i;
}

/*
 ** Functional Description:
 **   Xamine_CreateTentativeGrobjGC:
 **      This static function creates a tentative graphical context suitable
 **      for drawing graphical objects that are in the process of being
 **      built.  The graphical context given uses an alternating line style
 **      dotted line, and XOR drawing.  Color choice depends on dimensionality.
 **      1-d graphical contexts will use the foreground color for drawing
 **      while 2-d's will use the xor of the zero and top level colors.
 **
 ** Formal Parameters:
 **   Display *d:
 **     The display that the graphical object is associated with.
 **   Drawable w:
 **     An associated drawable (must have the same depth as drawables the
 **     GC is used for.
 **   XMWidget *pane:
 **     Widget that the object is used for (used mostly to supply color info.
 **   win_attributed *at:
 **     The pane attributes.  Used for color selection and to set the clipping
 **     region.
 */
GC Xamine_CreateTentativeGrobjGC(Display *d, Drawable w, XMWidget *pane,
				 win_attributed *at)
{
  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pane);
  xgc->SetProvisional();
  if(at->is1d()) xgc->Set1DColors(*pane);
  else             xgc->Set2DColors(*pane);

  GC gc =  xgc->gc;

  return gc;
  
}

/*
 ** Functional Description:
 **   Xamine_CreateGrobjGC:
 **     This static function creates a graphical context suitable for drawing
 **     graphical objects.  
 ** Formal Parameters:
 **    Display *d:
 **      Identifiers the server the GC is defined in.
 **    Window w:
 **      A window id that's used to determine the depth of image maps etc.
 **    XMWidget *wid:
 **     Underlying widget.
 **    win_attributed *attributes:
 **      Window attributres..
 */
GC Xamine_CreateGrobjGC(Display *d, Window w, XMWidget *wid, 
			win_attributed *attributes)
{
  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*wid);
  xgc->SetPermanent();
  if(attributes->is1d()) xgc->Set1DColors(*wid);
  else                   xgc->Set2DColors(*wid);

  /* Set the clipping region to cover the drawing region to allow lazy 
  ** drawers to let clipping extend lines to off window points.
  */
  Dimension nx, ny;
  int       orgx, orgy;

  Xamine_getsubwindow(wid, attributes, &orgx, &orgy, &nx, &ny);

  XRectangle rect;
  rect.x     = orgx;
  rect.y     = 0;
  rect.width = nx;
  rect.height= ny;
  xgc->SetClipRectangles(0,0, &rect, 1, Unsorted);
  
  
  /* Return the context to the caller */

  return xgc->gc;
}

/*
** Functional Description:
**    Xamine_LabelGrobj:
**       A local function to label a graphical object.  The grahpical
**       object is labelled with a string centered at the given coordinates.
**       In order to ensure that the text is visible, String images are
**       used.  This requires for ensured visibility that we produce a modified
**       GC using the fg, bg of the widget.
** Formal Parameters:
**  Display *d:
**    Identifies the server connection.
** Drawable w:
**    Identifies the window being draw into.
** GC gc:
**    The base graphical context (really we need just the font from this).
** XMWidget *wid:
**    The widget pointer for the pane being drawn into... needed to get the
**    foreground and background colors.
** char *text:
**    Text to draw as Null terminated C string.
** int x,y:
**     Mid point of the baseline of the string to draw.
*/
void Xamine_LabelGrobj(Display *d, Drawable w, GC gc, 
		XMWidget *wid, char *text, int x, int y)
{
  XGCValues gcv;

  /* Save the old foreground and background colors:
  */

  XGetGCValues(d, gc, GCForeground | GCBackground , &gcv);
 
  /* Set the fg and bg colors to match the widget fg and bg colors:
  */
  
  unsigned long fg, bg;
  wid->GetAttribute(XmNforeground, &fg);
  wid->GetAttribute(XmNbackground, &bg);
  XSetForeground(d, gc, fg);
  XSetBackground(d, gc, bg);

  /* Draw the text: */

  Xamine_DrawCenteredStringImage(d, w, gc, x,y, text);

  /* Restore the old fg and bg colors: */

  XSetForeground(d, gc, gcv.foreground);
  XSetBackground(d, gc, gcv.background);

}

/*
** Functional Description:
**  Xamine_PlotMarker:
**    This function plots a single marker.
** Formal Parameters:
**    Display *d:
**       Display connection id.
**    Drawable w:
**       Drawable we draw onto.
**    GC gc:
**       Graphical context used to do the drawing.
**    XMWidget *wid:
**       Widget of the pane.
**    Boolean label_marker:
**      True if labels should also be written.
**    Xamine_Converter *cvt:
**      Conversion object that does spectrum to pixel conversions.
**    grobj_generic *object:
**      The object we're drawing.
**    win_attributed *att:
**      Attributes of the spectrum.
*/
void Xamine_PlotMarker(Display *d, Drawable w, GC gc, XMWidget *wid,
		       Boolean label_marker, Xamine_Converter *cvt,
		       grobj_generic *object,
		       win_attributed *att)
{
  Boolean flip = (!att->is1d() && att->isflipped());

  /* Markers have a single point.  We construct a filled diamond around
  ** the point.
  */
  XFilPoly diamond(d, w, gc);
  grobj_point *marker = object->getpt(0); /* Get the marker point. */
  if(marker != NULL) {
    int x0 = marker->getx();	/* Spectrum coords of marker. */
    int y0 = marker->gety();
    if(flip) {
      int t = x0;
      x0    = y0;
      y0    = t;
    }
    int xp, yp;			/* Pixel coords of marker */

    cvt->SpecToScreen(&xp, &yp, x0, y0);

    /* Return without drawing anything if the points are outside the spectrum
    ** region:
    */

    if( (xp < 0) || (yp < 0)) return;

    /* Draw the diamond: */
 
    diamond.draw((short)xp, 
		 (short)(yp + MARKER_RADIUS)); /* Point below marker center. */
    diamond.draw((short)(xp - MARKER_RADIUS), 
			 (short)yp);       /* Point to left of marker center */
    diamond.draw((short)xp, 
		 (short)(yp - MARKER_RADIUS)); /* Point above marker center. */
    diamond.draw((short)(xp + MARKER_RADIUS), 
		 (short)yp);              /* Point to right of marker center */

    /* If marker labelling is turned on, then we draw the label centered 
    ** above the marker, displaced by the displacement amount.
    */

    if(label_marker) {
      char label[80];
      sprintf(label, " M%d", object->getid()); /* Construct marker label. */
      Xamine_LabelGrobj(d,w,gc, wid, label, xp, yp - MARKER_LABEL_DISPLACEMENT);
    }
    
  }


}


/*
** Functional Description:
**   Xamine_DrawObjects:
**      This function draws graphical objects on the display.  We do this
**      by getting the list of graphical objects associated with a spectrum.
**      For each object, we draw the associated points /figure on the 
**      window associated with the object.
** Formal Parameters:
**    Xamine_RefreshContext *ctx:
**      Refresh context produced when the update request was queued.
**    win_attributed *attributes:
**      Display attributes (mostly used to determine scaling and labelling
**      attributes.
*/
void Xamine_DrawObjects(Xamine_RefreshContext *ctx, win_attributed *attributes)
{
  XMWidget *wid = ctx->pane;
  int specid = attributes->spectrum();
  spec_type tp = Xamine_SpectrumType(specid);
  Boolean
  
  onedspec = (tp == onedword) || (tp == onedlong);

  /* Get the number of objects and allocate the list of pointers: */
  /* If there are no objects then we'll return early.             */

  int nobjects;
  grobj_generic **objects;

  nobjects = Xamine_GetSpectrumObjectCount(specid);
  if(nobjects >  0) {
   objects = new grobjptr[nobjects];
    if(objects == NULL) {
      Xamine_error_msg(wid,
		       "'new' failed in Xamine_DrawObjects for grobj_generic **");
      return;
    }
    Xamine_GetSpectrumObjects(specid, objects, nobjects, True);
    
    /*
    ** For each element in the object list if appropriate, draw the object.
    */
    for(int i = 0; i < nobjects; i++) {
      grobj_type type = objects[i]->type();
      if( (type < max1dtypes) || (onedspec != typeisoned[type])  ) {
	Xamine_DeleteGrobj(specid, objects[i]->getid());
      }
      else {
	objects[i]->draw(wid, attributes, True);
      }
    }
    delete []objects;		/* Free the object list. */
  }

  /* Now repeat the effort for stuff in the gates database  */

  nobjects = Xamine_GetSpectrumGateCount(specid);
  if(nobjects > 0) {
    objects  = new grobjptr[nobjects];
    if(objects == NULL) {
      Xamine_error_msg(wid,
		       "'new' failed in Xamine_DrawObjects for grobj_generic **2");
      return;
    }
    Xamine_GetSpectrumGates(specid, objects, nobjects, True);
    for(int i = 0; i < nobjects; i ++) {
      if( onedspec != typeisoned[objects[i]->type()]) {
	Xamine_DeleteGate(specid, objects[i]->getid());
      }
      else {
	objects[i]->draw(wid, attributes, True);
      }
    }
    delete []objects;
  }

}

/*
** Functional Description:
**   Xamine_PlotObject:
**     This function plots an object in all panes in which it should be visible.
**     We do this by invoking the object draw function once for each pane
**     in which the object's spectrum appears.
** Formal Parameters:
**   grobj_generic *obj:
**     The object that is being plotted.
**   Boolean Final:
**      True if object is being plotted in final form.
*/
void Xamine_PlotObject(grobj_generic *obj, Boolean final)
{
  int nr = Xamine_Panerows();
  int nc = Xamine_Panecols();

  for(int r = 0; r < nr; r++) 
    for(int c = 0; c < nc; c++) {
      win_attributed *att = Xamine_GetDisplayAttributes(r, c);
      if(att != NULL) {
	XMWidget *w = Xamine_GetDrawingArea(r, c);
	if(obj->getspectrum()   == att->spectrum() )
	  obj->draw(w, att, final);
      }
    }
}
