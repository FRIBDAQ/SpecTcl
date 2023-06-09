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
**      mapped coordinates to channel coordinates, and vice-versa.
** Author:
**   Jason Venema
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
#include <config.h>
#include <math.h>
#include "mapcoord.h"
#include "dispshare.h"
#include "transformations.h"
#include <panemgr.h>

#include <iostream>
#include <set>

extern volatile spec_shared *xamine_shared;


std::set<spec_type> OnedSpectrumTypes = {onedword, onedlong};
std::set<spec_type> TwodSpectrumTypes = {twodlong, twodword, twodbyte};

/**
 * channelWidth
 *   Figure out the width of a channel in real coordinates:
 *
 * @param lo - low limit real coordinates of axis.
 * @param hi - High limits of real coordinates of axis.
 * @param chans - Number of channels allocated for the range [lo hi).
 * @return float - width of a channel in real coordinates.
 */
double
channelWidth(double lo, double hi, double chans)
{
    return (hi - lo)/chans;  
}

// Return the x channel range of a spectrum.

static std::pair<int,int>
getXChannelRange(int specno, int row, int col)
{
// We need to get the win_attributed object corresponding to the
  // spectrum in order to figure out the range displayed if expanded.
  
  win_attributed* pAttrib = Xamine_GetDisplayAttributes(row, col);
  
  // Figure out the range of displayed channels.. full range unless expanded:
  // Default this to the full spectrum we eliminate the root over/underflow chans.
  // These are rerpesented in channel coordinates not channel numbers.
  
  int xl = 0;
  int xh = xamine_shared->getxdim(specno) - 2;
  if ((OnedSpectrumTypes.count(Xamine_SpectrumType(specno)) > 0) &&
      (reinterpret_cast<win_1d*>(pAttrib))->isexpanded()) {
      win_1d* p1d = dynamic_cast<win_1d*>(pAttrib);
      xl = p1d->lowlimit();
      xh = p1d->highlimit();
  } else if ((TwodSpectrumTypes.count(Xamine_SpectrumType(specno)) > 0) &&
              (reinterpret_cast<win_2d*>(pAttrib))->isexpanded()) {
      win_2d* p2d = dynamic_cast<win_2d*>(pAttrib);
      xl = p2d->xlowlim();
      xh = p2d->xhilim();
  }
  std::pair<int, int> result;
  result.first = xl;
  result.second = xh;
  return result;
}
static std::pair<int, int>
getYChannelRange(int specno, int row, int col)
{
  // We need to get the win_attributed object corresponding to the
  // spectrum in order to figure out the range displayed if expanded.
  
  win_attributed* pAttrib = Xamine_GetDisplayAttributes(row, col);
  
  // Figure out the range of displayed channels.. full range unless expanded:
  // Default this to the full spectrum we eliminate the root over/underflow chans.
  // These are rerpesented in channel coordinates not channel numbers.
  
  int yl = 0;
  int yh = xamine_shared->getydim(specno) - 2;
  win_2d* p2d = dynamic_cast<win_2d*>(pAttrib);
  
  if (p2d->isexpanded()) {
    // We can only be called for a 2-d Spectrum:
  
    
    yl = p2d->ylowlim();
    yh = p2d->yhilim();        
  
  }
  std::pair<int, int> result;
  result.first = yl;
  result.second = yh;
  return result;
}

/*!
   Do an arbitrary coordinate transformation:
   \param fSourcelow, fSourceHigh (float)
      Define a window in the source coordinate system.
   \param fDestLow, fDestHigh (float)
      Define the corresponding window in the destination
      coordinate system.
    \param point (float)
       The source coordinate.
    \return float
    \retval  the coordinate transformed as indicated by the
             two coordinate windows.
*/
float Transform(double fSourceLow, double fSourceHigh,
		double fDestLow,   double fDestHigh, 
		double point)
{
  double  fraction;
  if (fSourceHigh - fSourceLow != 0) {
    fraction = (point - fSourceLow)/(fSourceHigh - fSourceLow);
  } 
  else {
    fraction = 0.0;
  }
  
  return fDestLow + fraction*(fDestHigh - fDestLow);
}

/*
** Functional Description:
**   Xamine_XMappedToChan:
**     This function converts a mapped x-coordinate to the same coordinate in
**     channel space.
** Formal Parameters:
**   int specno
**     The spectrum number that the map is applied to
**   float value
**     The mapped coordinate to convert
** Returns:
**     The integer value of the channel which this mapped coordinate
**     corresponds to or -1.0 on failure
*/
int Xamine_XMappedToChan(int specno, double value)
{

  double xlo = xamine_shared->getxmin_map(specno);
  double xhi = xamine_shared->getxmax_map(specno);
  double nch = xamine_shared->getxdim(specno) -2;  // Remove root chans.
  return transform(value, xlo, xhi, 0, nch);
  

}

/*
** Functional Description:
**   Xamine_YMappedToChan:
**     This function converts a mapped y-coordinate to the same coordinate in
**     channel space.
** Formal Parameters:
**   int specno
**     The spectrum number that the map is applied to
**   float value
**     The mapped coordinate to convert
** Returns:
**     The integer value of the channel which this mapped coordinate
**     corresponds to, or -1.0 if failure.
*/
int Xamine_YMappedToChan(int specno, double value)
{


  double ylo = xamine_shared->getymin_map(specno);
  double yhi = xamine_shared->getymax_map(specno);
  double nch = xamine_shared->getydim(specno) -2; // Remove root chans.
  return transform(value, ylo, yhi, 0, nch);
}

/*
** Functional Description:
**   Xamine_XChanToMapped:
**     This function converts an x-channel to its mapped coordinate space value
** Formal Parameters:
**   int specno
**     The spectrum number that the map is applied to
**   float value
**     The channel number to convert
** Returns:
**     The floating point value of the mapped coordinate which this channel
**     corresponds to or -1.0 on failure
*/
float Xamine_XChanToMapped(int specno, double chan)
{
  double xlo = xamine_shared->getxmin_map(specno);
  double xhi = xamine_shared->getxmax_map(specno);
  int   nch = xamine_shared->getxdim(specno) - 2; // Remove root chans
  return transform(chan, 0, nch, xlo, xhi);
}

/*
** Functional Description:
**   Xamine_YChanToMapped:
**     This function converts a y-channel to its mapped coordinate space value
** Formal Parameters:
**   int specno
**     The spectrum number that the map is applied to
**   float value
**     The channel number to convert
** Returns:
**     The floating point value of the mapped coordinate which this channel
**     corresponds to or -1.0 on failure
*/
float Xamine_YChanToMapped(int specno, double chan)
{
  double ylo = xamine_shared->getymin_map(specno);
  double yhi = xamine_shared->getymax_map(specno);
  int   nch = xamine_shared->getydim(specno) -2;  // remove root chan.

  return transform(chan, 0, nch, ylo, yhi);
  

}
/**
 * Xamine_XChanToPixelRange
 *    Each X channel displayed takes up a range of pixels on the screen.
 *    Given the set of pixel coordinates that make up a spectrum's display
 *    area, this function returns that range.  We take into account any
 *    spectrum expansion that might be in effect as well.
 *
 *  @param specno - specturm number.
 *  @param row, col - Pane coordinates
 *  @param pixlow - Low pixel number that's used to display channels.
 *                  if the spectrum is flipped, this is the top pixel;
 *                  if not the left pixel.
 *  @param pixhigh - High pixel number that's used to display channels.
 *                  If the sepctrum si flipped, this is the bottom pixel;
 *                  if not the right pixel
 *  @param chan   - channel
 *  @returns std::pair<int, int> The low/high range of pixels taken up by the
 *                 chanel in it's axis direction.
 *          
 */
std::pair<int,int>
Xamine_XChanToPixelRange(int specno, int row, int col, int pixlow, int pixhigh, int chan)
{
    auto pxlo = xchan_to_xpixel(chan, row, col);
    auto pxhi = xchan_to_xpixel(chan+1, row, col);
    
  std::pair<int,int> result;
  result.first = pxlo;
  result.second = pxhi;
  return result;

}
/**
 * Xamine_YChanToPixelRange
 *    see Xamine_XChanToPixelRange... this does the same but for the Y channel
 *    of a 2-d spectrum.  For this to make sense, the spectrum must be one of a
 *    2-d spectrum.
 *    
 *  @param specno - specturm number.
 *  @param row, col - Pane coordinates
 *  @param pixlow - Low pixel number that's used to display channels.
 *                  if the spectrum is flipped, this is the top pixel;
 *                  if not the left pixel.
 *  @param pixhigh - High pixel number that's used to display channels.
 *                  If the sepctrum si flipped, this is the bottom pixel;
 *                  if not the right pixel
 *  @param chan   - channel
 *  @returns std::pair<int, int> The low/high range of pixels taken up by the
 *                 chanel in it's axis direction.
 *          
 */
std::pair<int,int>
Xamine_YChanToPixelRange(int specno, int row, int col, int pixlow, int pixhigh, int chan)
{
  std::pair<int,int> result;
  result.first = ychan_to_ypixel(chan, row, col);
  result.second = ychan_to_ypixel(chan+1, row, col);
  return result;
  
}
/**
 * Xamine_XPixelToChannel
 *    Given a pixel coordinate figures out the channel that pixel lives in.
 *    If the spectrum is flipped it's up to the caller to use the y pixel range.
 *
 *  @param specno    - Xamine Spectrum id.
 *  @param row, col  - Pane in which we're operating.
 *  @param pixlow, pixhigh - range of pixels that are used to plot the spectrum chans.
 *  @param x         - X-Pixel coordinate.
 *  @return int     - X Channel associated with that pixel. This will be the truncated
 *                    transform value. 
 *  
 */
int
Xamine_XPixelToChannel(
  int specno, int row, int col, int pixlow, int pixhigh, int x
)
{
    return xpixel_to_xchan(x, row, col);
  
}
/**
 * Xamine_YPixelToChannel
 *   Same as above but for a Y coordinate.
 */
int
Xamine_YPixelToChannel(
  int specno, int row, int col, int pixlow, int pixhigh, int y
)
{
    return ypixel_to_ychan(y, row, col);
  
}


