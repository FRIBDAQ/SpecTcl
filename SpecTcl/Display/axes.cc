/*
** Facility:
**   Xamine  - NSCL Display program.
** Abstract:
**   axes.cc:
**     This file contains code to draw Axes on an Xwindow.  The window
**     is assumed to be entirely dedicated to the spectrum.  The axes are
**     drawn offset from the left and bottom of the window by
**     XAMINE_MARGINSIZE fraction of the window size.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)axes.cc	8.2 10/3/95 \n";


/*
** Include files required:
*/
#include <math.h>
#include <assert.h>
#include "XMWidget.h"
#include "XBatch.h"
#include "axes.h"
#include "dispwind.h"
#include "dispshare.h"
#include "text.h"
#include "gcmgr.h"

/*
** Externally referenced globals:
*/

extern spec_shared *xamine_shared;

/*
** Functional Description:
**  ComputeLinearTickInterval:
**    Computes the number of pixels between tick marks on a linear axis.
** Formal Parameters:
**   int paramrange:
**     Range of parameters represented by the axis.
**   int pixels:
**     Number of pixels available.
* Returns:
**   The parameter interval between ticks.
*/
static int ComputeLinearTickInterval(unsigned int paramrange, int pixels)
{
  /*  Compute the 'tight' packed tick interval... */

  double tickmin = paramrange / XAMINE_TICK_MAXCOUNT;  /* Pack ticks tightly */
  int    tickpix = pixels/XAMINE_TICK_MAXCOUNT;        /* Pixels per tick    */
  int    ntick   = XAMINE_TICK_MAXCOUNT;               /* Number of ticks    */

  while( (tickpix < XAMINE_TICK_MINSPACE) && (ntick > 1) ) {
    ntick--;
    tickmin = paramrange / ntick;
    tickpix = pixels/ntick;
  }
  

  if(tickmin <= 0.0) tickmin = 1.0; // Don't take log10(0).
  double tickdec = log10(tickmin);  // # decades between ticks.
  int    itickdec = (int)tickdec;
  double tickmult  = pow(10.0, (double)itickdec); // # 0's after each tick.
  tickdec          = tickdec - (double)itickdec; // Fractional part of tight.
  double tickmant  = pow(10.0, tickdec);	  // Mantissa for tight pack.
  
  /* Here then:  tick mult is the exponent of the tick spacing. 
   ** tickmant:   is the mantissa of the tick spacing, however it's not
   ** yet one of the 'natural' intervals 1, 2, or 5.   The natural interval
   ** is the smallest of these which is larger than tickmant:
  */
  
  if( 1.0 > tickmant) {
    return (int)tickmult;
  }

  if( 2.0 > tickmant) {
    return (int)(tickmult*2.0);
  }

  if(5.0 > tickmant) {
    return (int)(tickmult*5.0);
  }

  return (int)(10.0*tickmult);		// Only possible if bad rounding problems.
}

/*
** Functional Description:
**   DrawAxes:
**     This function draws the axis lines.
** Formal Parameters:
**   Display *d:
**     Display pointer for the draw.
**   Window  w:
**     Window to draw in.
**   GC gc:
**     Graphical context handle.
**   int xbase, ybase:
**      Origin of axes.
**   int nx,ny:
**      end coordinates of the axes.
*/
static void DrawAxes(Display *d, Window w, GC gc,
		     int xbase, int ybase,
		     int nx, int ny)
{
  XPoint axis[3];		/* 3 points define the axis. */

  axis[0].x = xbase;		/* Upper left point. */
  axis[0].y = 0;

  axis[1].x = xbase;		/* Origin */
  axis[1].y = ybase;

  axis[2].x  = nx;		/* Lower right point. */
  axis[2].y  = ybase;

  /* Draw the axis lines: */

  XDrawLines(d, w, gc, axis, 3, CoordModeOrigin);

}

/*
** Functional Description:
**   DrawLinearXTicks:
**     This function draws a linearly scaled set of tick marks on the
**     X axis.  Tick mark lengths are some fixed fraction of the drawing region
**     so that they scale with size of the draw region.
**     In addition there is some sort of complex formula used to figure out
**     how often to draw a tick mark.  The formula is based on both the
**     range of values represented and the space available (e.g. there's a 
**     minimum distance between ticks enforced).
** Formal Parameters:
**  Display *disp:
**     The display that we're drawing on.
**  Window win:
**     The window we draw into.
**  GC gc:
**     The graphical context which determines the manner in which we draw.
**  int xbase,ybase:
**     The position of the axis origin.
**  int nx,ny:
**     The number of pixels in each direction on the axes.
**  int low,hi:
**     The range to be represented.
**   BOOL label_ticks:
**     True if ticks are supposed to have a label on them.
*/
static void DrawLinearXTicks(Display *disp, Window win, GC gc,
			     int xbase, int ybase, int nx, int ny,
			     unsigned int low, unsigned int hi, 
			     Boolean label_ticks)
{
  float interval;			/* Number of pixels between ticks. */
  int height;			/* Height of a tick. */
  float value_represented;	/* The value represented by a tick. */
  float value_interval;		/* The tick interval in value units. */
  int   last_value;
  float xb = (float)xbase;
  char label[20];

  last_value        = -1;	/* Make sure the first tick is drawn. */
  value_represented = (float)low;

  

  XSegmentBatch ticks(disp, win, gc);

  value_interval = (float)ComputeLinearTickInterval(hi-low + 1, (nx-xbase+1));


  interval       = ((float)(nx-xbase+1) * (float)value_interval)/
                    ((float)(hi - low + 1));
  height   = (int)(ybase * XAMINE_TICK_FRACTION);

  
  int labelheight  = (int)((float)(ny)*XAMINE_MARGINSIZE/2.0);
  int labelwidth   = (int)((float)(interval)/XAMINE_TICK_LABEL_INTERVALDIVISOR)*2;
  XFontStruct *font = NULL;	/* Start will NULL font .. no draw action */
  if(label_ticks) {		/* Choose the font  */
    sprintf(label, "%d", hi);	/* Biggest string we can represent. */
    font = Xamine_SelectFont(disp, label, labelwidth, labelheight);
  }


  while(xbase < nx) {
    if( (int)value_represented != last_value) {
      last_value = (int)value_represented;
      ticks.draw(xbase,ybase+2, xbase,ybase-height);
      if((label_ticks) && (font != NULL)) {
	sprintf(label, "%d", (int)value_represented);
	Xamine_DrawCenteredString(disp, win, font, gc,
				  (xbase - labelwidth/2), (ybase + labelheight),
				  (xbase + labelwidth/2), (ybase),
				  label);
      
      }
    }
    value_represented += value_interval;
    xb += interval;
    xbase = (int)(xb + 0.5);
  }
  ticks.flush();
}

/*
** Functional Description:
**   DrawLogXTicks:
**    This function draws tick marks for logarithmic X axes.  Our strategy
**    is to first draw all Decade ticks.  Then while the space between any
**    two adjacent ticks would be greater than XAMIN_TICK_MINSPACE, draw
**    the multiplier ticks.
**    This algorithm relaxes a requirement of at most XAMINE_TICK_MAXCOUNT
**    ticks on the axis in favor of more sensible tick marks.
** Formal Parameters:
**   Display *disp:
**     Pointer to the display that we must draw on.
**   Window  win:
**     Handle to the window that we're drawing ticks into.
**   GC gc:
**     Handle to the graphical context that determines the style with
**     which we draw.
**   int xbase,ybase:
**     The location of the origin. Remember that Y increases downards in
**     X-windows coordinates.
**   int nx,ny:
**     The number of pixels in the display window.  This includes the margin
**     since we need to know how to draw ther axis labels.
**   int low,hi:
**     The floor and ceiling.  Note that for log displays, we only draw
**     full decades.
**   Boolean label_ticks:
**     True if labels are to be drawn for the decade ticks.  Note that
**     we never draw labels for the multiplier ticks.
*/
static void DrawLogXTicks(Display *disp, Window win, GC gc,
			  int xbase, int ybase, int nx, int ny,
			  unsigned int low, unsigned  int hi, 
			  Boolean label_ticks)
{
  int interval;
  int tick;			/* Location of current tick on axis. */
  int low_decade;		/* Low decade exponent. */
  int high_decade;		/* High decade exponent. */
  int low_pos;			/* Position of tick below. (first decade) */
  int hi_pos;			/* Position of tick above. (first decade) */
  int height;
  XSegmentBatch ticks(disp, win, gc); /* buffered ticks. */
  int i;
  char label[20];

  /* figure out the full decade range.  The low decade is truncated to 
  ** display the whole decade, while the high decade is, if necessary,
  ** bumped up a bit to display the entire decade contained by the full 
  ** scale
  */

  if(low > 0) {
    low_decade = (int)log10((double)low);
  }
  else {
    low_decade = 0;
  }
  if(hi > 0) {
    double hd  = log10((double)hi);
    high_decade = (int)hd;
  }
  else {
    high_decade = 1; 
  }


  /* Decades are at fixed intervals... that's what log scale means */

  interval = (nx - xbase)/(high_decade - low_decade + 1);
  low_pos = xbase;
  hi_pos  = xbase+interval;	/* Set the low/hi for initial multipliers */

  /* Now draw the decade markers: */

  tick = xbase;
  height = (int)(ybase * XAMINE_TICK_FRACTION);
  int labelheight = (int)((float)ny*XAMINE_MARGINSIZE/2.0);
  int labelwidth  = (int)((int)(interval)/XAMINE_TICK_LABEL_INTERVALDIVISOR)*2;
  XFontStruct *font = NULL;
  if(label_ticks) {
    sprintf(label, "1E%d", high_decade);
    font = Xamine_SelectFont(disp, label, labelwidth, labelheight);
  }
  for(i = low_decade; i <= high_decade; i++) {
    ticks.draw(tick, ybase+2, tick, ybase-height);
    if(label_ticks && (font != NULL)) {
      sprintf(label, "1E%d", i);
      Xamine_DrawCenteredString(disp, win, font, gc,
				tick-(labelwidth/2), ybase+(labelheight),
				tick+(labelwidth/2), ybase, label);

    }
    tick += interval;
  }

  /* Now we draw the fractional decades beginning with .1 and
  ** working by .1 intervals
  */

  height = height/2;
  for(i = 2; i <= 9; i++) {
    int j;
    float multiplier = (float)i; /* This is the multiplier. */

    tick = xbase + (int)(((double)interval) * log10((double)multiplier));
    for(j = low_decade; j <= high_decade; j++) {
      ticks.draw(tick, ybase+2, tick, ybase-height);
      tick += interval;
    }

  }
  ticks.flush();
}
/*
** Functional Description:
**  DrawLinearYTicks:
**    This function draws a set of ticks on a linear y axis.
** Formal Parameters:
**    Display *disp:
**        The display on which the window is pasted.
**    Window win:
**        The window id of the window in which to draw the ticks.
**    GC gc:
**        The graphics context which describes how to draw the ticks.
**    int xbase, ybase:
**        The coordinates of the origin of the axes.  Note that 
**        the Y origin is at the top of the window (I usually forget this).
**    int nx, ny:
**        The number of x and y pixels in the display window.  This includes
**        the margins (we need to know this to draw labels on the ticks.
**    int low, hi:
**        The range of values expressed by the axis.
**    Boolean label_ticks:
**        True if the tick marks are to be labelled.
*/
static void DrawLinearYTicks(Display *disp, Window win, GC gc,
			     int xbase, int ybase, int nx, int ny,
			     unsigned int low,unsigned int hi, 
			     Boolean label_ticks)
{
  float value_represented, value_interval;
  float interval;
  float   yb = (float)ybase;
  int height;
  int last_value;

  XSegmentBatch ticks(disp, win, gc);
  char label[20];

  value_represented = (float)low;
  last_value        = -1;	/* Make sure first tick is drawn. */

  value_interval = (float)ComputeLinearTickInterval(hi - low +1, ybase+1);


  interval       = ((float)((ybase + 1) * value_interval))/ 
                   ((float)( hi-low + 1));


  height   = (int)((nx-xbase)*XAMINE_TICK_FRACTION);

  int labelheight = (int)((float)(2*interval)/
			  XAMINE_TICK_LABEL_INTERVALDIVISOR);
  int labelwidth  = xbase;
  XFontStruct *font = NULL;
  if(label_ticks) {
    sprintf(label, "%d", hi);
    font = Xamine_SelectFont(disp, label, labelwidth, labelheight);
  }
  while(ybase > 0) {
    if(last_value != (int)value_represented) {
      last_value = (int)value_represented;
      ticks.draw(xbase-2,ybase, xbase+height,ybase);
      if(label_ticks && (font != NULL)) {
	sprintf(label, "%d", (int)value_represented);
	Xamine_DrawCenteredString(disp, win , font, gc,
				  0, (ybase + labelheight/2),
				  labelwidth, (ybase - labelheight/2),
				  label);
      }
    }
    value_represented += value_interval;
    yb    -= interval;
    ybase  = (int)(yb + 0.5);

  }
  ticks.flush();
}

/*
** Functional Description:
**   DrawLogYTicks:
**    This function draws tick marks on a Y log scale axis.
** Formal Parameters:
**   Display *disp:
**      The pointer to the display connection structure.
**   Window win:
**      The window which the ticks should be draw on.
**   GC gc:
**     The graphical context to use for the draw.
**   int xbase,ybase:
**     The origin position.
**   int nx,ny:
**     The number of pixels available
**   int low, int hi:
**      The range of real values represented.
**   Boolean label_axes:
**      True if the decade tick marks should be labelled.
*/

static void DrawLogYTicks(Display *disp, Window win, GC gc,
			  int xbase, int ybase, int nx, int ny,
			  unsigned int low, unsigned int hi, 
			  Boolean label_axes)
{
  int interval; 		/* interval between successive ticks */
  int tick;			/* Location of current tick on axis. */
  int low_decade;		/* Low decade exponent. */
  int high_decade;		/* High decade exponent. */
  int low_pos;			/* Position of tick below. (first decade) */
  int hi_pos;			/* Position of tick above. (first decade) */
  int height;
  XSegmentBatch ticks(disp, win, gc); /* buffered ticks. */
  int i;
  char label[20];

  /* figure out the full decade range.  The low decade is truncated to 
  ** display the whole decade, while the high decade is, if necessary,
  ** bumped up a bit to display the entire decade contained by the full 
  ** scale
  */
  if(low > 0) {
    low_decade = (int)log10((double)low);
  }
  else {
    low_decade = 0;
  }
  if(hi > 0) {
    double hd  = log10((double)hi);
    high_decade = (int)hd;
  }
  else {
    high_decade = 1; 
  }

  /* Decades are at fixed intervals... that's what log scale means */

  interval = (ybase-1)/(high_decade - low_decade + 1);
  low_pos = ybase;
  hi_pos  = ybase-interval;	/* Set the low/hi for initial multipliers */

  /* Now draw the decade markers: */

  tick = ybase;
  height = (int)((nx - xbase) * XAMINE_TICK_FRACTION);
  int labelwidth = xbase;
  int labelheight = (int)((interval*2)/XAMINE_TICK_LABEL_INTERVALDIVISOR);
  XFontStruct *font = NULL;
  if(label_axes) {
    sprintf(label, "1E%d", high_decade);
    font = Xamine_SelectFont(disp, label, labelwidth, labelheight);
  }
  for(i = low_decade; i <= high_decade; i++) {
    ticks.draw(xbase-2, tick, xbase+height,tick);
    if(label_axes && (font != NULL)) {
      sprintf(label, "1E%d", i);
      Xamine_DrawCenteredString(disp, win, font, gc,
			      0, tick + labelheight/2,
			      xbase, tick - labelheight/2,
			      label);
    }
    tick -= interval;
  }

  /* Now we draw the fractional decades beginning with .1 and
  ** working by .1 intervals
  */

  height = height / 2;
  for(i = 2; i <= 9; i++) {
    int j;
    float multiplier = (float)i; /* This is the multiplier. */

    multiplier = ((double)interval * log10((double)(multiplier)));
    tick = ybase - (int)multiplier;
    for(j = low_decade; j <= high_decade; j++) {
      ticks.draw(xbase-2, tick, xbase+height,tick);
      tick -= interval;
    }

  }
  ticks.flush();
}

/*
** Functional Description:
**   Xamine_DrawAxes:
**     This function draws the axes of a histogram.  Note that in order
**     to draw axes labels, the scaling is computed (if autoscaling is on).
**     The scaling will be stored in the scaling cell of the attributes block
**     so that later rendering of the histogram can be done correctly.
**     Drawing is done with a graphical context that has had the foreground
**     set to the widget foreground color and the background set to the
**     widget background colors.  All other items default from the
**     initial values of a created graphical context.
** Formal Parameters:
**     Xamine_RefreshContext *ctx:
**       Redraw context.
**     win_attributed *attribs:
**       Attributes of the display.
*/
void Xamine_DrawAxes(Xamine_RefreshContext *ctx, win_attributed *attribs)
{

  
  /* Get the screen window and graphical context for the drawing.
  ** We use the foreground color in the resources of the target widget
  ** as the drawing color.  We also set the background from that widget as well.
  */
  XMWidget *pane = ctx->pane;
  Display *disp;
  Screen  *screen;
  Window  win;
  GC      gc; 
  XamineTextGC *context;
  XGCValues gc_values;

  if(!attribs->showaxes()) return; /* NO-OP if axes turned off. */

  disp   = XtDisplay(pane->getid());
  screen = XtScreen(pane->getid());
  win    = ctx->pixmap;

  Pixel  background;

  context = Xamine_GetTextGC(*pane);
  gc      = context->gc;	// for now do this the sleazy way.

  /* 
  ** Get the size of the window and figure out the axis origint.
  */
  Dimension nx, ny;
  Dimension xbase, ybase;

  pane->GetAttribute(XmNwidth, &nx);
  pane->GetAttribute(XmNheight, &ny);

  xbase = (int)((float)nx * (float)XAMINE_MARGINSIZE);
  ybase = ny - (int)((float)ny * (float)XAMINE_MARGINSIZE);

  /* Draw the axis lines: */

  DrawAxes(disp, win, gc, xbase, ybase, nx,ny);

  /* If the tick marks are visible, then we must draw them too. */
  /* Note that for counts axes ticks, we assume that the full scale */
  /* value (even for autoscaling) has been placed in the pane's attributes */
  /* object.  This is done by calls to full scale determination routines in */
  /* in our caller up in refreshctl.cc                                      */

  /* Regardless of the type of spectrum, there is one channels axis. This is */
  /* the X axis for unflipped panes and the Y axis for flipped panes         */

  if(attribs->showticks()) {

  /* Get the limits on the gaurenteed channels axis: */

    unsigned int low, hi;
    low = 0;
    hi  = xamine_shared->getxdim(attribs->spectrum());	/* Assume not expanded.*/
    
    if(attribs->is1d()) {
      win_1d *att = (win_1d *)attribs;
      if(att->isexpanded()) {
	low = att->lowlimit();
	hi  = att->highlimit();
      }
    }
    else {
      win_2d *att = (win_2d *)attribs;
      if(att->isexpanded()) {
	if(att->isflipped()) {
	  low = att->ylowlim();
	  hi  = att->yhilim();
	}
	else {
	  low = att->xlowlim();
	  hi  = att->xhilim();
	}
      }
    }
    if(attribs->isflipped()) {	/* Linear Y axis for X channels: */
      DrawLinearYTicks(disp, win, gc, xbase, ybase, nx,ny, low, hi,
		       attribs->labelaxes());
    }
    else {
      DrawLinearXTicks(disp, win, gc, xbase, ybase, nx,ny, low, hi,
		       attribs->labelaxes());
    }

  /* For 1-d spectra, the other axis is the counts axis and it may be linear
  ** or log.  The low limit is the maximum of 0 and floor, and the upper
  ** limit is the minimum of fullscale and ceiling. For 2-d spectra, the
  ** other axis is another channels axis and just depends on the y limits.
  */
    if(attribs->is1d()) {		/* 1-d counts axis: */
      low = 0;			/* Figure out the low/high limits for axes. */
      hi  = attribs->getfsval();
      if(attribs->hasfloor()) low = attribs->getfloor();
      if(attribs->hasceiling()) {
	int ceil = attribs->getceiling();
	hi = ceil < hi ? ceil : hi;
      }
      if(attribs->islog()) {
	if(attribs->isflipped()) {
	  DrawLogXTicks(disp, win, gc, xbase, ybase, nx,ny, low, hi, 
			attribs->labelaxes());
	}
	else {
	  DrawLogYTicks(disp, win, gc, xbase, ybase, nx,ny, low,hi,
			attribs->labelaxes());
	}
      }
      else {
	if(attribs->isflipped()) {
	  DrawLinearXTicks(disp, win,gc, xbase,ybase, nx,ny, low,hi,
			   attribs->labelaxes());
	}
	else {
	  DrawLinearYTicks(disp, win, gc, xbase,ybase, nx,ny, low,hi,
			   attribs->labelaxes());
	}
      }
    }
    else {			/* 2-d spectrum. Figure out hi/low like X */
      win_2d *att = (win_2d *)attribs;
      low = 0;
      hi  = xamine_shared->getydim(att->spectrum());
      if(att->isflipped()) {
	if(att->isexpanded()) {
	  low = att->xlowlim();
	  hi  = att->xhilim();
	}
	DrawLinearXTicks(disp,win,gc, xbase,ybase, nx,ny, low,hi,
			 att->labelaxes());
      }
      else {
	if(att->isexpanded()) {
	  low = att->ylowlim();
	  hi  = att->yhilim();
	}
	DrawLinearYTicks(disp,win,gc, xbase,ybase, nx,ny, low,hi,
			 att->labelaxes());
      }
    }
    
  /* End of tick drawing */

  }

  
}
