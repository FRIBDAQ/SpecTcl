/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/



/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
**   mapcoord.h:
**     This include file contains the definitions needed for clients of
**     mapped coordinates. These functions convert a value in a mapped 
**     coordinate system into a channel value, and vice-versa.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** History:
**   @(#)convert.h	8.1 6/23/95 
*/

#ifndef MAPCOORD_H
#define MAPCOORD_H
#include <utility>

// Width of a channel in real coords.

double
channelWidth(double lo, double hi, double chans);

// Generic transformation function
//  Transforms a point on a numberline from a source coordinate system
//  to a destination coordinate system.

float Transform(double fSourceLow, double fSourceHigh,
		double fDestLow,   double fDestHigh, 
		double point);

// Two transforms and their inverses need to be captured:

//     Mapped coordinates <--> Channel coordinates

int Xamine_XMappedToChan(int specno, double value);
int Xamine_YMappedToChan(int specno, double value);
float Xamine_XChanToMapped(int specno, double chan);
float Xamine_YChanToMapped(int specno, double chan);

//     Screen coordinates <---> Channel coordinates.
//     note channel to pixel transforms produce an inclusive range of pixels
//     that display the channel.

std::pair<int, int> Xamine_XChanToPixelRange(
	int specno, int row, int col, int pixlow, int pixhigh, int chan
);
std::pair<int, int> Xamine_YChanToPixelRange(
	int specno, int row, int col, int pixlow, int pixhigh, int chan
);
int Xamine_XPixelToChannel(int specno, int row, int col, int pixlow, int pixhigh, int x);
int Xamine_YPixelToChannel(int specno, int row, int col, int pixlow, int pixhigh, int y);

#endif

