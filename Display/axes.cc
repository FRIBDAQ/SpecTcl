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
#include "mapcoord.h"

/*
** Externally referenced globals:
*/

extern volatile spec_shared *xamine_shared;

/*
** Functional Description:
**  ComputeLinearTickInterval:
**    Computes the number of spectrum units between 
**    tick marks on a linear axis.
** Formal Parameters:
**   int paramrange:
**     Range of parameters represented by the axis.
**   int pixels:
**     Number of pixels available.
* Returns:
**   The parameter interval between ticks.
*/
static unsigned int ComputeLinearTickInterval(unsigned int paramrange, int pixels)
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

static float ComputeMappedTickInterval(float pparamrange, int pixels)
{
  float paramrange = fabsf(pparamrange); // Compute in abs vale.
  float rangesign = pparamrange/paramrange; // +/- 1 depending on sign of pparamrange.
  /*  Compute the 'tight' packed tick interval... */
  int    tickpix = pixels/XAMINE_TICK_MAXCOUNT;        /* Pixels per tick    */
  int    ntick   = XAMINE_MAPPED_TICK_MAXCOUNT;        /* Number of ticks    */

  while((tickpix < XAMINE_MAPPED_TICK_MINSPACE) && (ntick > 1)) {
    ntick--;
    tickpix = pixels/ntick;
  }

  float tickint  = (paramrange / (float)ntick);
  float tickmant = tickint - (int)tickint;

  // If the interval is already a nice one, return it
  if((tickint == 10.0) || (tickint == 5.0) || (tickint == 2.0))
    return tickint;
  else if((tickint == (int)tickint) && ((int)tickint % 10 == 0) && 
	  (ntick < 10) && (ntick >= 6))
    return tickint;
  else if((tickint == (int)tickint) && ((int)tickint % 5 == 0) && 
	  (ntick < 10) && (ntick >= 6))
    return tickint;
  else if((tickint == (int)tickint) && ((int)tickint % 2 == 0) && 
	  (ntick < 10) && (ntick >= 6))
    return tickint*rangesign;
  // Otherwise, see if we can make it a nice one the easy way
  if((paramrange / 10.0 >= 6) && (paramrange / 10.0 < 10)) {
    return 10.0*rangesign;
  }
  else if((paramrange / 5.0 >= 6) && (paramrange / 5.0 < 10)) {
    return 5.0*rangesign;
  }
  else if((paramrange / 2.0 >= 6) && (paramrange / 2.0 < 10)) {
    return 2.0*rangesign;
  }

  // We have to compare the tick mantissa to a percentage of the parameter
  // range, since the tick interval can be a floating point value.
  if(tickmant < (paramrange * .20)) {
    tickint = (int)tickint + 1;
    if(tickint > (paramrange / 2.0)) {
      tickint = paramrange / ntick;
    }
  }

  // If the tick interval is already an integer, then try to make it a nice one
  if((int)tickint == tickint) {
    int tempint = (int)tickint;
    while((10 % tempint != 0) && (tempint % 10 != 0)) {
      int power = (int)log10(tempint);
      int exp   = (int)pow(10.0, power);
      int ones_digit = exp ? (tempint % exp)  : 0; // Gaurd against exp == 0.
      if(ones_digit == 0)
	break;
      tempint -= ones_digit;
    }
    tickint = tempint;
  }

  return tickint*rangesign;
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
			     unsigned low, unsigned int hi, 
			     Boolean label_ticks)
{
  float interval;		/* Number of pixels between ticks. */
  int height;			/* Height of a tick. */
  float value_represented;	/* The value represented by a tick. */
  float value_interval;		/* The tick interval in value units. */
  unsigned int   last_value;
  float xb = (float)xbase;
  char label[20];

  if ((hi - low)  == 0 ) return; // No tick marks if no interval.

  last_value        = 0xffffffff;	/* Make sure the first tick is drawn. */
  value_represented = (float)low;

  

  XSegmentBatch ticks(disp, win, gc);

  value_interval = (float)ComputeLinearTickInterval(hi-low, (nx-xbase+1));

  interval       = ((float)(nx-xbase) * (float)value_interval)/
                    ((float)(hi - low ));
  height   = (int)(ybase * XAMINE_TICK_FRACTION);

  
  int labelheight  = (int)((float)(ny)*XAMINE_MARGINSIZE/2.0);
  int labelwidth   = 
    (int)((float)(interval)/XAMINE_TICK_LABEL_INTERVALDIVISOR)*2;
  XFontStruct *font = NULL;	/* Start will NULL font .. no draw action */
  if(label_ticks) {		/* Choose the font  */
    sprintf(label, "%u", hi);	/* Biggest string we can represent. */
    font = Xamine_SelectFont(disp, label, labelwidth, labelheight);
  }
  if (!font) return;		// Can't get a font so forget it.


  while(xbase < nx) {
    if( (unsigned int)value_represented != last_value) {
      last_value = (unsigned int)value_represented;
      ticks.draw(xbase,ybase+2, xbase,ybase-height);
      if((label_ticks) && (font != NULL)) {
	unsigned long labelValue = (unsigned long)value_represented;
	sprintf(label, "%lu", labelValue);
	Xamine_DrawCenteredString(disp, win, font, gc,
				  (xbase - labelwidth/2), (ybase+labelheight),
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
**   DrawMappedXTicks:
**     This function draws a linearly scaled set of mapped tick marks on the
**     X axis.  Tick mark lengths are some fixed fraction of the drawing region
**     so that they scale with size of the draw region.
**     In addition there is some sort of complex formula used to figure out
**     how often to draw a tick mark.  The formula is based on both the
**     range of values represented and the space available (e.g. there's a 
**     minimum distance between ticks enforced). The range is given in mapped
**     spectrum space.
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
**  float low,hi:
**     The range to be represented.
**   BOOL label_ticks:
**     True if ticks are supposed to have a label on them.
*/
static void DrawMappedXTicks(Display *disp, Window win, GC gc,
			     int xbase, int ybase, int nx, int ny,
			     float low, float hi, char* xlabel,
			     Boolean label_ticks)
{
  float interval;		/* Number of pixels between ticks. */
  int height;			/* Height of a tick. */
  float value_represented;	/* The value represented by a tick. */
  float value_interval;		/* The tick interval in value units. */
  float last_value;
  float xb = (float)xbase;
  char label[20];
  float rem;
  float mant = low - (int)low;

  if ((hi - low) == 0) return;	// If no range then no ticks.

  
  value_represented = low;
  last_value        = low - 100; // Make it different than low.

  XSegmentBatch ticks(disp, win, gc);

  // Compute the mapped tick interval between tick marks
  if((low < 0) && (hi > 0))
    value_interval = (float)ComputeMappedTickInterval((hi-low),(nx-xbase));
  else
    value_interval = (float)ComputeMappedTickInterval(hi-low, (nx-xbase));

  if(value_interval == 0)
    value_interval = 0.2;


  rem = value_interval - (int)value_interval;  
  interval = ((float)(nx-xbase) * value_interval) / (hi-low);
  height   = (int)(ybase * XAMINE_TICK_FRACTION);

  // Now get the size of the biggest string we can represent given our
  // space constraints and choose the appropriate font.
  int labelheight  = (int)((float)(ny)*XAMINE_MAPPED_MARGINSIZE / 3.0);
  int labelwidth   = 
    (int)((float)(interval)/XAMINE_TICK_LABEL_INTERVALDIVISOR)*2;
  XFontStruct *font = NULL;	/* Start will NULL font .. no draw action */
  if(label_ticks) {		/* Choose the font  */
    if((rem != 0.0) || (mant != 0.0)) 
      sprintf(label, "%.2f", hi);	/* Biggest string we can represent. */
    else
      sprintf(label, "%.0f", hi);
    font = Xamine_SelectFont(disp, label, labelwidth, labelheight);
  }
  if (!font) return;

  // Draw the ticks until we run out of space on the spectrum
  while(xbase < nx) {
    if(value_represented != last_value) {
      last_value = value_represented;
      ticks.draw(xbase,ybase+2, xbase,ybase-height);
      if((label_ticks) && (font != NULL)) {
	if((rem != 0.0) || (mant != 0.0)) 
	  sprintf(label, "%.2f", value_represented);
	else
	  sprintf(label, "%.0f", value_represented);
	Xamine_DrawCenteredString(disp, win, font, gc,
				  (xbase - labelwidth/2), (ybase+labelheight),
				  (xbase + labelwidth/2), (ybase),
				  label);
      
      }
    }
    value_represented += value_interval;
    xb += interval;
    xbase = (int)(xb + 0.5);
  }

  // Finally, draw the unit label below the axis
  Xamine_DrawCenteredString(disp, win, font, gc,
			    (nx/2 - labelwidth/2), (ybase+(2*labelheight)),
			    (nx/2 + labelwidth/2), (ybase+labelheight),
			    xlabel);
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
  if (!font) return;

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
  unsigned int last_value;



  XSegmentBatch ticks(disp, win, gc);
  char label[20];

  if ((hi - low) == 0) return;	// no ticks if no interval.

  value_represented = (float)low;
  last_value        = 0xffffffff;	/* Make sure first tick is drawn. */

  value_interval = (float)ComputeLinearTickInterval(hi - low, (ybase+1));


  interval       = ((float)((ybase ) * value_interval))/ 
                   ((float)( hi-low));


  height   = (int)((nx-xbase)*XAMINE_TICK_FRACTION);

  int labelheight = (int)((float)(2*interval)/
			  XAMINE_TICK_LABEL_INTERVALDIVISOR);
  int labelwidth  = xbase;
  XFontStruct *font = NULL;
  if(label_ticks) {
    sprintf(label, "%u", hi);
    font = Xamine_SelectFont(disp, label, labelwidth, labelheight);
  }
  if (!font) return;
  while(ybase > 0) {
    if(last_value != (unsigned int)value_represented) {
      last_value = (unsigned int)value_represented;
      ticks.draw(xbase-2,ybase, xbase+height,ybase);
      if(label_ticks && (font != NULL)) {
	unsigned long labelValue = (unsigned long)value_represented;
	sprintf(label, "%lu", labelValue);
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
**  DrawMappedYTicks:
**    This function draws a set of mapped ticks on a linear mapped y axis.
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
**    float low, hi:
**        The range of values expressed by the axis.
**    Boolean label_ticks:
**        True if the tick marks are to be labelled.
*/
static void DrawMappedYTicks(Display *disp, Window win, GC gc,
			     int xbase, int ybase, int nx, int ny,
			     float low, float hi, 
			     char* ylabel, Boolean label_ticks)
{
  float value_represented, value_interval;
  float interval;
  float   yb = (float)ybase;
  int height;
  float last_value;
  float rem;
  float mant = low - (int)low;

  if ((hi - low) == 0) return;	// No ticks if no interval.

  XSegmentBatch ticks(disp, win, gc);
  char label[20];

  value_represented = (float)low;
  last_value        = -1.111111;	/* Make sure first tick is drawn. */

  // Compute the interval to display between successive tick marks
 
  value_interval = ComputeMappedTickInterval((hi-low), (ybase+1));
  if(value_interval == 0) {
    value_interval = 0.2;
  }

  rem = value_interval - (int)value_interval;
  interval = ((float)(ybase) * value_interval) / (hi-low);
  height   = (int)((ybase)*XAMINE_TICK_FRACTION);

  int labelheight = (int)((float)(2*interval)/
			  XAMINE_TICK_LABEL_INTERVALDIVISOR);
  int labelwidth  = (int)(xbase / 2.0);
  XFontStruct *font = NULL;
  if(label_ticks) {
    if((rem != 0.0) || (mant != 0.0))
      sprintf(label, "%.2f", hi);
    else
      sprintf(label, "%.0f", hi);
    font = Xamine_SelectFont(disp, label, labelwidth, labelheight);
  }
  if (!font) return;
  while(ybase > 0) {
    if(last_value != value_represented) {
      last_value = value_represented;
      ticks.draw(xbase-2,ybase, xbase+height,ybase);
      if(label_ticks && (font != NULL)) {
	if((rem != 0.0) || (mant != 0.0))
	  sprintf(label, "%.2f", value_represented);
	else
	  sprintf(label, "%.0f", value_represented);
	Xamine_DrawCenteredString(disp, win , font, gc,
				  labelwidth,
				  (ybase + labelheight/2), 
				  (2 * labelwidth),
				  (ybase - labelheight/2), label);
      }
    }
    value_represented += value_interval;
    yb    -= interval;
    ybase  = (int)(yb + 0.5);
  }

  // Now we need to draw the y label down the side of the spectrum
  int lh = (int)((float)(150) / XAMINE_TICK_LABEL_INTERVALDIVISOR);
  int lw = labelwidth;
  int length = strlen(ylabel);
  char c[2];
  float fontsize_percent = (1 - (2*((float)length/100.0)));
  lh = (int)(lh*fontsize_percent);
  for(int i = 0; i < length; i++) {
    sprintf(c, "%1c\0", ylabel[i]);
    Xamine_DrawCenteredString(disp, win, font, gc,
			      0, (int)((float)ny / (length*2)) + ((lh/2)*i), 
			      lw,
			      (int)((float)ny / (length*2)) + ((lh/2)*(i+1)), 
			      c);
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
  if (!font) return;

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
  ** as the drawing color.  We also set the background from that widget as well
  */
  XMWidget *pane = ctx->pane;
  Display *disp;
  Screen  *screen;
  Window  win;
  GC      gc; 
  XamineTextGC *context;

  if(!attribs->showaxes()) return; /* NO-OP if axes turned off. */

  disp   = XtDisplay(pane->getid());
  screen = XtScreen(pane->getid());
  win    = ctx->pixmap;

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
  if(attribs->is1d()) {
    win_1d* at1 = (win_1d*) attribs;
    if(at1->ismapped()) {
      if(at1->isflipped())
	xbase = (int)((float)nx * (float)XAMINE_MAPPED_MARGINSIZE);
      else 
	ybase = ny - (int)((float)ny * (float)XAMINE_MAPPED_MARGINSIZE);
    }
  } else {
    win_2d* at2 = (win_2d*) attribs;
    if(at2->ismapped()) {
      xbase = (int)((float)nx * (float)XAMINE_MAPPED_MARGINSIZE);
      ybase = ny - (int)((float)ny * (float)XAMINE_MAPPED_MARGINSIZE);
    }
  }

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
    unsigned int mapped = 0;
    spec_label xlabel;
    spec_label ylabel;
    low = 0;
    hi  = xamine_shared->getxdim(attribs->spectrum());/* Assume unexpanded.*/
    
    if(attribs->is1d()) {
      win_1d *att = (win_1d *)attribs;
      int specno = att->spectrum();
      if(att->isexpanded()) {
	low = att->lowlimit();
	hi  = att->highlimit() +1;
      }
    }
    else {
      win_2d *att = (win_2d *)attribs;

      // The + 1 on high makes the expansion [low,hi].
      if(att->isexpanded()) {
	low = (att->isexpandedfirst() ? att->xlowlim() : att->ylowlim());
	hi  = (att->isexpandedfirst() ? att->xhilim()  : att->yhilim()) +1;
      }
    }

    /* If it's not mapped, just draw like normal */
    if(!attribs->ismapped()) { 
      if(attribs->isflipped()) 
	DrawLinearYTicks(disp, win, gc, xbase, ybase, nx, ny, low, hi,
			 attribs->labelaxes());
      else
	DrawLinearXTicks(disp, win, gc, xbase, ybase, nx,ny, low, hi,
			 attribs->labelaxes());
    }

    /* Otherwise, we need to compute the mapped spectrum range and draw
    ** the appropriate mapped tick marks.
    */
    else {
      float f_low = Xamine_XChanToMapped(attribs->spectrum(), low);
      float f_hi  = Xamine_XChanToMapped(attribs->spectrum(), hi);
      xamine_shared->getxlabel_map(xlabel, attribs->spectrum());
      if(attribs->isflipped())
	DrawMappedYTicks(disp, win, gc, xbase, ybase, nx, ny, f_low, 
			 f_hi, xlabel, attribs->labelaxes());
      else
	DrawMappedXTicks(disp, win, gc, xbase, ybase, nx, ny, f_low,
			 f_hi, xlabel, attribs->labelaxes());
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
	unsigned int ceil = attribs->getceiling();
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
      // The + 1 on high makes the expansion [low,hi].
      if(att->isexpanded() && att->isflipped()) {
	low = (att->isexpandedfirst() ? att->ylowlim() : att->xlowlim());
	hi  = (att->isexpandedfirst() ? att->yhilim()  : att->xhilim()) +1;
      }
      // The + 1 on high makes the expansion [low,hi].
      else if(att->isexpanded()) {
	low = (att->isexpandedfirst() ? att->ylowlim() : att->xlowlim());
	hi  = (att->isexpandedfirst() ? att->yhilim()  : att->xhilim()) +1;
      }
      else if(att->isflipped()) {
	low = 0;
	hi  = xamine_shared->getxdim(att->spectrum());
      }
      
      /* If not mapped, just draw like normal */
      if(!attribs->ismapped()) { 
	if(attribs->isflipped()) 
	  DrawLinearXTicks(disp,win,gc, xbase,ybase, nx,ny, low,hi,
			   attribs->labelaxes()); 
	else 
	  DrawLinearYTicks(disp,win,gc, xbase,ybase, nx,ny, low,hi,
			   attribs->labelaxes()); 
      }

      /* Otherwise, compute the mapped spectrum range and draw the
      ** mapped spectrum ticks
      */
      else {
	float f_ylow = Xamine_YChanToMapped(attribs->spectrum(), low);
	float f_yhi  = Xamine_YChanToMapped(attribs->spectrum(), hi);
	xamine_shared->getylabel_map(ylabel, attribs->spectrum());
	if(attribs->isflipped())
	  DrawMappedXTicks(disp, win, gc, xbase, ybase, nx, ny, f_ylow, 
			   f_yhi, ylabel, attribs->labelaxes());
	else
	  DrawMappedYTicks(disp, win, gc, xbase, ybase, nx, ny, f_ylow, 
			   f_yhi, ylabel, attribs->labelaxes());  
      }
    }
  }
}    
