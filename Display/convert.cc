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
#include <Iostream.h>

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
  win_1d *a = (win_1d *)attributes;
  int spec = attributes->spectrum();

  /* First we need to get the size of the pane containing the spectrum. */
  
  Dimension xs, ys;
  pane->GetAttribute(XmNwidth, &xs);
  pane->GetAttribute(XmNheight, &ys);
  int xsize = xs;
  int ysize = ys;

  /* The margins must now be figured into the picture.  When we finish this
  ** section of code: xsize, ysize will be the used segment of the display.
  ** xpix and ypix will be the cartesian (not X11) coordinates of the
  ** pixel within the spectrum display region.
  */
  Normalize(attributes, &xsize, &ysize, &xpix, &ypix);

  /* When we finish falling through this section, the following will be
  ** set up:
  **    chanpix   - Channel pixel offset.
  **    chanlow, chanhi   - Range of channels represented.
  **    chansize  - # of pixels representing the channels.
  **    cntspix   - Counts pixel offset.
  **    cntslow, cntshi  - Range of counts represented.
  **    cntssize   - Number of pixels of counts data.
  */

  int chanpix, chansize;
  int          cntspix;
  int chanlow, chanhi;
  unsigned int  cntslow, cntshi, cntssize;

  if(attributes->isflipped()) {	/* Flipped orientation. */
    chanpix   = ypix;
    chansize  = ysize;

    cntspix   = xpix;
    cntssize  = xsize;
  }
  else {			/* Unflipped orientation. */
    chanpix   = xpix;
    chansize  = xsize;
  
    cntspix   = ypix;
    cntssize  = ysize;
  }
  if(a->isexpanded()) {
    chanlow = (int)a->lowlimit();
    chanhi  = (int)a->highlimit() + 1;
  }
  else {
    chanlow = 0;
    chanhi  = (int)spectra->getxdim(spec);
  }
  cntslow = 0; 
  cntshi  = attributes->getfsval();

  /*
  ** Now we figure out the channel and counts values.  
  ** negative normalized pixels result in 'meaningful' values, negative 1 for
  ** channel positions and 0 for counts. 
  ** The variables we produce are:
  **    channel   - Channel position of the cursor.
  **    countpos  - Counts position of the cursor.
  **  we also fill in the counts value of the returnd spec_location structure
  */
  int channel;
  unsigned int countpos;
  if((chanpix < 0) && clipping) {		/* set at  bottom channel */
    channel =   chanlow;		// Clip to the axis.
    countpos=   0;
    loc->counts = 0;
  }
  else {
    // channel = (int)LinearPosition(chanpix, chanlow, chanhi, chansize);
   
    channel = (int)Transform(0.0, (float)(chansize-1), 
			(float)chanlow, (float)chanhi, 
			(float)chanpix);
    if (channel < chanlow) {
      channel = chanlow;
    }
    if (channel >= chanhi) {
      channel = chanhi - 1;
    }
    
    // if((channel < chanlow) || (channel > chanhi)) {
    //   loc->counts = 0;
    //  }
    // else {
    loc->counts  = spectra->getchannel(spec, channel);
    // }
    if(attributes->islog()) {
      countpos = LogPosition(cntspix, cntslow, cntshi, cntssize);
    }
    else {
      //      countpos =(int)LinearPosition(cntspix, cntslow, cntshi, cntssize);
      countpos = (unsigned int)Transform(0, (float)(cntssize-1), 
			   (float)cntslow, (float)cntshi, 
			   cntspix);
    }
  }
  /*
  ** convert the countpos, channel values into appropriate loc values.
  */
  if(attributes->isflipped()) {
    loc->xpos = countpos;
    loc->ypos = channel;
  }
  else {
    loc->xpos = channel;
    loc->ypos = countpos;

  }
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
  win_1d *att = (win_1d *)attributes;

  /* First get the x and y pixel extents of the screen. */

  Dimension nxs,nys;

  pane->GetAttribute(XmNwidth, &nxs);
  pane->GetAttribute(XmNheight, &nys);
  int nx = nxs - 1;
  int ny = nys - 1;

  /* Adjust these sizes figuring in the margins.  Note that the origin pixel
  ** will allow us to correct the final pixels back to the full window.
  */
  int orgx = 0;
  int orgy = ny;
  Normalize(attributes, &nx, &ny, &orgx, &orgy);
  /*
  ** Figure out which axis is the counts axis and setup the values of
  **    cntspix      - Number of pixels on the counts axis.
  **    cntslo       - What origin of counts axis means.
  **    cntshi       - What top of counts axis means.
  **    chanpix      - Number of pixels on the channels axis.
  **    chanlo       - Low channel meaning.
  **    chanhi       - high channel meaning.
  */
  int cntspix,cntslo,cntshi;
  int chanpix,chanlo,chanhi;

  if(attributes->isflipped()) {
    cntspix = nx;
    chanpix = ny;
  }
  else {
    cntspix = ny;
    chanpix = nx;
  }
  cntslo = 0;
  cntshi = attributes->getfsval();

  chanlo = 0;
  chanhi = spectra->getxdim(specno); // Goes to the end of the last chan.
  if(att->isexpanded()) {
    chanlo = att->lowlimit();
    chanhi = att->highlimit() +1; // Goes to end of last channel.
  }

  /* The channel axis is always linear so:  */
  int chpix;

  //  chpix = (int)LinearPosition(chan - chanlo, 1, chanpix-1, (chanhi-chanlo));
  

  chpix = (int)Transform((float)chanlo, (float)(chanhi - 1),
			 0.0, (float)(chanpix), chan) + 1;

  /* The counts axis could be log though:  */
  int cpix;
  if(attributes->islog()) {
    cpix = LogPixel(counts, cntslo, cntshi, cntspix);
  }
  else {

    //    cpix = (int)LinearPosition(counts - cntslo, 1, cntspix-1, (cntshi-cntslo));
    cpix  = (int)Transform((float)cntslo, (float)cntshi,
			   0.0, (float)(cntspix-1), counts);
  }

  /* Figure the cartesian pixels relative to the axis origin based on the
  ** flipped state: 
  */
  if(attributes->isflipped()) {
    *xpix = cpix;
    *ypix = chpix;
  }
  else {
    *xpix = chpix;
    *ypix = cpix;
  }

  /* Adjust for origin shift and X-11 coordinate system:  */

  *xpix -= orgx;
  *ypix = ny - (*ypix);

  /* If this takes anything out of the display then clip to the edge. */
 
  if(clipping) {
    
    if(*xpix < 0)     *xpix = 0;
    if(*xpix >= nxs)  *xpix  = nxs;
    if(*ypix < 0)     *ypix = 0;
    if(*ypix >= nys)  *ypix  = nys;
  }
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

  Dimension nxs;
  Dimension nys;

  pane->GetAttribute(XmNwidth, &nxs);
  pane->GetAttribute(XmNheight, &nys);
  
  int nx = nxs;
  int ny = nys;

  /*  Figure the X/Y channel limits represented by nx and ny pixels */
  int xl,xh;
  int yl,yh;
  
  if(att->isexpanded()) {
    xl = (att->isflipped() ? att->ylowlim() : att->xlowlim());
    xh = (att->isflipped() ? att->yhilim()  : att->xhilim());
    yl = (att->isflipped() ? att->xlowlim() : att->ylowlim());
    yh = (att->isflipped() ? att->xhilim()  : att->yhilim());
    xh++;
    yh++;
    if(att->isexpandedfirst()) {
      /*
	int temp = xl;
	xl = yl;
	yl = temp;

	temp = xh;
	xh = yh;
	yh = temp;
      */
    }
  }
  else {
    xl = 0;
    xh = spectra->getxdim(spec)-1;
    yl = 0;			/* These are in spectrum orientation so... */
    yh = spectra->getydim(spec)-1;
  }

  /* If the axes are flipped, then so are (xl,xh) and (yl,yh) */
  
  if(att->isflipped()) {
    int temp;
    temp = xl;
    xl   = yl;
    yl   = temp;
    
    temp = xh;
    xh   = yh;
    yh   = temp;  
  }

  /* Next normalize the coordinates to Cartesians relative to the
  ** start of the spectrum display region.
  */

  Normalize(attributes, &nx, &ny, &xpix, &ypix);

  /* Get channel values: */

  int xp, yp;

  // xp = (int)LinearPosition(xpix, xl, xh, nx);
  // yp = (int)LinearPosition(ypix, yl, yh, ny);

  xp = (int)Transform(0.0, (float)(nx-1),
		      (float)xl, (float)xh, (float)xpix);
  yp = (int)Transform(0.0, (float)(ny-1),
		      (float)yl, (float)yh, (float)ypix);
		      

  if((att->isflipped() && att->isexpanded() && att->isexpandedfirst()) ||
     (!att->isflipped() && att->isexpanded() && !att->isexpandedfirst())) {
    loc->ypos = xp;
    loc->xpos = yp;
    int temp = xl;
    xl = yl;
    yl = temp;
    temp = xh;
    xh = yh;
    yh = temp;
  }
  else {
    loc->xpos = xp;
    loc->ypos = yp;
  }

  if( ((xp >= xl) && (yp >= yl) && (xp <= xh) && (yp <= yh))) {
    if(attributes->isflipped()) {
      loc->counts = spectra->getchannel(spec, yp, xp);
    }
    else {
      loc->counts = spectra->getchannel(spec, xp, yp);
    }
  }
  else {
    loc->counts = 0;
  }

  if(clipping) {
    if(xp < xl) loc->xpos = xl;
    if(xp > xh) loc->xpos = xh;
    if(yp < yl) loc->ypos = yl;
    if(yp > yh) loc->ypos = yh;
  }
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
  /* First figure out the number of pixels in X and Y directions.
  ** We'll use the translated origin coordinates later to adjust the
  ** computed pixels into Full window X-11 coordinates:
  */

  Dimension nxs, nys;
  pane->GetAttribute(XmNwidth, &nxs);
  pane->GetAttribute(XmNheight, &nys);

  int orgx = 0;
  int orgy = nys;

  int nx = nxs;
  int ny = nys;

  Normalize(attributes, &nx, &ny, &orgx, &orgy);

  /* Figure out the channel ranges represented by each axis, here's where
  ** we need to take into account any axis flipping.
  */

  int xl,xh,yl,yh;
  
  if(a->isexpanded()) {
    xl = a->xlowlim();
    xh = a->xhilim();		/* These are in screen orientation. */
    yl = a->ylowlim();
    yh = a->yhilim();
  }
  else {
    xl = 0;
    xh = spectra->getxdim(spec)-1;
    yl = 0;			/* These are in spectrum orientation and */
    yh = spectra->getydim(spec)-1; /* Must therefore be flipped if the  */
				/* spectrum is in flipped orientation.  */
    if(a->isflipped()) {
      int temp;
      temp = xl;
      xl   = yl;
      yl   = temp;
      temp = xh;
    xh   = yh;
      yh   = temp;
    }

  }
  /* Figure out the pixel positions relative to the origin in cartesian
  ** coordinates.
  */


  // *xpix = (int)LinearPosition(chanx - xl,1, nx, (xh-xl + 1));
  // *ypix = (int)LinearPosition(chany - yl,1, ny, (yh-yl + 1));

  *xpix = (int)(Transform((float)xl, (float)xh,
			  0.0, (float)(nx-1), (float)chanx));
  *ypix = (int)(Transform((float)yl, (float)yh, 
			  0.0, (float)(ny-1), (float)chany));
		
  /* Adjust pixel coordinates into X/Y X-11 positions: */

  *xpix -= orgx;
  *ypix = ny - (*ypix);
  if(clipping) {
    if(*xpix < orgx) *xpix = orgx;
    if(*xpix > nxs)  *xpix = nxs;
    if(*ypix < 0)    *ypix = 0;
    if(*ypix > ny)   *ypix = ny;
    
  } 
}
