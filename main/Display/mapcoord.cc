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
#include "Transform.h"

extern volatile spec_shared *xamine_shared;



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
  float nch = xamine_shared->getxdim(specno);

	return WorldToChannel(xlo, xhi, 0, nch, value, false);
  float x = Transform(xlo, xhi, 0.0, (float)(nch-1), value);

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
  float nch = xamine_shared->getydim(specno);

	return WorldToChannel(ylo, yhi, 0, nch, value, false);

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
  int   nch = xamine_shared->getxdim(specno);

	return ChannelToWorld(0, nch, xlo, xhi, (int)chan, false);
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
  int   nch = xamine_shared->getydim(specno);

  return ChannelToWorld(0, nch, ylo, yhi, (int)chan, false);

}
