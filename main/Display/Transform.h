/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  Transform.h
 *  @brief: Define coordinate transformation functions.
 */

#ifndef TRANSFORM_H
#define TRANSFORM_H

// This is a generic transformation:

double Transform(
    double fromLow, double fromHigh, double toLow, double toHigh, double coord
);

// Special transformation for channels because the world coordinate for
// high is actually on the left side of the last channel... if not expanded.
// Note that some of these functions are just convenience functions;  all
// however map to Transform.
//
unsigned WorldToChannel(
    double worldLow, double worldHigh, unsigned chanLow, unsigned  chanHigh,
    double wc,
    bool expanded
);
double   ChannelToWorld(
    unsigned chanLow, unsigned chanHigh, double worldLow, double worldHigh,
    unsigned chan,
    bool expanded
);

unsigned PixelToChannel(
    unsigned pixLow, unsigned pixHigh, unsigned chanLow, unsigned chanHigh,
    unsigned pix
);
unsigned ChannelToPixel(
    unsigned chanLow, unsigned chanHigh, unsigned pixLow, unsigned pixHigh,
    unsigned chan
);

double  PixelToWorld(
    unsigned pixLow, unsigned pixHigh, double worldLow, double worldHigh,
    unsigned pix
);
unsigned WorldToPixel(
    double worldLow, double worldHigh, unsigned pixLow, unsigned pixHigh,
    double world
);


#endif