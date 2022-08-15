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

/** @file:  Transform.cc
 *  @brief: Coordinate transformations.
 */

/**
 * Note that coordinate transforms involving unexpanded spectra
 * are a bit wonky in 3.x because the Stolzian coordinate specifications
 * have the high world coordinate limit at the left end of the last channel.
 * After a bit of rumination this is a bit like having a 'normal' specification
 * with one less actual channel.
 */

#include "Transform.h"

/**
 * Transform
 *    This is just the classic linear transform of a point in a coordinate system
 *    running from [fromlow, from hi) to a coordinate system that runs from
 *    [tolow, tohi)
 *
 *  @param fromLow, fromHigh -- from coordinate window.
 *  @param toLow, toHigh     -- corresponding coordinate window in transformed space.
 *  @param coord  -  Coordinate to transform.
 *  @return double - coord transformed from the from coordinates to the to coordinates.
 *  @note the step by step-edness is done so that I have intermediate results
 *        I can look at in gdb since I don't trust myself :-)
 */
double
Transform(
    double fromLow, double fromHigh, double toLow, double toHigh, double coord
)
{
    double zeroBased = coord - fromLow;
    double scaled    = (coord/(fromHigh - fromLow)) * (toHigh - toLow);
    double result    = scaled + toLow;
    return result;
}
////////////////////////  Channel <---> World transforms.
/**
 * WorldToChannel
 *    Transform a world coordinate to a channel coordinate.
 *    This is wonky because world coordinate limits, in this SpecTcl are
 *    (I think)  in unexpandeed mode (only?) low limit at the left of the
 *    left most channel (so far so good) and righ most at the left of the high channel
 *    (wonky).  This means in expanded mode the transform is as if we had one
 *    less channel than actual.
 * @param worldLow, worldHigh - World coordinate slice from which to transform.
 * @param chanLow, chanHigh   - Corresponding channel windwo.
 * @param wc                  - World coordinate to transform.
 * @param expanded            - true if this  is an expanded view.
 */
unsigned
WorldToChannel(
    double worldLow, double worldHigh, unsigned chanLow, unsigned  chanHigh,
    double wc,
    bool expanded
)
{
    if (!expanded) chanHigh--;        // For wonkiness.
    if (chanHigh == chanLow) return chanLow;
    
    return Transform(worldLow, worldHigh, chanLow, chanHigh, wc);
}
/**
 * ChannelToWorld
 *    Inverse transform of WorldToChannel.  Transforms from channel space
 *    to word coorincate space.
 */
double
ChannelToWorld(
    unsigned chanLow, unsigned chanHigh, double worldLow, double worldHigh,
    unsigned chan,
    bool expanded
)
{
    if (!expanded) chanHigh--;      // for end limit wonkiness.
    
    
    if (chanHigh == chanLow) return worldLow;
    
    return Transform(chanLow, chanHigh, worldLow, worldHigh, chan);
}

///////////////////  Pixel <----->   Channel transforms.

/**
 * PixelToChannel
 *    Transformation from display pixels to channels.
 *    This is a truncated result so that we can't spill over past
 *    the end of the channel range as we would if we rounded and the
 *    pixel was past the midpoint of the last channel.
 */
unsigned
PixelToChannel(
    unsigned pixLow, unsigned pixHigh, unsigned chanLow, unsigned chanHigh,
    unsigned pix
)
{
    return Transform(pixLow, pixHigh, chanLow, chanHigh, pix);
}
/**
 * ChannelToPixel
 *    Inverse of PixelToChannel
 */
unsigned
ChannelToPixel(
    unsigned chanLow, unsigned chanHigh, unsigned pixLow, unsigned pixHigh,
    unsigned chan
)
{
    return Transform(chanLow, chanHigh, pixLow, pixHigh, chan);
}

//////////////////////////// Pixel <-->  World coordinates (mapped mode).

/**
 * PixelToWorld
 *    Transform pixel coordinates to world coordinates.
 *    This is a straightforward call to Transform.
 * @param pixLow, pixHigh -- pixel window from which the pixel is being transformed.
 * @param wordLow, worldHigh - world coordinates into which the pixel is being transformed.
 * @return double -the world coordinate cooresponding to the pixel.
 */
double PixelToWorld(
    unsigned pixLow, unsigned pixHigh, double worldLow, double worldHigh,
    unsigned pix
)
{
    return Transform(pixLow, pixHigh, worldLow, worldHigh, pix);
}
/**
 * WorldToPixel
 *   Inverse transform of WorldToPixel.
 * @return unsigned - the pixel corresponding to the world coordinate.
 *       note that this is truncated not rounded to prevent it from going
 *       off screen.
 *
 */
unsigned
WorldToPixel(
    double worldLow, double worldHigh, unsigned pixLow, unsigned pixHigh,
    double world
)
{
    return Transform(worldLow, worldHigh, pixLow, pixHigh, world);
}
