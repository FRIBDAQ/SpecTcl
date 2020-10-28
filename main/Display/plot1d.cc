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
**   Xamine -- NSCL display program.
** Abstract:
**   plot1d.cc:
**     This file contains the code necessary to plot 1-d spectra on 
**     an Xamine pane.  The public entry point is Xamine_Plot1d.  The
**     publics are defined in chanplot.h
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/* 
** Include files:
*/

#include <config.h>

#include <stdio.h>
#include <math.h>

#include "XMWidget.h"
#include "XBatch.h"

#include "scaling.h"
#include "chanplot.h"
#include "panemgr.h"
#include "dispshare.h"
#include "dispwind.h"
#include "mapcoord.h"

#include <iostream>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*
** External data structures referenced:
*/
extern spec_shared *xamine_shared;

/*
** Local data:
*/
static 
int   min_pixels[] = {  SMOOTH1D_PIXELS,
			 HISTO1D_PIXELS,
			 LINES1D_PIXELS,
			 POINTS1D_PIXELS };

 


/*
** Channel drawing methods -- these methods and their virtual functions
** allow the process of drawing to be made completely generic once the
** appropriate sampling and channel draw classes have been instantiated.
*/

class Channel	{	    /* Base class... never meant to be instantiated. */
 protected:
  int pixl, pixh;      // Pixel range of the plot area.
		int numchans;     // CHannel range of the plot area.
 public:
  Channel(int pixlo, int pixhi, int nchans) :
			pixl(pixlo), pixh(pixhi), numchans(nchans) 
		{
    
  }
  virtual ~Channel()  {}
  virtual void drawchan(std::pair<unsigned, unsigned>htandchans) = 0;
		double width(int chan1, int nchans) {
			double lowlim = Transform(0, numchans-1, pixl, pixh, chan1);
			double hilim  = Transform(0, numchans-1, pixl, pixh, chan1+nchans);
			return hilim-lowlim;             // Extent of the channel.
		}
};

/*
** HChannel draws histogram bar chart channels 
*/
class HChannel : public Channel, public XLineBatch {
 protected:
  double xpos;
  double xbase;
  short   yorg;
  int   channel;
 public:
  HChannel(Display *d, Drawable w, GC c, short x0, short y0, short xhi, int nchans) :
    Channel(x0,xhi, nchans), XLineBatch(d, w, c) {
      xpos = (float)x0;
      xbase= (float)x0;
      channel = 0;
      yorg = y0;
      channel = 0;
      draw(x0, y0);
    }
  virtual ~HChannel() { }
  virtual void drawchan(std::pair<unsigned, unsigned>htandchans) {
			unsigned short height = htandchans.first;
			unsigned       nchans = htandchans.second;
    draw((short)xpos,yorg-height);		/* Go to channel level... */
    xpos = xpos + width(channel,nchans); // prevents round off problems from multiple sums.
    draw((short)xpos,yorg-height);		/* Go across channel */
				channel+=nchans;
  }
};

class HChannelF : public Channel, public XLineBatch {
 protected:
  float ypos;
  short   xorg;
  float ybase;
  int   channel;
 public:
  HChannelF(Display *d, Drawable w, GC c, short x0, short y0, int ymax, int nchans) :
    Channel(y0, ymax, nchans),
				XLineBatch(d, w, c) {
      ypos = (float)y0;
      ybase = ypos;
      channel = 0;
      draw(x0, y0);
      xorg = x0;
    }
  virtual ~HChannelF() { }
  virtual void drawchan(std::pair<unsigned, unsigned> htandchans) {
			unsigned height = htandchans.first;
			unsigned nchans = htandchans.second;
    draw(xorg+height, (short)ypos);		/* Go to channel level... */
    ypos = ypos - width(channel, nchans); 
    draw(xorg+height, (short)ypos);		/* Go across channel */
				channel+=nchans;
  }
};

/*
** CLass LChannel draws histograms as connect the dots in the center of the
** channel.
*/
class LChannel : public Channel, public XLineBatch {
 protected:
  float xpos;
  short yorg;
		int channel;
 public:
  LChannel(Display *d, Drawable w, GC c, short x0, short y0, short xmax, int nchans) :
     Channel(x0, xmax, nchans), XLineBatch(d, w, c) {
      xpos = (float)x0 - width(0, 1)/2; /* Set up for first channel. */
      yorg = y0;
						channel = 0;
    }
  virtual void drawchan(std::pair<unsigned, unsigned>htandchans) {
			unsigned height = htandchans.first;
			unsigned nchans = htandchans.second;
    xpos += width(channel,nchans);		/* This is center of the channel... */
    draw((short)xpos, yorg-height);	/* Draw a channel */
				channel+=nchans;
  }
};

class LChannelF : public Channel, public XLineBatch {
 protected:
  float ypos;
  short xorg;
		int  channel;
 public:
  LChannelF(Display *d, Drawable w, GC c, short x0, short y0, short ymax, int nchans) :
     Channel(y0, ymax, nchans), XLineBatch(d, w, c) {
						channel  = 0;
      ypos = (float)y0 - width(0, 1)/2; /* Set up for first channel. width is <0 */
      xorg = x0; 
    }
  virtual void drawchan(std::pair<unsigned, unsigned>htandchans) {
			unsigned height = htandchans.first;
			unsigned nchans = htandchans.second;
    ypos += width(channel, nchans);		/* This is center of the channel... width < 0*/
    draw(xorg+height, (short)ypos);	/* Draw a channel */
				channel+=nchans;
  }
};

/*
** Class PChannel draws histograms as dots in the center of the channel.
*/
class PChannel : public Channel, public XPointBatch {
 protected:
  float xpos;
  short yorg;
		int channel;
 public:
  PChannel(Display *d, Drawable w, GC c, short x0, short y0, int xmax, int nchans) :
     Channel(x0, xmax, nchans), XPointBatch(d, w, c) {
						channel = 0;
      xpos = (float)x0 - width(0,1 )/2; /* Set up for first channel. */
      yorg = y0;
    }
  virtual void drawchan(std::pair<unsigned, unsigned>htandchans) {
			unsigned height = htandchans.first;
			unsigned nchans = htandchans.second;
    xpos += width(channel, nchans);		/* This is center of the channel... */
    draw((short)xpos, yorg-height);	/* Draw a channel */
				channel += nchans;
  }
};

class PChannelF : public Channel, public XPointBatch {
 protected:
  float ypos;
  short xorg;
		int channel;
 public:
  PChannelF(Display *d, Drawable w, GC c, short x0, short y0, int ymax, int nchans) :
     Channel(y0, ymax, nchans), XPointBatch(d, w, c) {
						channel = 0;
      ypos = (float)y0 - width(0,1)/2; /* Set up for first channel. width < 0*/
      xorg = x0;
    }
  virtual void drawchan(std::pair<unsigned, unsigned> htandchans) {
			unsigned height= htandchans.first;
			unsigned nchans = htandchans.second;
    ypos += width(channel,nchans);		/* This is center of the channel... width < 0*/
    draw(xorg+ height, (short)ypos);	/* Draw a channel */
				channel+=nchans;
  }
};


/*
** Functional Description:
**    PlotLin:
**       Plot a histogram AOI with linear scaling.
** Formal Parameters:
**    Channel *d:
**       The object which knows how to draw a single channel on the display.
**    Sampler *s:
**       The object which knows how to produce the value of the next channel 
**       to draw.
**    float xw:
**       The number of pixels in a channel representation.
**    int nx,ny:
**        The number of x and y pixels available for drawing.
**    int base:
**         The lower level of the region represented by ny pixels.
**    int maximum:
**         The upper level of the region represented by ny pixels.
*/
static void PlotLin(Channel *d, Sampler *s, float xw, int nx, int ny,
		    unsigned int base, unsigned int maximum)
{
  float channels = xw/2.0;	/* Starting offset from where we're going */
  float fnx      = (float)nx;	/* Number of x pixels in fp... */
  unsigned int   range    = maximum - base;

		
  while(channels < fnx) {
			 std::pair<unsigned, unsigned>htandchans = s->sample();
    unsigned long value = htandchans.first;
    if(value <= base) {
      value = 0;
    } else {
      value -= base;
    }
    if(value > range) value = range;

    /* now scale the counts to the ny range... The plotter adjust the
    ** coordinates to fit the range.
    */

    value = (unsigned long)((float)ny*((float)value)/((float)range));
				htandchans.first = value;
    d->drawchan(htandchans);

    channels += xw;		/* Go on to the next channel position */
  }
}

/*
** Functional Description:
**   PlotLog:
**      This function plots channels on a log scale.
**      There are a few hinks and kinks related to the fact that the log
**      scales only allow full decades to be shown.  In addition of course
**      we must attempt to avoid log's of zeroes and negatives to avoid
**      Math traps.
**  Formal Parameters:
**    Channel *d:
**      The channel drawing object.  This object knows how to draw one channel
**    Sampler *s:
**      The sampler object.  This object knows how to reduce the data to
**      fit the tube.
**    float xw:
**      The number of pixels per channel including fractions.
**    int nx, ny:
**       The number of pixels in the x and y direction.
**    int base,maximum:
**       The range of values represented by the data... this will be
**       made to fit into complete decades prior to scaling.
*/
static void PlotLog(Channel *d, Sampler *s, float xw, int nx, int ny, 
		    unsigned int base, unsigned int maximum)
{
  /* Figure out the scaling parameters in logs: */

  double logbase, logmax, loginterval;
  if(base <= 0) 
    logbase = 0.0;
  else
    logbase = log10((double)base);
  if(maximum > 0)
    logmax = log10((double)maximum);
  else
    logmax = 1.0;
  logbase = (double)((int)logbase); /* truncate low limit for full decade */
  logmax  = (double)((int)logmax);  /* Truncate hi limit for full decade */
  loginterval = logmax - logbase + 1.0;
  if(loginterval < 1.0) loginterval = 1.0;

  /* now loop over the data: */

  float offset = xw/2.0;
  float fnx    = (float)nx;
  while (offset < fnx) {
			 std::pair<unsigned, unsigned> htandchans = s->sample();
    double value = htandchans.first;
    if (value == 1.0) {
      value = 1.5;		// This kludge allows 1 count chans to be seen
    }
    if(value <= 0.0) 
      value = -logbase;
    else
      value = log10(value) - logbase;	/* We're plotting logs though. */
    if(value < 0.0) value = 0.0;
    if(value > loginterval) value = loginterval;
    int y = (int)((float)ny*(value/loginterval));
    htandchans.first = y;
    d->drawchan(htandchans);


    offset += xw;
  }
}

/*
** Functional Description:
**   Plot:
**     This function plots the spectrum channels on the display segment.
**     Using the sampler and drawer passed in, we can generically sample
**     and plot channels.  The only other work besides this is the scaling
**     and cutting that is needed to apply the user height cuts.
** Formal Parameters:
**   Channel *d:
**      Drawer... this object iteratively draws single channels of the
**      spectrum.
**   Sampler *s:
**      This object samples values from the spectrum to make it fit in the
**      region available.
**   int xw:
**      Width of a channel in pixels.
**   int nx,ny:
**      pixel size of region to plot in.
**   win_1d *att:
**      Display attributes object describing the pane.
*/
static void Plot(Channel *d, Sampler *s, float xw, int nx, int ny, win_1d *att)
{
  /* Figure out the scaling and offset parameters.  */

  unsigned int base, maximum;		/* Bottom value and full scale. */

  base = 0;			/* Assume zero bias... */
  if(att->hasfloor()) base = att->getfloor();
  maximum  = att->getfsval();	/* Assume no ceiling. */
  if(att->hasceiling()  && (att->getceiling() < maximum)) 
    maximum = att->getceiling();

  /* Select between log plotter and linear plotter: */

  if(att->islog()) {
    PlotLog(d, s, xw, nx,ny, base,maximum);
  }
  else {
    PlotLin(d,s, xw, nx,ny, base,maximum);
  }

}

/*
** Functional Description:
**   Xamine_Plot1d:
**     This function plots 1-d histograms.  1-d histograms can largely be
**     plotted using virtual method objects to genericize the action.
**     Action is genericized using sampler objects to genericize the action
**     of getting a spectrum channel value and channel drawers to genericize
**     drawing channels in given renditions.
**     This function:
**       Determines the area of interest.
**       Determines the appropriate reduction parameters and pixels per chan.
**       Choses and instantiates a sampler object.
**       Choses and instantiates a drawer object.
**       Draws the area of interest.
**       Destroys the instantiated drawer and sampler
** Formal Parameters:
**   Screen *s:
**     Screen to plot on.
**   Display *d:
**      Display to plot on.
**   GC gc:
**      Graphic context to use in the plot.
**   win_attributed *att:
**      attributes of the spectrum.
**   Drawable win:
**      The drawable to write the histogram into.
**   int orgx,orgy:
**       Origin coordinates of the drawing region.
**   int nx, ny:
**       Extent of drawing region in pixels:
** NOTE:  X windows coordinates extend to the right and down as positive
**        increments from the origin.. that's why you'll see a lot of cases
**        where values are SUBTRACTED from y0 instead of added throughout
**        this file.
** Returns:
**    True - Indicating that the draw is done in a single leap. 
*/
Boolean Xamine_Plot1d(Screen *s, Display *d,  win_attributed *att,
		      Drawable win, XMWidget *pane,
		      int orgx, int orgy, int nx, int ny)
{
  win_1d *at1 = (win_1d *)att;	     /* Cast the attributes to 1-d.  */
  int     spno= at1->spectrum();    /* Fetch the spectrum number. */
  float   chanw;		/* Width of a channel in pixels. */
  SuperpositionList &super_list = at1->GetSuperpositions();
  SuperpositionListIterator supers(super_list);

  
  GC        gc = Xamine_MakeDrawingGc(d, pane, -1);

  /* Figure out the area of interest. */

  int xl, xh;			/* The region of interest. */
  xl = 1;			/* Assume no expansion... */
  xh = xamine_shared->getxdim(spno) - 1;	/* -2 because inclusive and dead root channels */
  if(at1->isexpanded()) {	/* If expanded, then we must use the exp. */
    xl = at1->lowlimit()+1;	/* limits in the window attributes object */
    xh = at1->highlimit()+1; // +1 because of root underflow chan.
  }				/* xl,xh now define the area of interest. */

  /* Determine the reduction parameters and method and instantiate a sampler */
  /* Note that if things are flipped, then we must generate a sampler        */
  /* using the Y axis resolution.                                            */
  
  Sampler *sample;		/* Will point to the instantiated sampler */
  int nchans = (xh - xl);           // # of channels tot plot
		nchans = (nchans >= 0) ? nchans : -nchans;  //  could have been negative...
		nchans++;                      // We plot to the end of the last channel.
  while(1) {
    if(at1->isflipped()) {
      sample = Xamine_GenerateSampler(xamine_shared->getbase(spno),
				      xamine_shared->gettype(spno),
				      at1->getrend(),
				      at1->getreduction(),
				      xl,xh,ny,
				      &chanw);
    }
    else {
      sample = Xamine_GenerateSampler(xamine_shared->getbase(spno), // Base ptr
				      xamine_shared->gettype(spno),// spec type
				      at1->getrend(),  // Spectrum rendition
				      at1->getreduction(), // reduction mode
				      xl,xh, nx, // AoI and geom. constraints
				      &chanw); // Width of a channel in pixels
    }
    

    /* Determine the rendition style and instantiate a channel drawer. */
    /* The drawer can be modified by the flippedness of the spectrum:  */
    Channel *drawer;
    switch(at1->getrend()) {
    case histogram:
      if(at1->isflipped()) {
							drawer = (Channel *) new HChannelF(
								d, win, gc, orgx, orgy, orgy+ny, nchans
							);
      }
      else {
							drawer = (Channel *) new HChannel(
									d, win, gc, orgx,orgy, orgx+nx, nchans
							);
      }
      break;
    case lines:
      if(at1->isflipped()) {
								drawer = (Channel *)new LChannelF(
										d, win, gc, orgx, orgy, orgy+ny, nchans
								);
      }
      else {
									drawer = (Channel *) new LChannel(
										d, win, gc, orgx,orgy, orgx+nx, nchans
									);
      }
      break;
    case points:
      if(at1->isflipped()) {
								drawer = (Channel *) new PChannelF(
											d,win, gc, orgx, orgy, orgy+ny, nchans
								);
      }
      else {
									drawer = (Channel *) new PChannel(
												d, win, gc, orgx, orgy, orgx+nx, nchans
									);
      }
      break;
    default:
      fprintf(stderr, "Invalid 1-d rendition type detected in Plot1d %d\n",
	      at1->getrend());
      exit(-1);
    }
    
    /* Draw the spectrum.  */
    
    sample->setsample(xl);
    
    if(at1->isflipped()) {
      Plot(drawer, sample, chanw, ny,nx, at1);
    }
    else {
      Plot(drawer, sample, chanw, nx, ny, at1);
    }
    
    /* Release the drawer and sampler  */
    
    delete drawer;
    delete sample;
    
    /* If there are more spectra then do them too: */
 
    int last = supers.Last();
    if(last) break;	/* Break out of loop after last one... */
    Superposition &s = supers.Next(); /* Get next superposition */
    while(!last) {
      spno = s.Spectrum();
      if((xamine_shared->gettype(spno) == undefined) || !(
	 (xamine_shared->gettype(spno) == onedword)  ||
	 (xamine_shared->gettype(spno) == onedlong) ) )  {
	supers.DeleteCurrent();      // The superposed spectrum vanished.
	last = supers.Last();
      }
      else {
	break;
      }
    }
    if(last) break;
    gc = Xamine_MakeDrawingGc(d, pane, s.Representation());

  }

  /* Return indicating that the whole spectrum has been redrawn */
  return True;
}



