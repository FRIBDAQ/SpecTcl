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

#include <iostream>

extern volatile spec_shared *xamine_shared;


/**
 * channelWidth
 *   Figure out the width of a channel in real coordinates:
 *
 * @param lo - low limit real coordinates of axis.
 * @param hi - High limits of real coordinates of axis.
 * @param chans - Number of channels allocated for the range [lo hi).
 * @return float - width of a channel in real coordinates.
 */
static float
channelWidth(float lo, float hi, float chans)
{
    return (hi - lo)/chans;  
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
float Transform(float fSourceLow, float fSourceHigh,
		float fDestLow,   float fDestHigh, 
		float point)
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
int Xamine_XMappedToChan(int specno, float value)
{
  float xlo = xamine_shared->getxmin_map(specno);
  float xhi = xamine_shared->getxmax_map(specno);
  float nch = xamine_shared->getxdim(specno) -2;  // Remove root chans.
  
  // There's really an extra 1/2 channel xhi we need o allow for.
  
  xhi +=  channelWidth(xlo, xhi, nch);

  float x = Transform(xlo, xhi, 0.0, (float)(nch+1), value);
  
  return x;

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
int Xamine_YMappedToChan(int specno, float value)
{


  float ylo = xamine_shared->getymin_map(specno);
  float yhi = xamine_shared->getymax_map(specno);
  float nch = xamine_shared->getydim(specno) -2; // Remove root chans.
  
  yhi +=  channelWidth(ylo, yhi, nch);

  float y = Transform(ylo, yhi, 0.0, (float)(nch+1), value);
  
  return y;
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
float Xamine_XChanToMapped(int specno, float chan)
{
  float xlo = xamine_shared->getxmin_map(specno);
  float xhi = xamine_shared->getxmax_map(specno);
  int   nch = xamine_shared->getxdim(specno) - 2; // Remove root chans

  // We go to the end of the last channel so xhi must be adjusted by
  // a channel width?
  
  xhi += channelWidth(xlo, xhi, nch);
  
  return Transform(0, (float)(nch-1),    
		   xlo, xhi, chan);                  
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
float Xamine_YChanToMapped(int specno, float chan)
{
  float ylo = xamine_shared->getymin_map(specno);
  float yhi = xamine_shared->getymax_map(specno);
  int   nch = xamine_shared->getydim(specno) -2;  // remove root chan.
  
  // We go to the end of the last y channel so we need to add
  // one more channel width to yhi:
  
  yhi += channelWidth(ylo,  yhi, nch);

  return Transform(0, (float)(nch), ylo, yhi, chan); // Remove root chans

}
