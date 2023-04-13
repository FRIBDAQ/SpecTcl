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
#include "transformations.h"
#include "mapcoord.h"
#include <iostream>
#include <math.h>
#include <stdint.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif




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
  
  uintptr_t index;
  pane->GetAttribute(XmNuserData, &index);
  int row = index/WINDOW_MAXAXIS;
  int col = index % WINDOW_MAXAXIS;
  int spec = attributes->spectrum();  

  
  int channel = xpixel_to_xchan(xpix, row, col);
  unsigned counts = spectra->getchannel(spec, channel);
  double ht       = ypixel_to_yaxis(ypix, row, col);
  
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
  uintptr_t index;
  pane->GetAttribute(XmNuserData, &index);
  int row = index/WINDOW_MAXAXIS;
  int col = index % WINDOW_MAXAXIS;
  
  *xpix = xchan_to_xpixel(chan, row, col);
  *ypix = yaxis_to_ypixel(counts, row, col);
  
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
  
  uintptr_t index;
  pane->GetAttribute(XmNuserData, &index);
  int row = index/WINDOW_MAXAXIS;
  int col = index % WINDOW_MAXAXIS;
  int spec = attributes->spectrum();
  
  // Now the X/Y coordinates are just linear transforms.
  // The counts are gotte4n from the channel indexes and the
  
  int cx = xpixel_to_xchan(xpix, row, col);
  int cy = ypixel_to_ychan(ypix, row, col);



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
  
  uintptr_t index;
  pane->GetAttribute(XmNuserData, &index);
  int row = index/WINDOW_MAXAXIS;
  int col = index % WINDOW_MAXAXIS;
  
  *xpix = xchan_to_xpixel(chanx, row, col);
  *ypix = ychan_to_ypixel(chany, row, col);
  
}
