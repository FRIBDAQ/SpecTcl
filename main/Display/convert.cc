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
**   Xamine -- NSCL Display program.
** Abstract:
**   convert.cc:
**      This file contains implementations of the functions which convert from
**      pixel offsets into spectrum coordinates and vica-versa.  The conversion
**      process is dependent to some extent on the attributes of the spectrum
**      however the major differences are in the 1-d vs. 2-d.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files:
*/
#include <config.h>
#include  <math.h>

#include "convert.h"
#include "XMWidget.h"
#include "dispwind.h"
#include "dispshare.h"
#include "axes.h"
#include "mapcoord.h"
#include <iostream>
#include <math.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


extern volatile spec_shared *xamine_shared;

/*
** Functional Description:
**   Normalize:
**     This function converts a pixel and size into the correct pixel/size
**     positions by adjusting for margins.
** Formal Parameters:
**   win_attributed *a:
**     Pane display attributes.
**   int *xs, *ys:
**     Pointers to the window sizes.
**   int *xp, *yp:
**     Pointer to the pixel sizes.
** The raw sizes passed in will be adjusted to reflect the plotting region
** of the spectrum.  The raw pixel coordinates will be adjusted to cartesian
** (not X-11) coordinates relative to the origin of the spectrum plotting region
*/
static void Normalize(win_attributed *a, int *xs, int *ys, int *xp, int *yp)
{
  int xmarg, ymarg;
  if(a->is1d()) {
    win_1d* a1 = (win_1d*)a;
    if(a1->ismapped()) {
      if(a1->isflipped()) {
        xmarg = (int)((float)(*xs) * XAMINE_MAPPED_MARGINSIZE);
        ymarg = (int)((float)(*ys) * XAMINE_MARGINSIZE);
      }
      else {
        xmarg = (int)((float)(*xs) * XAMINE_MARGINSIZE);
        ymarg = (int)((float)(*ys) * XAMINE_MAPPED_MARGINSIZE);
      }
    }
    else {
      xmarg = (int)((float)(*xs) * XAMINE_MARGINSIZE);
      ymarg = (int)((float)(*ys) * XAMINE_MARGINSIZE);
    }
  }
  else {
    win_2d* a2 = (win_2d*) a;
    if(a2->ismapped()) {
      xmarg = (int)((float)(*xs) * XAMINE_MAPPED_MARGINSIZE);
      ymarg = (int)((float)(*ys) * XAMINE_MAPPED_MARGINSIZE);
    }
    else {
      xmarg = (int)((float)(*xs) * XAMINE_MARGINSIZE);
      ymarg = (int)((float)(*ys) * XAMINE_MARGINSIZE);
    }
  }
  
  /* adjust pixels to cartesian coords relative to the window origin: */

  *yp = *ys - *yp;		/* X y coords are backwards. */

  /* If there's an axis, then remove the margin: */

  if(a->showaxes()) {

    *xs -= xmarg;
    *xp -= xmarg;

    *ys -= ymarg;
    *yp -= ymarg;
    *xp -= 1;
    return;
  }

  /* If there's no axis, but titles are being displayed, then half the margin 
  ** is used on the Y axis, and no margin is used on the X axis.
  */
  if(a->showname()    ||   a->shownum()   ||
     a->showdescrip() ||   a->showpeak()  ||
     a->showupdt()    ||   a->showlbl()) {
    ymarg = ymarg >> 1;

    *ys -= ymarg;
    *yp -= ymarg;
  }
  *xp -= 1;
}

/*
** Functional Description:
**   LinearPosition:
**     This function determines what a pixel value represents in a 
**     linear scale.
** Formal Parameters:
**   int pixel:
**      Pixel position relative to the start of the axis.
**   int lo,hi:
**      World coordinates represented by the axis.
**   int npix:
**      Number of pixels in the axis.
** Returns:
**     other  - The value in the [low,hi] range represented by the pixel.
*/
static float LinearPosition(int pixel, int lo, int hi, int npix)
{

  return Transform(0.0, (float)(npix-1), (float)lo, (float)hi,
		    (float)pixel);

}

/*
** Functional Description:
**   LogPosition:
**     This function determines what a pixel value represents in a log scale
**     range.
** Formal Parameters:
**   int pixel:
**     The pixel offset relative to the start of the axis.
**   int lo,hi:
**     The axis range 
**   int npix:
**     The number of pixels available for the axis.
*/
static int LogPosition(int pixel, int lo, int hi, int npix)
{
  double loglo, loghi;

  if(pixel < 0) return -1;
  if(pixel > npix) return -1;


  /* Xamine only allows full decade displays so we compute the log of the 
  ** low/hi range truncated to the decade.
  */

  if(lo > 0) {
    int llo = (int)log10((double)lo);
    loglo   = (double)llo;
  }
  else {
    loglo = 0.0;
  }
  if(hi > 0) {
    double lhi = log10((double)hi);
    if ((float)((int)lhi) != lhi) {
      loghi = (double)((int)(lhi+1.0));
    } 
    else {
      loghi      = (double)((int)lhi);    
    }
  }
  else {
    loghi = 1.0;
  }
  float logvalue = ((float)pixel/(float)npix)*(loghi - loglo) + loglo;

  return (int)pow(10, logvalue);



  /* now compute the log of the position: */

  // double lpos = Transform(0, log10((float)(npix-1)), 
  //		  loglo, loghi, log10((float)pixel));
  
  //  double lpos = loglo + ((double)(pixel)*(loghi - loglo+1)/(double)npix);
  
  //   lpos = pow(10, lpos);
  //   return (int)lpos;
}

/*
** FUnctional Description:
**    LogPixel:
**      This function locates a counts position on a log axis.
** Formal Parameters:
**    int counts:
**      Number of counts to convert
**    int lo,hi:
**      Range of counts represented by the axis.
**    int npix:
**      Number of pixels in the axis.
** Returns:
**   Pixel offset that corresponds to the counts.
*/
int LogPixel(int counts, int lo, int hi, int npix)
{
  /* We compute the log of the bottom and top counts */

  int low_decade, hi_decade;
  if(lo > 0) {
    low_decade = (int)log10((double)lo);
  }
  else {
    low_decade = 0;
  }
  if(hi > 0) {
    hi_decade = (int)log10((double)hi);
  }
  else {
    hi_decade = 1;
  }

  /* We compute the log of the counts:   */

  double lcounts;
  if(counts > 0.0) {
    lcounts  = log10((double)counts);
  }
  else {
    lcounts = 0.0;
  }

  /* Now figure out where on the axis this all lands: */

  if(low_decade > hi_decade) low_decade = hi_decade;   // Protect divide by 0.
  
  // int position  = (int)Transform(low_decade, hi_decade, 
  //			 0, (float)npix-1,
  //				 (float)lcounts);
				 
    int position = (int)( (lcounts * (double)npix)/
  		       ((double)(hi_decade-low_decade + 1)));
  return position;

}

/*
** Functional Description:
**   Xamine_Convert1d::ScreenToSpec:
**      Convert screen coordinates to spectrum coordinates.
**      Note that we return a structure which includes X and y values
**      and channel values.  One of the X/Y values will be a channel number,
**      the other a counts value, depending on the flippedness of the axes.
** Formal Parameters:
**    spec_location *loc:
**       Returns the spectrum location values here.
**    int xpix, ypix:
**       pixel positions to convert.
*/
void Xamine_Convert1d::ScreenToSpec(spec_location *loc, int xpix, int ypix)
{
  // All this is much simpler now that we don't allow flipping.

  win_1d *a = (win_1d *)attributes;
  int spec = attributes->spectrum();
  
  // Pane manager coordinates:
  
  int index;
  pane->GetAttribute(XmNuserData, &index);
  int row = index/WINDOW_MAXAXIS;
  int col = index/WINDOW_MAXAXIS;
  
  // Drawing region:
  
  Rectangle drawRegion = Xamine_GetSpectrumDrawingRegion(pane, attributes);
  
  // Get the channel in the spectrum. From that we get counts.
  // which has to be corrected for the possibility this is a log plot.
  
  int channel = Xamine_XPixelToChannel(
    spec, row, col, drawRegion.xbase, drawRegion.xmax, xpix
  );
  unsigned counts = spectra->getchannel(spec, channel);
  
  // Now figure out how to transform the y position to counts as well:
  
  // First the range of displayed y values.
  double base = 0;
  if (a->hasfloor()) base = a->getfloor();
  double maximum = a->getfsval();
  if(a->hasceiling() && (a->getceiling() < maximum)) maximum = a->getceiling();
  
  // If these are log we're really operating in log coordinates:
  
  if (a->islog()) {
    if (base > 0) base = log10(base);
    if (maximum > 0) {
      maximum = log10(maximum);
      maximum = ceil(maximum);   // since we have a complete set of decades.
    }
  }
  double ht = Transform(drawRegion.ybase, 0, base, maximum, ypix);
  
  // If log pu it back into actual values;
  
  if (a->islog()) ht = exp10(ht);
  
  // Now fill in the locator:
  
  loc->xpos = channel;
  loc->ypos = ht;
  loc->counts = counts;
}

/*
** Functional Description:
**    Xamine_Convert1d::SpecToScreen:
**      This function converts spectrum positions to screen coordinates.
**      This allows, for example, typed in positions to be converted into
**      pixels.
**  Formal Parameters:
**    int *xpix, int *ypix:
**       Returned pixel values.
**    int chan, counts:
**      Channel number and counts coordinates.
*/
void Xamine_Convert1d::SpecToScreen(int *xpix, int *ypix, int chan, int counts)
{
  int specno = attributes->spectrum();
  int index;
  pane->GetAttribute(XmNuserData, &index);
  int row = index/WINDOW_MAXAXIS;
  int col = index/WINDOW_MAXAXIS;
  auto drawregion = Xamine_GetSpectrumDrawingRegion(pane, attributes);
  
  // Note this is simplified now that flips are disabled.  We place the x position
  // at the left edge of the channel:
  
  auto xpixrange = Xamine_XChanToPixelRange(
    specno, row, col, drawregion.xbase, drawregion.xmax, chan
  );
  *xpix = xpixrange.first;
  
  // If we are log scale we need to transform the counts:
  
  double c = counts;
  if(attributes->islog()) {
    if (c > 0) {
      c = log10(c);
    }
  }
  // Now the range (if needed in log form)
  
  double base = 0;
  if (attributes->hasfloor()) base = attributes->getfloor();
  double maximum = attributes->getfsval();
  if(attributes->hasceiling() && (attributes->getceiling() < maximum)) {
    maximum = attributes->getceiling();
  }
  
  // If these are log we're really operating in log coordinates:
  
  if (attributes->islog()) {
    if (base > 0) base = log10(base);
    if (maximum > 0) {
      maximum = log10(maximum);
      maximum = ceil(maximum);     // we display a complete set of decades.
    }
  }
  // Transform from counts -> pixels:
  
  *ypix = Transform(base, maximum, drawregion.ybase, 0, c);
  
}

/*
** Functional Description:
**   Xamine_Convert2d::ScreenToSpec:
**     This function takes x and y pixels on a 2-d pane and computes the
**     channel coordinats as well as the number of counts at the channel
**     represented by that position.  The code is much simpler than for
**     1-d spectra since both axes must be linear (both are channel axes).
** Formal Parameters:
**    spec_location *loc:
**      points to the buffer that will be filled with a description of
**      the world coordinate position represented by the pixels.
**    int xpix, ypix:
**       Pixel coordinates to convert.
*/
void Xamine_Convert2d::ScreenToSpec(spec_location *loc, int xpix, int ypix)
{
  int spec = attributes->spectrum();
  win_2d *att = (win_2d *)attributes;
  
  /* First figure out how many pixels are in the X and Y channel direction. */

  int index;
  pane->GetAttribute(XmNuserData, &index);
  int row = index/WINDOW_MAXAXIS;
  int col = index/WINDOW_MAXAXIS;
  Rectangle drawRegion = Xamine_GetSpectrumDrawingRegion(pane, attributes);
  
  // Now the X/Y coordinates are just linear transforms.
  // The counts are gotte4n from the channel indexes and the
  
  int cx = Xamine_XPixelToChannel(
    spec, row, col, drawRegion.xbase, drawRegion.xmax, xpix
  );
  int cy = Xamine_YPixelToChannel(
    spec, row, col, drawRegion.ybase, 0, ypix
  );
  // Now we get the value:
  
  int counts = spectra->getchannel(spec, cx, cy);
  
  // FIll in the locator:
  
  loc->xpos = cx;
  loc->ypos = cy;
  loc->counts = counts;
  
}

/*
** Functional Description:
**   Xamine_Convert2d::SpecToScreen:
**     This function converts a set of channel positions into screen coordinates
**     This allows the code to take a set of channel values and convert them
**     into display pixels.
** Formal Parameters:
**   int *xpix, *ypix:
**     These are returned and are the pixel coordinates corresponding to the
**     channels.
**   int xchan, ychan:
**     These are the X and Y channel coordinates entered by the user.
*/
void Xamine_Convert2d::SpecToScreen(int *xpix, int *ypix, int chanx, int chany)
{
  int spec = attributes->spectrum();
  win_2d *a = (win_2d *)attributes;
  
  // Figure out the drawing rectangle:
  
  int index;
  pane->GetAttribute(XmNuserData, &index);
  int row = index/WINDOW_MAXAXIS;
  int col = index/WINDOW_MAXAXIS;
  Rectangle drawRegion = Xamine_GetSpectrumDrawingRegion(pane, attributes);
  
  // This is simple now that flips are disabled.  We're going to put the
  // position at the lower left of the channel block.
  
  auto xpixrange = Xamine_XChanToPixelRange(
    spec, row, col, drawRegion.xbase, drawRegion.xmax, chanx
  );
  *xpix = xpixrange.first;
  
  auto ypixrange = Xamine_YChanToPixelRange(
    spec, row, col, drawRegion.ybase, 0, chany
  );
  *ypix = ypixrange.first;
 
}
