/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   scaling.cc
**     This file contains code to compute the full scale of a spectrum
**     prior to display.  This only needs to be done if autoscaling is
**     enabled for the spectrum.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Includes:
*/

#include "XMWidget.h"
#include "dispwind.h"
#include "scaling.h"
#include "dispshare.h"
#include "axes.h"
/*
** External references
*/

extern spec_shared *xamine_shared;
/*
** Static local storage:
*/
/* The tables below indicate the number of pixels required to represent
** a channel of each type of rendition
*/

int onedpixtable[] = { 
                        SMOOTH1D_PIXELS, 
			HISTO1D_PIXELS, 
			LINES1D_PIXELS, 
		        POINTS1D_PIXELS
			};
int twodpixtable[] = { SCATT2D_PIXELS,
		       BOXES2D_PIXELS,
		       COLOR2D_PIXELS,
		       CONTOUR2D_PIXELS,
		       SURFACE2D_PIXELS,
		       LEGO2D_PIXELS
		       };

/*
** Functional Description:
**    Xamine_GenerateSampler:
**      This function generates the appropriate sampler object given the
**      area of interest, number of available pixels and reduction mode.
**      It also returns the width of a channel in floating point pixels.
**      If the display does not require sampling, then we're going to 
**      return a sampler that samples words or longwords with a step
**      of one channel... e.g. returns every channel.
** Formal Parameters:
**   unsigned int *b:
**      Pointer to channel zero of the spectrum.
**   spec_type    st:
**      Spectrum type (number of bytes per channel is what we're really after).
**   rendition_1d rend:
**      The rendition (affects the minimal channel size.
**   reduction_1d sr:
**      Spectrum reduction... indicates along with the spec_type which
**      sampler we need to chose if sampling is required.
**   int xl,xh:
**      Defines the area of interest.
**   int nx:
**      The number of available pixels in which to draw the Area of Interest.
**   float *chanw:
**      Returned... the number of pixels that each channel will use.
**      This is done F.P to prevent shortened spectra.
** Returns:
**   Pointer to a sampler object of the appropriate type.
*/
Sampler *Xamine_GenerateSampler(volatile unsigned int *b, 
				spec_type st, rendition_1d rend,
				reduction_mode sr, int xl, int xh, int nx,
				float *chanw)
{
  /* First determine what the channel width is and if sampling is needed */

  *chanw = (float)nx/(xh-xl+1);                   /* Default chan */

  /* If reduction is not required, then jimmy up a simple sampler that
  ** samples all channels:
  */
  if(*chanw > (float)onedpixtable[rend]) {            
    switch(st) {
    case onedlong:
      return (Sampler *)new Samplel(b);
    case onedword:
      return (Sampler *)new Samplew((unsigned short *)b);
    default:
      fprintf(stderr, "*BUG* in Plot1d/GenerateSampler.  Invalid spectrum type %d\n",
	      st);
      exit(-1);
    }
  }
  /* IF reduction is required, then the width becomes the minimum allowed
  ** pixel width and we need to chose an appropriate reduction method and step.
  */
  else {
    float step;
    step     = (float)onedpixtable[rend]/(*chanw);
    *chanw   = (float)onedpixtable[rend];             /* Pixels per channel. */
    switch(sr) {		                    /* Chose reduction type  */
    case sampled:		/* Reduce by sampling. */
      switch(st) {		/* Chose spectrum type... */
      case onedlong:
	return (Sampler *)new Samplel(b, step);
      case onedword:
	return (Sampler *)new Samplew((unsigned short *)b, step);
      default:	
	fprintf(stderr, "*BUG* in Plot1d/GenerateSampler invalid spectype %d\n",
		st);
	exit(-1);
      }
    case summed:		/* Reduce by summing */
      switch(st) {		/* Chose spectrum type: */
      case onedlong:
	return (Sampler *)new Suml(b, step);
      case onedword:
	return (Sampler *)new Sumw((unsigned short *)b, step);
      default:
	fprintf(stderr,
		"*BUG* in Plot1d/GenerateSampler invalid spectrum type %d\n",
		st);
	exit(-1);
      }
    case averaged:		/* Reduce by averaging */
      switch(st) {		/* Chose spectrum type: */
      case onedlong:
	return (Sampler *)new Avgl(b, step);
      case onedword:
	return (Sampler *)new Avgw((unsigned short *)b, step);
      default:
	fprintf(stderr,
		"*BUG* in Plot1d/GenerateSampler invalid spectrum type %d\n",
		st);
	exit(-1);
      }
    default:			/* Error. */
      fprintf(stderr,
	      "*BUG* in Plot1d/GenerateSampler invalid sampler type %d\n",
	      sr);
      exit(-1);
    }
  }
  return (Sampler *)NULL;
}

/*
** Functional Description:
**  Scale1d:
**     This is done in a very generic manner using the sampler
**     objects to do the actual channel selection depending on
**     the reduction mode. The function of Scale1d is to
**     select an appropriate sampler (using Xamine_GenerateSampler)
**     and then to go over the area of interest looking for the
**     biggest channel value using the sampler to get 'channel
**     values'.
** Formal Parameters:
**   int specid:
**     Spectrum number.
**   win_1d *att:
**     Attributes of the pane.
**   int resolution:
**     number of pixels available for the display.
*/
int Scale1d(int specid, win_1d *att, int resolution)
{

  /* First we need to determine the area of interest... this is either the
  ** entire spectrum if it isn't expanded or the expansion region.
  ** The area of interest will be stored in chanl, chanh
  */

  int chanl, chanh;
  chanl = 0;			/* Start at channel zero. */
  chanh = xamine_shared->getxdim(specid) - 1; /* Last channel is dim -1. */
  if(att->isexpanded()) {
    chanl = att->lowlimit();
    chanh = att->highlimit();
  }
  /* One of the things we don't want to do is auto scale based on the
  ** artificial peaks that sometimes inhabit the ends of the spectra:
  */
  if(chanl == 0) chanl = 1;
  if(chanh == xamine_shared->getxdim(specid)-1) chanh--;
  /*
  ** We now have enough information to generate a sampler:
  */
  Sampler *channels;
  float   channel_width;
  channels = 
    Xamine_GenerateSampler((unsigned int *)xamine_shared->getbase(specid),
			   xamine_shared->gettype(specid),
			   att->getrend(),
			   att->getreduction(),
			   chanl, chanh, resolution,
			   &channel_width);

  /*  Using the sampler, we iterate over the area of interest to get
  **  the maximum value into the scale variable.
  */

  float pix1 = channel_width/2.0;
  float pix2 = (float)resolution;
  unsigned int   scale  = 0;
  channels->setsample(chanl);           /* Set the starting location. */
  while(pix1 <= pix2) {
	unsigned int val = channels->sample(); /* Get a channel..            */
	if(val > scale) scale = val;         /* If necessary update the scale */
	pix1 += channel_width;
  }

  /*  Now that we're done, delete the sampler: */

  delete channels;

  /* return the result:  */

  if(scale <= 0) scale = 1;      /* Prevent integer zero divides later */
  return scale;
}

/*
** Functional Description:
**   Xamine_GenerateSampler2:
**      This function selects and creates a spectrum channel sampler 
**      appropriate to the spectrum given some attributes of the spectrum,
**      the shape of the spectrum and the shape of the region it has to fit
**      within.
** Formal Parameters:
**   int specno:
**       Spectrum number of the spectrum of interest.
**   spec_type st:
**      The type of the spectrum (must be either twodword or twodbyte).
**   rendition_2d rend:
**       The rendition of the spectrum.  This determines the actual number
**       of displayable channels since it determines the real estate that
**       a single channel will eat.
**   reduction_mode sr:
**       The method of reducing the spectrum to fit within the available
**       real estate.  This along with the spectrum type determine
**       which sampler will be chosen.
**   int xl,xh, yl,yh:
**       Define the region of interest to be displayed.
**   int nx,ny:
**       The number of x and y pixels of resolution that we have to display
**       the area of interest.
**   float *chanwx, *chanwy:
**       Returned... the number of pixels per channel of display in the X and
**       Y directions.
*/
Sampler2 *Xamine_GenerateSampler2(int specno,
				  spec_type st,
				  rendition_2d rend,
				  reduction_mode sr,
				  int xl, int xh, int yl, int yh,
				  int nx, int ny,
				  float *chanwx, float *chanwy)
{
  /*
  ** First we determine the number of pixels available for each
  ** channel of the spectrum (chanwx and chanwy).  If this is greater than
  ** the minima in twodpixtable, we hokey up a Sample2x with spacing of
  ** 1 in each direction.  If not, then we force the widths to the minimal
  ** spacing and figure out which of the samplers we really should be using.
  */
  float wx, wy;			/* Widths of x/y channels. */
  int   must_reduce = 0;
  wx = (float)nx/((float)(xh-xl+1));
  wy = (float)ny/((float)(yh-yl+1));
  *chanwx = wx;			/* Save the default channel sizes. */
  *chanwy = wy;
  if(wx < twodpixtable[rend]) {
    must_reduce = -1;		/* Indicate that reduction is needed. */
    wx = twodpixtable[rend];	/* Set the final channel width. */
  }
  if(wy < twodpixtable[rend]) {
    must_reduce = -1;
    wy  = twodpixtable[rend];
  }


  /* Now we selecte a reduction method.  If must_reduce is false then we
  ** can use an unreduced representation which is represented by a sampler
  ** constructed to return all channels in the area of interest.
  */
  Sampler2  *sampler;
  if(!must_reduce) {
    switch(st) {
    case twodword:
      sampler = (Sampler2 *)new Sample2w((unsigned short *)
					    xamine_shared->getbase(specno),
					 1.0, 1.0, 
					 xamine_shared->getxdim(specno));
      break;
    case twodbyte:
      sampler = (Sampler2 *)new Sample2b((unsigned char *)
					    xamine_shared->getbase(specno),
					 1.0, 1.0, 
					 xamine_shared->getxdim(specno));
      break;
    default:
      fprintf(stderr, "*BUG* -- Invalid spectrum type in GenerateSampler2 %d\n",
	      st);
      exit(-1);
    }
    sampler->setsample(xl, xh, yl); /* Set the region of interest params. */
  }
  else {
    /* If control passes here then we have to do some form of reduction.
    ** First we need to determine what the X/Y bin sizes are for the re-binned
    ** histogram.  Next we need to select an appropriate sampler object based
    ** on both the reduction method selected and the histogram type.
    */
    float xstep,ystep;
    xstep = (float)wx/(*chanwx); /* Note that for reduced axes, wx or wy */
    ystep = (float)wy/(*chanwy); /* comes from twodpixtable[] lookup. */
    *chanwx= wx;		/* Update the user's channel widths */
    *chanwy= wy;		/* to match the current ones. */
    switch(sr) {
    case sampled:
      switch(st) {
      case twodword:
	sampler = (Sampler2 *)new Sample2w((unsigned short *)
					      xamine_shared->getbase(specno),
					   xstep, ystep, 
					   xamine_shared->getxdim(specno));
	break;
      case twodbyte:
	sampler = (Sampler2 *)new Sample2b((unsigned char *)
					      xamine_shared->getbase(specno),
					   xstep,ystep,
					   xamine_shared->getxdim(specno));
	break;
      default:
	fprintf(stderr,
		"*BUG* detected in GenerateSampler2 invalid spectype %d\n",
		st);
	exit(-1);
      }
      break;
    case summed:
      switch(st) {
      case twodword:
	sampler = (Sampler2 *)new Sum2w((unsigned short *)
					   xamine_shared->getbase(specno),
					xstep, ystep, 
					xamine_shared->getxdim(specno));
	break;
      case twodbyte:
	sampler = (Sampler2 *)new Sum2b((unsigned char *)
					   xamine_shared->getbase(specno),
					xstep,ystep,
					xamine_shared->getxdim(specno));
	break;
      default:
	fprintf(stderr,
		"*BUG* detected in GenerateSampler2 invalid spectype %d\n",
		st);
	exit(-1);
      }
      break;
    case averaged:
      switch(st) {
      case twodword:
	sampler = (Sampler2 *)new Average2w((unsigned short *)
					       xamine_shared->getbase(specno),
					    xstep, ystep, 
					    xamine_shared->getxdim(specno));
	break;
      case twodbyte:
	sampler = (Sampler2 *)new Average2b((unsigned char *)
					       xamine_shared->getbase(specno),
					    xstep,ystep,
					    xamine_shared->getxdim(specno));
	break;
      default:
	fprintf(stderr,
		"*BUG* detected in GenerateSampler2 invalid spectype %d\n",
		st);
	exit(-1);
      }
      break;
    default:
      fprintf(stderr,
	      "*BUG* GenerateSampler2 detected invalid spectrum rendition %d\n",
	      sr);
      exit(-1);
    }
    sampler->setsample(xl,xh,yl);
  }
  return sampler;
}

/*
** Functional Description:
**   Scale2d:
**     This function oversees the scaling of 2-d spectra.  The main
**     purpose of this function is to select the appropriate reducing
**     scaling function (Scale2dSampled or Scale2dSummed) and to determine
**     the actual channel limits within which to do the scaling.
** Formal Parameters:
**   int specid:
**      The number of the spectrum to display.
**   win_2d *att:
**      Pointer to the display attributes object which controls the display
**      properties of the pane.
**   int resx, resy:
**      The number of pixels of resolution in the X and Y direction.
**      These must also be converted into channels using the twodpixtable
**      lookup table.
*/
unsigned int Scale2d(int specid, win_2d *att, int resx, int resy)
{
  /* Figure out the channel limits for the scaling (including any expansion) */

  int chanxl, chanyl, chanxh, chanyh;

  chanxl = 0;
  chanxh = xamine_shared->getxdim(specid) - 1;

  chanyl = 0;
  chanyh = xamine_shared->getydim(specid) - 1;
  if(att->isexpanded()) {
    chanxl = att->xlowlim();
    chanyl = att->ylowlim();
    chanxh = att->xhilim();
    chanyh = att->yhilim();
  }
  /* Generate a sampler which will be used to provide channel values for the
  ** scaling process:
  */
  float wx,wy;			/* pixel widths. */
  Sampler2 *sampler = Xamine_GenerateSampler2(specid,
					      xamine_shared->gettype(specid),
					      att->getrend(),
					      att->getreduction(),
					      chanxl, chanxh,
					      chanyl, chanyh,
					      resx,resy,
					      &wx, &wy);
  /* Now sum over the pixel map: */

  float x,y;
  float xl,yl;

  xl = (float)resx;
  yl = (float)resy;

  int   val;
  unsigned int full_scale = 0;
  for(y = wy/2.0; y < yl; y+=wy) {
    for(x = wx/2.0; x < xl; x+=wx) {
      val = sampler->sample();
      if(val > full_scale)
	full_scale = val;
    }
  }
  /* Destroy the sampler on exit and return the scale value: */
  delete sampler;
  if(full_scale <= 0) full_scale = 1;    /* Prevent divide by zero later */
  return full_scale;
}

/*
** Functional Description:
**   Xamine_ComputeScaling:
**     Compute the full scale value for a display spectrum segment.
** Formal Parameters:
**    win_attributed *att:
**      Spectrum display attributes includes the spectrum number,
**      the scaling mechanism and the display limits.
**    XMWidget *pane:
**      The pane to display in.  This is used to determine the reduction.
** Returns:
**      The value of the full scale.
*/
unsigned int Xamine_ComputeScaling(win_attributed *att, XMWidget *pane)
{
  int specid;
  Dimension nx, ny;

  /* Figure out the drawing rectangle size in pixels.  */

  pane->GetAttribute(XmNheight, &ny);
  pane->GetAttribute(XmNwidth,  &nx);
  specid = att->spectrum();

  /* The major branch is based on the dimensionality of the spectrum: */

  int resolution = nx;
  int fs;
  if(att->is1d()) {		/* 1d spectrum */
	 if(att->isflipped()) resolution = ny;
	 if(att->showaxes())
		resolution = (int)((float)resolution*(1.0 - XAMINE_MARGINSIZE));
	 fs = Scale1d(specid, (win_1d *)att,
			  resolution);
	 if(fs <= 0) fs = 1;
	 fs = (int)((float)fs*1.1); /* Give some margin for the top. */
	 return fs;
  }
  else {			/* 2d spectrum */
    int resx = nx;
    int resy = ny;
    if(att->isflipped()) {
      resx = ny; 
      resy = nx;
    }
    if(att->showaxes()) {
      resx = (int)((float)resx*(1.0 - XAMINE_MARGINSIZE));
      resy = (int)((float)resy*(1.0 - XAMINE_MARGINSIZE));
    }
    else if(att->showname()    || att->shownum()   ||
	    att->showdescrip() || att->showpeak()  || /* If only titles... */
	    att->showupdt()    || att->showlbl()) {
      resy = (int)((float)resy*(1.0 - XAMINE_MARGINSIZE/2.0));
    }
    fs = Scale2d(specid, (win_2d *)att, resx, resy);
    if(fs <= 0) fs = 1;
    return fs;
  }
}
