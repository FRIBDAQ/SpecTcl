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
**   colormgr.cc:
**    This file implements the color table manager.   Clients should refer
**    to colormgr.h for exported items.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

#include <config.h>

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>

#include <stdlib.h>
#include "colormgr.h"

/*
** Local storage:
*/

static unsigned int planes = 1;	       /* Total number of planes in widgets. */
static unsigned int numcolors = 0;     /* Total number of colors we're using */
static unsigned long *pixels = NULL;   /* Pointer to pixel values.           */
static unsigned long pctpixels[101];   /* Lookup table for % of FS values.   */
static unsigned long planemasks = 0; /* Will contain an or of the plane masks */
static Colormap colormap_id;	/* Id of color map used by widgets.  */


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      ConvertMask:
**	   Converts a color pixel mask into a multiplier and a
**	   max_value.
**
**  FORMAL PARAMETERS:
**
**      unsigned long *max:
**	    Maximum value.
**	unsigned long *mult:
**	    Shift multiplier.
**	unsigned long mask:
**	    Color mask.
**
**--
*/
static void ConvertMask(unsigned long *max, unsigned long *mult,
		        unsigned long mask)
{
    unsigned long mu = 1;
    long          tb = 1;	    // Will be just the top bit set...

    //
    //  Behave consistently if uselessly if the mask is zero.
    //	
    if (mask == 0)
    {
    	*max = 0;
	*mult = 1;
	return;
    }
    while(tb > 0) {		    // Create mask with only top bit set.
	tb = tb << 1;
    }
    while ((mask & 1) == 0)	    // Right justify mask.
    {
    	mu = mu	 << 1;		    // While computing multiplier.
	mask = (mask >> 1) & (unsigned)(~tb);
    }
    *max = mask;
    *mult= mu;
}

// Count # bits set in a mask
//
static int countbits(unsigned long mask)
{
  int nbits = 0;
  int bit  = 1;
  while(bit) {
    if (mask & bit) {
      nbits++;
    }
    bit = bit << 1;
  }
  return nbits;
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      ComputeDirectColor:
**	   This function computes the pixel value of an r,g,b triplet given
**	   A direct map or True color map description.
**
**  FORMAL PARAMETERS:
**
**      XStandardColormap *cm:
**	    Points to the color map description structure.
**	int r,g,b:
**	    The color in red,green,blue terms.
**
**  RETURN VALUE:
**
**      THe pixel which corresponds to the designated color.
**
**--
*/
static unsigned long ComputeDirectColor(XStandardColormap *cm,
					int r, int g, int b)
{
    unsigned long red = ((unsigned long)r*cm->red_max)/100;
    unsigned long green = ((unsigned long)g*cm->green_max)/100;
    unsigned long blue  = ((unsigned long)b*cm->blue_max)/100;

    unsigned long pixel = cm->base_pixel
			+ cm->red_mult*red
			+ cm->blue_mult*blue
			+ cm->green_mult*green;

    return pixel;
}

/*
** Functional Description:
**   GetWidgetColorParams:
**     This function takes a window and returns information about the colormap
**     and depth of the widget.
** Formal Parameters:
**   Display *d:
**     Display identifier.
**   Window w:
**     Window we want the info from.
**   Colormap *cmap:
**     Returned... color map id for associated window.
**   int *nplanes:
**     Returned... Number of planes (depth) of the window.
*/
static void GetWidgetColorParams(Display *d, Window w, 
				 Colormap *cmap, unsigned int *nplanes)
{
  XWindowAttributes attributes;

  XGetWindowAttributes(d, w, &attributes);

  *cmap     = attributes.colormap;
  *nplanes  = attributes.depth;

}

/*
** Functional Description:
**  AllocateColorMapEntries:
**    Allocates an appropriate number of color map entries for Xamine to use.
**    An appropriate number is determined as follows:
**     Maximum number of planes is the smaller of the depth/2 and 
**     XAMINE_MAXPLANES.
**     If this number of planes is not available, then we keep trying one
**     fewer until we succeed or until XAMINE_MINPLANES is reached.
**  Formal Parameters:
**    Display *d:
**      Pointer to the display structure which defines our server.
**    Colormap cmap:
**      Color map we're diddling with.
**    unsigned int planes:
**      Total number of planes in the color map.
**    unsigned int *ourplanes:
**      Returned... Number of planes we were able to get.
**    unsigned long **pixels:
**      Pointer to an array of pixels received.
*/
static void AllocateColorMapEntries(Display *d, Colormap cmap,
				    unsigned int planes,
				    unsigned int *ourplanes,
				    unsigned long **pixels)
{

  /* First determine the maximum number of colors we'll try for: */

  planes = planes - 1;		/* Only go for half the map. */
  if(planes > XAMINE_MAXPLANES) planes = XAMINE_MAXPLANES;

  /* The loop below loops over the set of plane values between
  ** planes down to XAMINE_MINPLANES trying to allocate pixels.
  ** On success we return the appropriate stuff.  On failure, we
  ** fall out and return 1 for ourplanes.
  */

  unsigned long plane_masks[XAMINE_MAXPLANES];
  unsigned int  numpixels;

  for(int i = planes; i >= XAMINE_MINPLANES; i--) {
    numpixels = 1 << i;		/* This is the number of pixels we want. */
    *pixels = new unsigned long[numpixels]; /* Allocate pixel storage */
    if(XAllocColorCells(d, cmap, False, plane_masks, 0,
			*pixels, numpixels)) {
      *ourplanes = i;		/* We got this many planes. */
      planemasks = 0xffffffff;
      return;			/* *pixels gets filled in by the alloc. */
    }
    delete [](*pixels);		/* Delete storage prior to next try. */

  }

  /* Control passes here if we can't get the appropriate number of planes
  ** or if planes < XAMINE_MINPLANES (e.g. monochrome server).
  ** we return NULL for pixels and 1 for ourplanes (2 colors).
  */

  *pixels    = NULL;
  *ourplanes = 1;

}

/*
** Functional Description:
**  OpenColorTable:
**   This function locates and opens the approprate color tabel file.
**   This is done by searching first environment variables, for the directory
**   path, then the user's home directory and finaly system default directories
**   for an appropriate color table file.  See 
ReadColorMap for a description
**   of what an appropriate color table file is.
** Formal Parameters:
**   int planes:
**     Number of color planes in our segment of the colorm map.
** Returns:
**   FILE *: Pointer to file stream data block open on the color table file
**           else NULL for failure.
*/
FILE *Xamine_OpenColorTable(unsigned int planes)
{
  char filename[BUFSIZ];	/* Build up filenames here. */
  const char *fmtstring = "%s/Xamine%d.ctbl"; /* Describe how to build up filename */

  /* First try environment path search: */
 
  char *env = getenv(XAMINE_COLOR_ENVIRONMENT);
  if(env != NULL) {
    sprintf(filename, fmtstring, env, planes);
    if(!access(filename, R_OK)) {
      printf("Color table %s  open via env variable\n", filename);
      fflush(stdout);
      return fopen(filename, "r");
    }
  }
  /*
  ** Next try the user's home directory: 
  */
  env = getenv("HOME");
  if(env != NULL) {		/* Should never fail but check anyway */
    sprintf(filename, fmtstring, env, planes);
    if(!access(filename, R_OK)) {
      printf("Opening colortable in home dir %s \n", filename);
      return fopen(filename, "r");
    }
  }
  /* Now try the system wide defaults: */

#ifndef HOME
  sprintf(filename, fmtstring, XAMINE_DEFAULT_COLOR_DIR, planes);
#else
  fmtstring = "%s/etc/Xamine%d.ctbl";
  sprintf(filename, fmtstring, HOME, planes);
  printf("Opening system colortable file %s\n", filename);
  fflush(stdout);
#endif
  return fopen(filename, "r");

}

/*
** Functional Description:
**    ReadColorMap:
**       This function reads the color level color map from file.  This process
**       involves determining what the actual full name of the color map is.
**       The color map name is composed of the following elements:
**          %s[/]Xamine%d.ctbl  Where:
**
**               system.
**           %d  Is the number of color planes in the color map that we will
**               use.
**        The [/] indicates that for UNIX systems a '/' character separates
**        the directory string from the filename section, while in VMS,
**        there is no such seperator character.
** Formal Parameters:
**   Display *d:
**     Display identifying the connection to the server.
**   Colormap cmap:
**     Color map ID of the color map we're loading.
**   unsigned int *pixels:
**     Set of pixels that we're allowed to use.
**   unsigned int *pixtable:
**     A pixel table that we will fill in indicating which pixel values to use
**     for which percentages of full scale.
**   int planes:
**     Number of color planes we're using.
*/
static void ReadColorMap(Display *d, Colormap cmap, unsigned long *pixels,
			 unsigned long *pixtable, int planes)
{
  /* Prepare for scaling by getting the color full scales: */

  XColor vwhite, white;
  XLookupColor(d, cmap, "white", &vwhite, &white);

  /* First we need to open the color table file:  */

  FILE *ctblfile;

  ctblfile = Xamine_OpenColorTable(planes);
  if(ctblfile == (FILE *)NULL) {
    perror("Xamine -- Unable to open colortable file");
    fprintf(stderr, "Attempted open of file for %d color planes\n", planes);
    exit(errno);
  }
  /* Read the first line of the color table file.  The number of entries must
  ** match the number of entries implied by the number of planes:
  */
  int entries;
  fscanf(ctblfile, "%d\n", &entries);
  if(entries != (1 << planes)) {
    fclose(ctblfile);
    fprintf(stderr, 
	    "Color table file Xamine%d.ctbl has %d entries, not %d entries\n",
	    planes, entries, (1 << planes));
    exit(-1);
  }
  /*
  ** Next read in each subsequent line of the file.  Each line contains
  **   low  high  read  green blue
  ** Where:
  **    low,high   - Represent the percentage limits of full scale represented
  **                 by that color.
  **    red,green,blue - are the RGB values for the color table entry.
  */
  unsigned long *p = pixels;	/* Pointer into pixel table. */
  for(int i = 0; i < entries; i++) {
    int lo,hi,
        r,g,b;
    XColor color;
    color.pixel = *p++;
    color.flags = (DoRed | DoGreen | DoBlue);
    if(fscanf(ctblfile, "%d %d %d %d %d\n",&lo, &hi, 
	      &r, &g, &b) != 5) {
      fprintf(stderr,
	      " Color table format error on line %d\n",
	      i + 2);
      fclose(ctblfile);
      exit(-1);
    }
    if( (lo > hi) ||  (lo < 0) || (hi > 100)) {
      fprintf(stderr, "Low/High range error on line %d of color table file\n",
	      i+2);
      fclose(ctblfile);
      exit(-1);
    }
    color.red   = (int)((float)r * (float)white.red / 100.0);
    color.green = (int)((float)g * (float)white.green/100.0);
    color.blue  = (int)((float)b * (float)white.blue/100.0);
    XStoreColor(d, cmap, &color); /* Set the colors: */
    for(int j = lo; j < hi; j++) {
      pixtable[j] = color.pixel;
    }
    if(hi == 100) pixtable[100] = color.pixel;
  }
  /*
  ** Close the color table file and return:
  */
  fclose(ctblfile);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      GetWidgetVisualInfo:
**	    Returns the visual information structure which describes
**	    the window corresponding to a widget.
**
**  FORMAL PARAMETERS:
**
**      Display *d:
**	    Pointer to the display/connection information.
**	Window win:
**          Identifies the window we're asking about.
**      XVisualInfo *vis:
**          Returned visual information structure.
**
**
**--
*/
static void GetWidgetVisualInfo (Display *d, Window w, XVisualInfo *vis)
{
    XWindowAttributes att;
    VisualID          vid;
    XVisualInfo       template_vis;
    XVisualInfo       *result;
    int                nitem;

    XGetWindowAttributes(d, w, &att);	    /* Get info about the window. */
    vid = XVisualIDFromVisual(att.visual);  /* Get the visual ID.         */
    /* 
    ** Now that we've tortured the visual ID on the window from the server
    ** We can get detailed information about the visual from the server
    ** about that visual:
    */
    template_vis.visualid = vid;
    //    result            = XGetVisualInfo(d, VisualIDMask,
    //				       &template_vis, &nitem);
    result = XGetVisualInfo(d, 0, &template_vis, &nitem);
//    if (nitem != 1)
//    {
//    	fprintf(stderr, 
// 	    "XGetVisualInfo from GetWidgetVisualInfo unexpected nitem = %d\n",
//	        nitem);
//	exit(-1);
//   }


    sleep(1);			// Let the rest of the crap come out.

    // We need to get the 'best visual'  This is defined as:
    // 1. If there are pseudo color visuals, the one with the
    //    largest number of bits per rgb.
    // 2. If there are no pseudo color visuals, the colormapped
    //    visual with the largest depth.
    //
    int besttrue   = -1;
    int besttruebits = -1;
    int bestpseudo = -1;
    int bestpseudodepth = 0;

    for(int i =0; i < nitem; i++) {
      if(result[i].c_class == PseudoColor) {
	if(result[i].depth > bestpseudodepth) {
	  bestpseudodepth = result[i].depth;
	  bestpseudo      = i;
	  fprintf(stderr, "Pseudo visual with depth: %d \n", bestpseudodepth);
	} 
      }
      if((result[i].c_class == TrueColor) ||
	 (result[i].c_class == DirectColor)) {
	int totalbits = countbits(result[i].red_mask)  +
	                countbits(result[i].blue_mask) +
	                countbits(result[i].green_mask);
	if(totalbits > besttruebits) {
	  besttruebits = totalbits;
	  besttrue     = i;
	  fprintf(stderr, "Direct or true visual with %d total bits\n", besttruebits);
	}
      }
    }
    
    if(besttrue >= 0) {		// Got a nice true color visual.
      fprintf(stderr, "Selected direct/true with %d bits\n", besttruebits);
      memcpy(vis, &(result[besttrue]), sizeof(XVisualInfo));
    }
    else if(bestpseudo >= 0) {	// Falling back to a nice pseudo
      fprintf(stderr, "Selected pseudo with depth %d\n", bestpseudodepth);
      memcpy(vis, &(result[bestpseudo]), sizeof(XVisualInfo));
    }

   // Nothing good: Take the first visual...

    else {
      fprintf(stderr, "Took nothing good!!\n");
      memcpy (vis,result,sizeof(XVisualInfo));
    }
    XFree((char* )result);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      SetupMappedColors:
**	    This function sets up the color table and pixel table for
**          a mapped color display.  
**
**  FORMAL PARAMETERS:
**
**      Display *d:
**          Pointer to display/connection iformation.
**      Window w:
**	    A representative window on the display.
**
**  NOTE:
**     After calling this it is still possible to end up in bitonal mode.
**     This happens if Xamine is unable to get sufficent color table entries
**     to support color usage.
**
**--
*/
static void SetupMappedColors (Display *d, Window w)
{

  /* Allocate the set of colors we'll actually be able to use: */

  unsigned int ourplanes;
  if(planes > XAMINE_MINPLANES) {
    AllocateColorMapEntries(d, colormap_id, planes, &ourplanes, &pixels);
    numcolors = (1 << ourplanes);
  }
  else {
    ourplanes = 1;
    pixels    = NULL;
    numcolors = 2;
  }
  /* Set up the color map.     */

  if(ourplanes > 1) {
    ReadColorMap(d, colormap_id, pixels, pctpixels, ourplanes);
  }

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      ReadDirectMap:
**	    Read in the colormap file corresponding to a direct mapped
**          visual.
**
**  FORMAL PARAMETERS:
**
**      XStandardColormap *mapinfo:
**         Points to the structure which describes the way the direct color
**         lookup works.
**
**  SIDE EFFECTS:
**
**      On success, planes <= XAMINE_MAXPLANES and
**                  numcolors <= 1 << XAMINE_MAXPLANES
**	            pixels    <= dynamically allocated array of pixel values
**                               actually used.
**                  pctpixels <= percent to pixel mapping.
**                  planemask <= all bits set.
**--
*/
static void ReadDirectMap (XStandardColormap *mapinfo)
{

    //  Open the color plane table. 
    //
    FILE *fp = Xamine_OpenColorTable(XAMINE_MAXPLANES);

    if (fp == (FILE *)NULL)
    {
    	perror("Xamine - Unable to open colortable file\n");
	fprintf(stderr, "Attempted open of file for %d color planes\n",
	       XAMINE_MAXPLANES);
	exit(errno);
    }
    //
    // Set up the color table description as all failures now are fatal
    //
    numcolors = 1 << XAMINE_MAXPLANES;
    pixels    = new unsigned long[numcolors];
    unsigned long       *p = pixels;
    planemasks = (unsigned long)-1;	// This has all bits set.
    //
    // Read the first line of the color table file.  It is the number of entries
    // in the colortable file and must match the number of entries implied
    // by the plane count.
    //
    int entries;
    fscanf(fp, "%d\n", &entries);
    if (entries != (numcolors))
    {
    	fclose(fp);
	fprintf(stderr,
		" Color table file Xamine%d.ctbl. has %d entries not %d\n",
		planes, entries, numcolors);
	exit(-1);
    }
    //	Subsequent lines of the color table are of the form:
    //		low  high red green blue
    //
    //  low,high    - represent the percentage limits of full scale.
    //	red,green,blue - are precentages of full scale red green blue values
    //		        to display for that range.
    //			
    for (int i = 0; i < entries; i++)
    {
    	int lo,hi;
	long r,g,b;
	if (fscanf(fp, "%d %d %ld %ld %ld\n", &lo, &hi,
			      &r, &g, &b) != 5)
	{
	    fprintf (stderr,
		     "Color table file format error line %d\n",
		     i+2);
	    fclose(fp);
	    exit(-1);
	}
	if ((lo > hi) || (lo < 0) || (hi > 100))    // Validate lo, hi.
	{
	    fprintf(stderr,
		    "Low/High Range error on line %d of color table file\n",
		    i+2);
	    fclose(fp);
	    exit(-1);
	}
	//  Figure out the pixel value corresponding to this color:
	//  Fill in the pixel entry in the 'color map' and also fill in the
	//  appropriate set of pixtable entries.
	//  
	*p = ComputeDirectColor(mapinfo, r,g,b);
	for (int j = lo;  j < hi;  j++)
	{
	    pctpixels[j] = *p;
	}
	if(hi == 100) pctpixels[100] = *p;
	p++;
    }
    fclose(fp);
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      SetupDirectColors:
**          This function sets up Xamine's color handling for direct lookup
**          style xserver visuals.  This includes DirectColor and TrueColor
**          visual types at thist time.
**
**  FORMAL PARAMETERS:
**
**      Display *d:
**          Pointer to display/connection information
**       
**      Window w:
**          Id of a typical window on the display used by Xamine/Motif.
**	 
**      XVisualInfo *vis:
**          Pointer to visual information structure describing the visual
**          That Motif is using on the window.
**
**--
*/
static void SetupDirectColors (Display *d, Window w, XVisualInfo *vis)
{
    XStandardColormap mymap;     // This is actually a fake.  
    //
    //  The mymap is really a fake StandardColormap which is used since
    //	it provides a much more convenient way to specify the shape of the
    //	direct color visual than the information in the XVisualInfo struct.
    //	The main purpose of this function is to map the masks into max, mult's
    //	and so on.
    //	
    //	    First fill in the no-brainer fields:
    //	    
    mymap.visualid   = vis->visualid;
    mymap.base_pixel = 0;
    //
    //   For each color, convert the mask into a multiplier and max field:
    //	 
    ConvertMask(&mymap.red_max, &mymap.red_mult, vis->red_mask);
    ConvertMask(&mymap.blue_max, &mymap.blue_mult, vis->blue_mask);
    ConvertMask(&mymap.green_max, &mymap.green_mult, vis->green_mask);
    fprintf(stderr, "red_max = %d blue_max = %d green_max = %d\n",
	    mymap.red_max, mymap.blue_max, mymap.green_max);
    //
    //   Then ReadDirectMap does the remainder of the job:
    //
    ReadDirectMap(&mymap);
    return;		    /* Falling through with no match remains bitonal */
			    /* Breaking out with a match gives direct color  */
}


/*
** Functional Description:
**   Xamine_InitColors:
**      This function initializes the color map manager.  This involves
**      figuring out color map ID, the depth of the windows and number of
**      free color planes.  We then read in the Xamine color map following the
**      rules for default color map behavior.
** Formal Parameters:
**      XMWidget *w:
**        A widget anywhere in the widget heirarchy tree which
**        Xamine's widgets are running in.
*/
void Xamine_InitColors(XMWidget *w)
{
  Display *d;			/* Display of the connection. */
  Window  win = XtWindow(w->getid());
  XVisualInfo  vis;

  d = XtDisplay(w->getid());	        /* Get the display id. */

  /* Determine the color parameters: */

  GetWidgetVisualInfo(d,win, &vis);
  GetWidgetColorParams(d, win, &colormap_id, &planes);
  /*
  **   Depending on the visual class, we initialize the Xamine color system
  **   in one of three ways:
  **      Mapped color:   There's a color map which can be diddled by
  **                      Xamine (GrayScale and PseudoColor).
  **      Direct Lookup:  Pixels are broken in to red, green and blue fields
  **                      which in turn are directly (or indirectly) sent to
  **                      the DAC... the assumption is that DirectColor visuals
  **                      will probably have monotonically increasing
  **                      table values (DirectColor, TrueColor)
  **      Bitonal:        Only the fg and bg colors are used (StaticGray,
  **                      StaticColor).
  */
  switch (vis.c_class)
  {
	/*		Mapped color usage.     */
      case GrayScale:
      case PseudoColor:
	fprintf(stderr, "Setting up  pseudo color planes = %d\n", vis.depth);
	  SetupMappedColors(d,win);
	  break;
      case DirectColor:
      case TrueColor:
	fprintf(stderr, "Setting up Direct/True:\n");
	  pixels    = NULL;	/* This assumes we can't get a good true */
	  numcolors = 2;	/* color match and may be overridden     */
	  SetupDirectColors(d,win, &vis);
          break;
      /*		 Bitonal color usage.  */
      case StaticGray:
      case StaticColor:
      default:
	fprintf(stderr, "Bitonal!!\n");
	  pixels    = NULL;
	  numcolors = 2;
	  break;
  }

}

/*
** Functional Description:
**   Xamine_ColorDisplay:
**     Determines if the current display is color.  This requires that
**     Xamine_InitColors had been previously called.
** Returns:
**   TRUE   - if the display is color
**   FALSE  - If the display is not color.
*/
Boolean Xamine_ColorDisplay()
{
  return (numcolors > 2);
}

/*
** Functional Description:
**   Xamine_NumColors:
**     Returns number of color levels available for color intensity plots.
*/
unsigned int Xamine_NumColors()
{
  return numcolors;
}
/*
** Functional Description:
**   Xamine_ColorDepth:
**     Returns the number of color planes associated with our colors.
*/
unsigned int Xamine_ColorDepth()
{
  return planes;
}

/*
** Functional Description:
**   Xamine_GetPixel:
**     This function looks up the pixel value associated with level idx.
**     Out of range values are pushed to the nearest edge.
** Formal Parameters:
**   int idx:
**     Color table entry number we're looking for.
** Returns:
**   unsigned long:  A pixel value.
*/
unsigned long Xamine_GetPixel(int idx)
{
  if(!Xamine_ColorDisplay()) {
    fprintf(stderr, 
	    "*BUG* Detected in colormgr -- attempted color lookup on B/W \n");
    exit(-1);
  }
  if(idx < 0) return pixels[0];
  if(idx >= numcolors) return pixels[numcolors-1];
  return pixels[idx];

}
/*
** Functional Description:
**   Xamine_GetColorTable:
**    Returns a pointer to the pixel table.
*/
unsigned long *Xamine_GetColorTable() 
{
  return pixels; 
}
/*
** Functional Description:
**   Xamine_PctToPixel:
**     Translates a percentage of fullscale to a pixel value.
** Formal Parameters:
**   int val:
**     Percentage of full scale... forced to [0,100]
** Return:
**    unsigned long: Pixel value
*/
unsigned long Xamine_PctToPixel(int val)
{
  if(val < 0) val = 0;
  if(val > 100) val = 100;
  return pctpixels[val];
}
/*
** Functional Description:
**   Xamine_GetColormap:
**     returns colormap ID of color map used for Xamine colors.
*/
Colormap Xamine_GetColormap()
{
  return colormap_id;
}

/*
** Functional Description:
**    Xamine_GetVisual:
**     Gets the visual structure pointer that describes the
**     window drawing.
** Formal Parameters:
**    Display *d:
**      Points to the display connection identification structure.
**    Window w:
**      A window to get the info for.
*/
Visual *Xamine_GetVisual(Display *d, Window w)
{
  XWindowAttributes att;

  if(XGetWindowAttributes(d, w, &att) == 0) {
    fprintf(stderr, 
         "*BUG* In Xamine_GetVisual... call to XGetWindowAttributes failed\n");
    exit(-1);
  }
  return att.visual;
}

/*
** Functional Description:
**   Xamine_GetXorDrawingColor:
**     Returns a color pixel suitable for use with XOR functions as an
**     overlay color.
** Formal Parameters:
**    NONE
** Returns:
**    A pixel value.
*/
unsigned long Xamine_GetXorDrawingColor()
{
  unsigned long fg,bg;
  
  if(numcolors <= 2) {
    
    assert( (sizeof(long) == 4) ||
	   (sizeof(long) == 8) ||
	   (sizeof(long)  == 2) ||
	   (sizeof(long) == 1));
    
    switch(sizeof(long)) {
    case 4:
      return 0xffffffff;
#ifdef HAVE_TRUE_SIXTY_FOUR   // OSF1
    case 8:
      return 0xffffffffffffffffl;
#endif
    case 2:
      return 0xffff;
    case 1:
      return 0xff;
    }
  }
  fg = Xamine_PctToPixel(100);	/* Treat high color as foreground. */
  bg = Xamine_PctToPixel(0);	/* Treat low color as background.  */

  return (fg ^ bg);
}

/*
** Functional Description:
**   Xamine_GetColorPlaneMask:
**     Retrieves a bit mask of the logical or of all plane masks used in
**     the user color table.  Note if the system is monochrome, then 
**     a mask with all bits set will be returned.
*/
unsigned long Xamine_GetColorPlaneMask() 
{
  assert((sizeof(long) == 4)  ||
	 (sizeof(long) == 8)  ||
	 (sizeof(long) == 2)  ||
	 (sizeof(long) == 1));
  if(numcolors <= 2) {
    switch(sizeof(long)) {
    case 4:
      return 0xffffffff;
#ifdef HAVE_TRUE_SIXTY_FOUR  // OSF1
    case 8:
      return 0xffffffffffffffff;
#endif
    case 2:
      return 0xffff;
    case 1:
      return 0xff;
    }
  }
  return planemasks;
}


/*
** Functional Description:
**   Xamine_HtsToColor:
**     Transforms an array of spectrum heights to colors.
** Formal Parameters:
**   unsigned int *hts:
**       Input:   Set of heights to transform.
**       Output:  Set of pixel values.
**   int full_scale:
**       Full scale height to scale against.
**   int npts:
**       Number of points to scale to color.
*/
void Xamine_HtsToColor(unsigned int *hts, int full_scale, int npts)
{
  while(npts > 0) {
    int pct = (*hts * 100)/full_scale;
    if((pct == 0) && (*hts != 0)) pct = 1;
    if(pct > 100) pct = 100;
    *hts++ = pctpixels[pct];
    npts--;
  }
}
