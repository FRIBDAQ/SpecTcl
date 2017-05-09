/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/



/*
** Include files:
*/

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef HAVE_DECL_MAXINT
#include <values.h>
#elif defined(HAVE_DECL_INT_MAX)
#include <limits.h>
#define MAXINT INT_MAX
#else
#define MAXINT (0x10000000)	/* *BUGBUGBUG* Need a good way to get maxint */
#endif

#include "XMWidget.h"
#include "XBatch.h"
#include "dispwind.h"
#include "dispshare.h"
#include "chanplot.h"
#include "scaling.h"
#include "colormgr.h"

#include <memory>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

#define LOG_RESOLUTION 100	/* Multiplier for log values. */
#define RANDOM_RANGE   (MAXINT-1) /* Range of values from rand */
/*
** External references:
*/
extern spec_shared *xamine_shared;
/*
** Little in-line functions:
*/
static inline double frandom()	/* Return random # in range [0,1] */
{
  return drand48();		// Can't think why not do this now.
                                // Don't really even need good randoms.
  /*
  double val;
  int    r;
  r = rand();
  if(r < 0) r = -r;
  val = (double)r;
  val = val / (float)RANDOM_RANGE;
  return val;
  */
}
static int Xamine_PutPixel8(XImage *i, int x, int y, unsigned long pixel)
{
  i->data[x + y * ((short)i->width)] = (char)pixel;
  return 0;
}



/* 
** Class and struct definitions.
*/
/* The Drawer2 family of classes draws a channel of a 2-d spectrum.
** At present we support a base class to virtualize the behavior (Drawer2)
** and classes which draw Rectangle plotes (DrawRect2), scatter plots
** (DrawScatt2), and color plots (DrawColor2).
*/
class Drawer2 {
 protected:
  float xwid, ywid;		/* Channel widths in pixels. */
  float   nx;			/* Number of X pixels available. */
  float   ny;
  float   x,y;			/* Current x/y position in pixels. */
  float   x0;			/* X origin pixel. */
  float   y0;			/* Y origin pixel. */
  float   lx;
  int    full_scale;		/* Full scale value. */
  int   xsteps,ysteps;
  int   xbin, ybin;
  int   *xstart,*xend;
  int   *ystart,*yend;
 public:
  Drawer2(float xw, float yw, int xpixels, int ypixels, 
	  short orgx, short orgy, int fs) {
    float x = (float)orgx;
    float y = (float)orgy;
    xwid   = xw;
    ywid   = yw;
    nx     = (float)xpixels;
    ny     = (float)ypixels;
    x0     = (float)orgx;
    y0     = (float)orgy;
    x      = (float)orgx,
    y      = (float)orgy;
    lx     = x0 + nx - 1;
    full_scale = fs;
    xsteps = (int)((nx/xwid) + 0.5);
    ysteps = (int)((ny/ywid) + 0.5);

    xstart = new int[xsteps];
    xend   = new int[xsteps];
    ystart = new int[ysteps];
    yend   = new int[ysteps];
    xbin   = 0;
    ybin   = 0;

    /* Fill in the dope vectors:  */

    xstart[0] = (int)x;
    x += xwid;
    xend[0]   = (int)x;
    int i;
    for( i = 1; i < xsteps; i++) {
      xstart[i] = xend[i-1];
      x      += xwid;
      xend[i]   = (int)x;
      if(xend[i] > (int)lx) {
	xend[i] = (int)lx;
      }
    }
    ystart[0] = (int)(ny-1.0);
    y -= ywid;
    yend[0]   = (int)(y);
    for(i = 1; i < ysteps; i ++) {
      ystart[i] = yend[i-1];
      y -= ywid;
      yend[i]   = (int)y;
      if(yend[i] < 0) {
	 yend[i] = 0;
       }
    }
  }
  void nextrow() {
    if(xbin != 0) {
      xbin = 0;
      ybin++;
    }
  }
  void nextcol() {
    if(ybin != 0) {
      ybin = 0;
      xbin++;
    }
  }
  int donex() {
    return (xbin >= xsteps);
  }
  int doney() {
    return (ybin >= ysteps);
  }

  virtual void next() = 0;
  virtual int  done() = 0;
  virtual ~Drawer2() {
    delete []xstart;
    delete []xend;
    delete []ystart;
    delete []yend;
  }
  virtual void drawchan(unsigned int height) = 0;
  virtual void drawchans(unsigned int *hts,int nchan) {
    while(nchan > 0) {
      drawchan(*hts++);
      nchan--;
    }
  }
};

/*
** DrawRect2 draws channels as rectangles with the size of the rectangle
** proportional to the channel height. DrawRect2f  flips X/Y axes.
*/

class DrawRect2 : public Drawer2,     /* Draw channel as a filled rectangle. */
                  public XFilRectBatch {
 public:
   DrawRect2(Display *d, Drawable w, GC c,
	     float xw, float yw, int xp, int yp, short ox, short oy, int fs) :
	       Drawer2(xw, yw, xp, yp, ox, oy, fs), XFilRectBatch(d, w, c)
	      { }
   virtual ~DrawRect2() {}
   virtual void next() { nextrow(); }
   virtual int  done() { return doney(); }
   virtual void drawchan(unsigned int height) {
     short xw = (short)(height*(xend[xbin]-xstart[xbin]+1))/full_scale;
     short yw = (short)(height*(ystart[ybin] - yend[ybin] + 1))/full_scale;
     if((xw > 0) || (yw > 0)) {	/* Don't do anything for zero valued chans. */
       draw((short)xstart[xbin], (short)ystart[ybin], xw, yw);
     }
     xbin++;
   }
};


class DrawRect2f : public Drawer2,     /* Draw channel as a filled rectangle. */
                  public XFilRectBatch {
 public:
   DrawRect2f(Display *d, Drawable w, GC c,
	      float xw, float yw, int xp, int yp, short ox, short oy, int fs) :
	       Drawer2(xw, yw, xp, yp, ox, oy, fs), XFilRectBatch(d, w, c)
	      { }
   virtual ~DrawRect2f() {}
   virtual void next()  { nextcol(); }
   virtual int done()  { return donex(); }
   virtual void drawchan(unsigned int height) {
     short xw = (short)(height*(xend[xbin] - xstart[xbin] +1))/full_scale;
     short yw = (short)(height*(ystart[ybin] - yend[ybin] + 1))/full_scale;
     if((xw > 0) || (yw > 0)) {	/* Don't do anything for zero valued chans. */
       draw((short)xstart[xbin], (short)yend[ybin], xw, yw);
     }
     ybin++;
   }
};


/*
** Drawers for scatter plots...
*/
class DrawScatter2 : public Drawer2,
                     public XPointBatch {
 int rectsize;
 public:
   DrawScatter2(Display *d, Drawable w, GC c, 
		float xw, float yw, int xp, int yp,  short ox, short oy, int fs) :
		Drawer2(xw, yw, xp, yp, ox, oy, fs), XPointBatch(d,w,c)
		  {}
   virtual ~DrawScatter2() {}
   virtual void next() { nextrow(); }
   virtual int  done() { return doney(); }
   virtual void drawchan(unsigned int height) {
     short xb = (short)xstart[xbin];		/* Coords of base of rectangle. */
     short yb = (short)ystart[ybin];

     int xm = (short)(xend[xbin]);	/* Coords of upper part of rectangle. */
     int ym = (short)(yend[ybin]);
     int wid = xm-xb;		/* Width of bin. */
     int ht = yb-ym;		/* Height of bin. */
     int pixels = wid*ht;	/* Pixels in bin. */

     int npts   = height * pixels / full_scale; /* Npts to scatter. */
     for(int i = 0; i < npts; i++) {
       short ix = xb + (short)(wid * frandom()); /* Randomize point. */
       short iy = yb - (short)(ht  * frandom());
       draw(ix,iy);		/* Draw it. */
     }
     xbin++;
   }
};

class DrawScatter2f : public Drawer2,
                     public XPointBatch {
 int rectsize;
 public:
   DrawScatter2f(Display *d, Drawable w, GC c, 
		float xw, float yw, int xp, int yp, short ox, short oy, int fs) :
		Drawer2(xw, yw, xp, yp, ox, oy, fs), XPointBatch(d,w,c)
		  {}
   virtual ~DrawScatter2f() {}
   virtual void next()  { nextcol(); }
   virtual int done()  { return donex(); }
   virtual void drawchan(unsigned int height) {
     short xb = (short)(xstart[xbin]);		/* Coords of base of rectangle. */
     short yb = (short)(ystart[ybin]);

     int xm = (short)(xend[xbin]);	/* Coords of upper part of rectangle. */
     int ym = (short)(yend[ybin]);
     int wid = xm-xb;		/* Width of bin. */
     int ht = yb-ym;		/* Height of bin. */
     int pixels = wid*ht;	/* Pixels in bin. */

     int npts   = height * pixels / full_scale; /* Npts to scatter. */
     for(int i = 0; i < npts; i++) {
       short ix = xb + (short)(wid * frandom()); /* Randomize point. */
       short iy = yb - (short)(ht  * frandom());
       draw(ix,iy);		/* Draw it. */
     }
     ybin++;
   }
};
/*   Color intensity plot:    */

class DrawColor2 : public Drawer2 {
 protected:
    Display *display;		/* Display data structure */
    Visual  *vis;		/* Visual data struct. */
    Drawable win;		/* Window in to which we're drawing  */
    GC       context;		/* Graphical context for draws. */
    char *imgdata;	/* Data for image. */
    XImage        *image;	/* Image data structure for X-11 */
    int        xpix;            /* Number of last xpixel. */
    Boolean    eightbit;	/* 8 bit image (no conversion needed). */
  public:
    DrawColor2(Display *d, Drawable pm, Drawable w, GC c,
	       float xw, float yw, int xp, int yp, short ox, short oy, int fs) :
		 Drawer2(xw, yw, xp, yp, ox, oy, fs) {
	display = d;
	win     = pm;
	context = c;
	xpix    = (int)(nx - 1.0);
	vis     = Xamine_GetVisual(display, w);
	int depth = Xamine_ColorDepth();
	int bppix = (depth+7)/8;
        bppix     = ((bppix % 2) == 0) ? bppix : bppix + 1;
	imgdata = new char[(int)nx * (int)(y0 + 1.0)* (bppix)];
	image   = XCreateImage(display, vis, depth,
			       ZPixmap, 0, imgdata,
			       (int)nx,  (int)y0+1,8, 0);
	/* We can be more efficient than X if depth = 8  */

	eightbit = False;
	if( (depth == 8)) {	/* 8 plane workstations... */
	  eightbit = True;
	  image->f.put_pixel = Xamine_PutPixel8;
	}

      }
    virtual ~DrawColor2() {
      XPutImage(display, win, context, image,
		0,0, (int)x0,0, (int)nx, (int)ny);
      image->data = NULL;
      delete []imgdata;
      XDestroyImage(image);
    }
   virtual void next() { nextrow(); }
   virtual int  done() { return doney(); }

    virtual void drawchan(unsigned int height) {
      int xb = (int)xstart[xbin];	  /* X part of rectangle starts here. */
      xb     -= (int)x0;	/* Convert to image coords. */
      int xm = (int)(xend[xbin]);
      xm     -= (int)x0;	/* X part of rectangle ends here in image. */
      int yb  = yend[ybin];
      int ym  = ystart[ybin];    
      int   pct = (height * 100 )/ full_scale;
      if((pct == 0) && (height != 0)) pct = 1; /* Display chans with >1 count */
      unsigned long pixel = Xamine_PctToPixel(pct);
      for(int j = yb; j <= ym; j++)
	for(int i = xb; i <= xm; i++)
	XPutPixel(image, i, j, pixel);
      
      xbin++;
    }
    /* Restriction... the channels must all be within the current
    **     'scan line'
    */
    virtual void drawchans(unsigned int *hts, int nchan) {
      if(!eightbit) {
         Drawer2::drawchans(hts, nchan); /* Can't optimize. */
      }
      else {
	int yb = yend[ybin]; 
	int numy = ystart[ybin] - yb;	
	int xb = xstart[xbin];
	    xb -= (int)x0;
	char *pixel = imgdata + yb*((short)image->width); /* Fill this scanline */
 
	Xamine_HtsToColor(hts, full_scale, nchan);

	for(int y = 0; y <= numy; y++) { /* Loop over height of cells. */
	  char *p = pixel;
	  unsigned int *h = hts;

	  xb = xbin;
	  for(int i = 0; i < nchan; i++) { /* Loop over heights table. */
	    int xl = xstart[xb] - (int)x0;
	    int xh = xend[xb]   - (int)x0;
     	    for(int x = xl; x <= xh; x++) {
	      p[x] = *h;
	    }
	    xb++;
	    h++;
	  }
	  pixel += (short)image->width;
	  
	}
	xbin = xb;
      }
    }
};	       

/*   Color intensity plot:    */

class DrawColor2f : public Drawer2 {
 protected:
    Display *display;		/* Display data structure */
    Visual  *vis;		/* Visual data struct. */
    Drawable win;		/* Window into which we're drawing  */
    GC       context;		/* Graphical context for draws. */
    char *imgdata;	/* Data for image. */
    XImage        *image;	/* Image data structure for X-11 */
    Boolean  eightbit;
  public:
    DrawColor2f(Display *d, Drawable pm, Window w, GC c,
	       float xw, float yw, int xp, int yp, short ox, short oy, int fs) :
		 Drawer2(xw, yw, xp, yp, ox, oy, fs) {
	display = d;
	win     = pm;
	context = c;
	vis     = Xamine_GetVisual(display, w );
	int depth = Xamine_ColorDepth();
	int bppix = (depth+7)/8;
        bppix     = ((bppix % 2) == 0) ? bppix : bppix + 1;
	imgdata = new char[(int)(y0 + 1.0) * (int)nx * (bppix)];
	image   = XCreateImage(display, vis, depth,
			       ZPixmap, 0, imgdata,
			       (int)nx,  (int)y0 + 1, 8, 0);
	/* We can be more efficient than X-11 if depth = 8  */

	eightbit = False;
	if( (depth == 8)) {
	  eightbit = True;
	  image->f.put_pixel = Xamine_PutPixel8;
	}
      }
    virtual ~DrawColor2f() {
      XPutImage(display, win, context, image, 0,0, (int)x0,0,
		(int)nx, (int)ny);
      image->data = NULL;
      delete []imgdata;
      XDestroyImage(image);
    }
    virtual void next()  { nextcol(); }
    virtual int done()  { return donex(); }

    virtual void drawchan(unsigned int height) {
      int yb = yend[ybin];	     /* Y part of rectangle starts here. */
      int ym = ystart[ybin];
      int xb = xstart[xbin] - (int)x0; /* X extent of rectangle translated  */
      int xm = xend[xbin]   - (int)x0; /* into image coordinates.           */
      
      int   pct = (height * 100 )/ full_scale;
      if((pct == 0) && (height != 0)) pct = 1;
      unsigned long pixel = Xamine_PctToPixel(pct);

      for(int j = yb; j <= ym; j++)
	for(int i = xb; i <= xm; i++)
	XPutPixel(image, i, j, pixel);
      
      ybin++;
    }
    /* Restriction -- all must fall in the same vertical scan line. */

    virtual void drawchans(unsigned int *hts, int nchan) {
      if(!eightbit) {
         Drawer2::drawchans(hts, nchan);
      }
      else {
	int xb   = xstart[xbin];
	int numx = xend[xbin] - xb;
	xb -= (int)x0;		/* Translate to image coords. */

	int ynow = ystart[ybin];
	char *pixel = imgdata + ynow*((short)image->width) + xb;
	int  width  = (short)image->width;

	Xamine_HtsToColor(hts, full_scale, nchan);

	for(int i = 0; i < nchan; i++) {
	  int yb = yend[ybin];
	  int yn = ystart[ybin] - yb;
	  for(int y = 0; y < yn; y++) {
	    char *p = pixel;
	    for(int x = 0; x < numx; x++) {
	      *p++ = *hts;
	    }
	    pixel -= width;
	  }
	  hts++;
	  ybin++;
	}
      }
    }

};	       

/*
** The context block below describes where in a plot the drawing of the
** plot was broken off to poll the event queue.  Enough information is
** included to support a restart of the draw.
*/
struct Draw2dContext {

  /* Drawing location context.  
  ** Coordinates are spectrum coordinates.
  */
  float x,y;			/* Location in X of last plot. */
  float xstep,ystep;		/* step s in x/y. */
  float xl,yl;			/* Last x,y. */

  /* This segment of the context keeps track of spectrum attributes,
  ** the drawer and the sampler.
  */

  Sampler2 *sampler;		/* Gets spectrum channels  */
  Drawer2 *drawer;		/* Pointer to the drawing class. */
  win_2d  *attributes;		/* Attributes of the display. */
  Display *d;			/* Display we're connected to. */
  GC      gc;			/* Graphical context, saved for release. */
  
};

/*
** local storage:
*/
static int pixwidtbl[] = { SCATT2D_PIXELS,
			   BOXES2D_PIXELS,
			   COLOR2D_PIXELS,
			   CONTOUR2D_PIXELS,
			   SURFACE2D_PIXELS,
			   LEGO2D_PIXELS
			 };

/*
** Functional Description:
**  DeleteSegmentContext:
**     This local function is supplied as a call in the refresh context data
**     structure.  It is responsible for deleting dynamically allocated
**     data and objects that are represented in the segment structure.
**     It can be called in two cases:
**     1. Directly from Xamine_Plot2d when drawing is done.
**     2. From the refreshctl.cc module through a pointer to it in
**        the refresh context block when a plot is being aborted either
**        due to a resize or due to a change in the plot that is to be
**        shown.
**     This rigamarole is necessary to ensure that resizes and so on don't
**     cause resource leakages.
** Formal Parameters:
**    XtPointer p:
**      Actually a pointer to a Draw2dContext data structure which 
**      is in the process of being deleted.
*/
static void DeleteSegmentContext(XtPointer p)
{
  Draw2dContext *ctx = (Draw2dContext *)p;

  /* Delete the objects */

  delete ctx->drawer;
  delete ctx->sampler;

  /* Free the context itself. */

  delete ctx;

}

/*
** Functional Description:
**   DrawSegment:
**     This local function is responsible for drawing a segment of the plot.
**     by convention, we draw one scan line worth of stuff before returning.
**     note that the coordinate rectangle used in the context block is 
**     (0,0)-(nx,ny) so we don't need a saved origin.
** Formal Parameters:
**    struct Draw2dContext *c:
**      Pointer to the drawing context which we use to keep track of where
**      we are in the draw from segment to segment.
** Returns:
**      TRUE  - Done with the entire plot.
**      FALSE - Not yet done with the plot.
*/
static Boolean DrawSegment(Draw2dContext *c)
{
  /* get all the floor/ceiling stuff for height cuts. */

  int chans = c->sampler->getsteps();

  unsigned int floor = 0, ceiling;
  int hasceiling = c->attributes->hasceiling();
  ceiling = hasceiling ? c->attributes->getceiling() :
                         c->attributes->getfsval();
  if(c->attributes->hasfloor()) floor = c->attributes->getfloor();
  ceiling -= floor;		/* Adjust ceiling relative to floor. */

  //  unsigned int values[chans];
  std::unique_ptr<unsigned int> pvalues(new unsigned int[chans]);
  unsigned int*  values = pvalues.get();

  c->sampler->getscanline(values);
  for(int steps = 0; steps < chans; steps++) {
    unsigned int val = values[steps];
    if(val < floor) { val = 0; } /* Don't draw if below floor. */
    else {
      val -= floor;		/* Remove floor and.. */
      if(val > ceiling)		/* Adjust if above ceiling depending */
	val = hasceiling ?	/* Depending on if this is fs or ceiling: */
	                   0 :	/* Above ceiling removes channel... */
			   ceiling; /* Above fsval just pegs to fsval */
    }				/* val is now plottable channel... */
    values[steps] = val;
  }
  c->drawer->drawchans(values, chans);
  c->drawer->next();
  return c->drawer->done();


}


/*
** Functional Description:
**   DrawLogSegment:
**     This local function is responsible for drawing a segment of the plot.
**     when the counts scale is in log mode.
**     by convention, we draw one scan line worth of stuff before returning.
**     note that the coordinate rectangle used in the context block is 
**     (0,0)-(nx,ny) so we don't need a saved origin.
** Formal Parameters:
**    struct Draw2dContext *c:
**      Pointer to the drawing context which we use to keep track of where
**      we are in the draw from segment to segment.
** Returns:
**      TRUE  - Done with the entire plot.
**      FALSE - Not yet done with the plot.
*/
static Boolean DrawLogSegment(Draw2dContext *c)
{

  /* get all the floor/ceiling stuff for height cuts. */

  unsigned int floor = 0, ceiling;
  int nsteps = c->sampler->getsteps();
  int hasceiling = c->attributes->hasceiling();
  ceiling = hasceiling ? c->attributes->getceiling() :
                         c->attributes->getfsval();
  if(c->attributes->hasfloor()) floor = c->attributes->getfloor();
  ceiling -= floor;		/* Adjust ceiling relative to floor. */

  //  unsigned int vals[nsteps];
  std::unique_ptr<unsigned int> pvals(new unsigned int[nsteps]);
  unsigned int* vals = pvals.get();

  c->sampler->getscanline(vals);
  for(int steps = 0; steps < nsteps; steps++) {
    unsigned int val = vals[steps];
    if(val < floor) { val = 0; } /* Don't draw if below floor. */
    else {
      val -= floor;		/* Remove floor and.. */
      if(val > ceiling)		/* Adjust if above ceiling depending */
	val = hasceiling ?	/* Depending on if this is fs or ceiling: */
	                   0 :	/* Above ceiling removes channel... */
			   ceiling; /* Above fsval just pegs to fsval */
    }				/* val is now plottable channel... */
    if(val > 0) {
      if(val == 1)   {          // Special case to display 1 count channels.
        val = LOG_RESOLUTION/2;
      } else {
         val = (int)(log10((double)val)*LOG_RESOLUTION); /* Log scale it. */
      }
    }
    else {
      val = 0;
    }
    vals[steps] = val;
  }
  c->drawer->drawchans(vals, nsteps);
  c->drawer->next();
  return c->drawer->done();
}

/*
** Functional Description:
**   ChooseDrawer:
**     Using the attributes block of the pane, this function selects a 
**     2-d channel plotter, instantiates it and returns a pointer to it as
**     a generic Drawer2 object.  This allows generic plotting to be
**     done without requiring knowledge of the channel representation.
** Formal Parameters:
**   Display *d:
**     X-11 display pointer.
**   Drawable pm:
**     The place we'll actually draw into (to support pixmap draws with
**     backing store genration.
**
**   Window win:
**     The window into which we will draw... needed to get the visual.
**   GC gc:
**     X-11 graphical context.
**   win_2d *at:
**     Pointer to the pane attributes. We get the rendition and full scale
**     and cuts from here.
**   int orgx,orgy:
**     The pixel coordinates of the plot area origin (X-11 coordinates).
**   int nx, ny:
**      Number of screen pixels available for display.
**   float xw, yw:
**      Channel widths in pixels.
** Returns:
**    Drawer2 *  - Pointer to the drawer selected.
*/
static Drawer2 *ChooseDrawer(Display *d, Drawable pm, Window win, GC gc,
			     win_2d *at, int orgx, int orgy, int nx, int ny,
			     float xw, float yw)
{
  rendition_2d r = at->getrend();


  /*
  ** Figure out the full scale from the full scale value and the height cuts:
  */
  int fs = at->getfsval();
  if(!at->islog()) {
    if(at->hasceiling()) fs = at->getceiling(); /* Peg to ceiling value. */
    if(at->hasfloor())  fs -= at->getfloor(); /* If floor, reduce by the offset. */
  }
  else {
    double logmax, logbase, loginterval;
    logbase = 0.0;		   /*  Get the basic scaling values. */
    logmax  = 1.0;
    if(fs > 0) logmax = log10((double)fs); /* for the floor and ceiling. */
    if(at->hasceiling() && (at->getceiling() > 0)) {
      logmax = log10((double)at->getceiling());
    }
    if(at->hasfloor() && (at->getfloor() > 0)) {
      logbase = log10((double)at->getfloor());
    }
    /* get the decade range:   */
    
    logbase = (double)((int)logbase);
    logmax  = (double)((int)logmax);
    loginterval = logmax - logbase + 1.0;
    if(loginterval < 1.0) loginterval = 1.0;
    fs = (int)(loginterval * LOG_RESOLUTION); /* Scale good to .01 decade */

  }
  
  /*
  ** Next, using the switch statement below, select and instantiate the
  ** appropriate drawer:
  */
  Drawer2 *drawer;
  switch(r) {
  case color:
    /* This one's a bit tricky.  If a user created some windows on a Color
    ** tube, then the rendition might be color, but we might be displaying
    ** on a B/W tube.  In that case, we must default and alter the
    ** representation to a B/W representation (in this case scatter.).
    ** to keep us from dying horrible deaths.
    */
    if(Xamine_ColorDisplay()) {
      if(at->isflipped()) {
	drawer = (Drawer2 *)new DrawColor2f(d, pm, win, gc,
					    xw, yw, nx, ny,
					    (short)orgx, (short)orgy,
					    fs);
      }
      else {
	drawer = (Drawer2 *)new DrawColor2(d, pm, win, gc,
					   xw, yw, nx, ny,
					   (short)orgx, (short)orgy,
					   fs);
      }
      break;
    }
    else {			/* If black/white, default to boxes. */
      at->scatterplot();
    }
/*
**   Do not insert cases here as this will disturb the logic for 
**   rendering 2-d's from color windows onto B/W tubes.
*/

  case scatter:
    if(at->isflipped()) {
      drawer = (Drawer2 *)new DrawScatter2f(d, pm, gc, xw,yw, nx, ny,
					    (short)orgx, (short)orgy,
					    fs);
    }
    else {
      drawer = (Drawer2 *)new DrawScatter2(d, pm, gc, 
					   xw,yw, nx, ny,
					   (short)orgx, (short)orgy,
					   fs);
    }
    break;
  case contour:
  case lego:
    fprintf(stderr, "Rendition not supported, defaulting to something suitable\n");
  case boxes:    
    if(at->isflipped()) {
      drawer = (Drawer2 *)new DrawRect2f(d, pm, gc,
					 xw,yw, nx, ny, 
					 (short)orgx, (short)orgy,
					 fs);
    }
    else {
      drawer = (Drawer2 *)new DrawRect2(d,pm,gc, 
					xw,yw, nx, ny,
					(short)orgx, (short)orgy,
					fs);
    }
    break;
  default:
    fprintf(stderr, "*BUG* ChoseDrawer... invalid rendition type %d\n", r);
    exit(-1);
  }
  return drawer;
}

/*
** Functional Description:
**    Xamine_Plot2d:
**      This function oversees the plotting of the 2-d spectrum.
**      it must select an appropriate sampler and plotter and then
**      it must initiate the plot.  Note that some of the more complex
**      plots don't have a plotter since drawing a single channel
**      may not be independent of drawing other channels in the spectrum
**      (consider e.g. contour plots).
** Formal Parameters:
**    Screen  *s,
**    Display *d:
**       Screen and display pointers which indicate which X-server is to
**       receive the plot.
**    win_attributed *att:
**       The display attributes of the spectrum.
**    Drawable win:
**       The window to draw in.
**    int orgx, int orgy:
**       The 0,0 position (note that Y increases going down the tube).
**    int nx, ny:
**       Number of pixels in the x and y direction.
**    Xamine_RefreshContext *context:
**       The update context.  Note that if the plot update is segmented,
**       we will be saving context information linked to the ctx field of
**       the refresh context.
** Returns:
**     TRUE -- The update is done.
**     FALSE - The update is still in progress and we need to be re-called.
*/
Boolean Xamine_Plot2d(Screen *s, Display *d, 
		      win_attributed *att, Drawable win, XMWidget *pane,
		      int orgx, int orgy, int nx, int ny,
		      Xamine_RefreshContext *context)
{
  win_2d *at2  = (win_2d *)att;

  if(context->ctx == NULL) {
    Draw2dContext *ctx;

    
    GC             gc = Xamine_MakeDrawingGc(d, pane, -1);

    /* No drawing context exists yet, so we must build one. */

    ctx = new Draw2dContext;	/* Create a context. */
    ctx->attributes = at2;	/* Display attributes. */
    ctx->x = 0.0;
    ctx->y = 0.0;
    ctx->xl= (float)nx;
    ctx->yl= (float)ny;

    ctx->gc= gc;		/* Save graphical context and */
    ctx->d = d;			/* display. */
    /* Figure out the area of interest: */
    int spno = at2->spectrum();

    int xl = 0;
    int yl = 0;
    int xh = xamine_shared->getxdim(spno)-1;
    int yh = xamine_shared->getydim(spno)-1;

    /*   Generate the sampler : */

    if(at2->isflipped()) {
      if(at2->isexpanded()) {
	xl = (at2->isexpandedfirst() ? at2->xlowlim() : at2->ylowlim());
	xh = (at2->isexpandedfirst() ? at2->xhilim()  : at2->yhilim());
	yl = (at2->isexpandedfirst() ? at2->ylowlim() : at2->xlowlim());
	yh = (at2->isexpandedfirst() ? at2->yhilim()  : at2->xhilim());
      }
      ctx->sampler = 
	Xamine_GenerateSampler2(spno, xamine_shared->gettype(spno),
				at2->getrend(), at2->getreduction(),
				xl, xh, yl, yh, ny, nx,
				&ctx->ystep, &ctx->xstep);
    }
    else {
      if(at2->isexpanded()) {
	xl = (at2->isexpandedfirst() ? at2->xlowlim() : at2->ylowlim());
	xh = (at2->isexpandedfirst() ? at2->xhilim()  : at2->yhilim());
	yl = (at2->isexpandedfirst() ? at2->ylowlim() : at2->xlowlim());
	yh = (at2->isexpandedfirst() ? at2->yhilim()  : at2->xhilim());
	//	if(at2->ismapped()) {
	// xh++;
	//  }
      }
      ctx->sampler = 
	Xamine_GenerateSampler2(spno, xamine_shared->gettype(spno),
				at2->getrend(), at2->getreduction(),
				xl, xh, yl, yh, nx, ny,
				&ctx->xstep, &ctx->ystep);
    }
    /* Choose the drawer */

    ctx->drawer = ChooseDrawer(d, win,
			       XtWindow(pane->getid()), gc, at2, orgx, orgy,
			       nx,ny,	/* Chose the drawer */
			       ctx->xstep, ctx->ystep);

    /* Link the segment context to the work proc's context and also point
    ** to the segment context destroyer so that plots can be aborted.
    */
    context->ctx = (void *)ctx;	
    context->destroyer = DeleteSegmentContext;

    return False;		/* That's enough work for the first time. */
  }
  else {
    Boolean plot_done;
    Draw2dContext *ctx = (Draw2dContext *)context->ctx;
    time_t entry_time = time(NULL);
    /*
    ** To balance between user interface responsiveness and 
    ** reduction of work procedure activation overhead, we'll loop over
    ** scan lines at most 1 second stopping if done of course
    */
    if(ctx->attributes->islog()) {
      while((!(plot_done = DrawLogSegment(ctx))) && (entry_time == time(NULL)))
	;
    }
    else {
      while((!(plot_done = DrawSegment(ctx))) &&(entry_time == time(NULL)))
	;
    }
    if(plot_done) {	/* Done drawing... */
      DeleteSegmentContext((XtPointer)ctx); /* Kill off the segment. */
      context->destroyer = NULL; /* Remove the destroyer reference. */
      context->ctx = NULL;	    /* Nulling it out prevents caller from */
      return True;		/* deleting.  Return true to indicate done */
    }
    return False;		/* Return False to indicate more to do. */
  }
}





