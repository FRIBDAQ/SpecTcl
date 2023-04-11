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

/** @file:  transformations.cpp
 *  @brief: Implement generic coordinate transforms.
 */
#include "transformations.h"
#include "panemgr.h"
#include "axes.h"
#include "dispshare.h"
#include <dispwind.h>
#include <stdexcept>

extern spec_shared* xamine_shared;    // Shared memory with API>

/**
 * transform
 *   Perform a generic coordinate transform of a point x relative to 
 *  a window that goes from [x0, x1) to a window that goes
 *  [xp0, xp1)
 *
 *  @param x - the point to transform.
 *  @param x0,x1  - the source coordinate window.
 *  @param xp0,xp1 - the destination coordinate window.
 *  @return double - The transformed poin.
 *  @note Divisions by zero are protected by returning 0.0  not ideal but...
 */
double transform(double x, double x0, double x1, double xp0, double xp1) {
    double soffset = x - x0;     // Offset into source window.
    double sextent = x1 -x0;
    if (sextent == 0.0) {
        return 0.0;             // Dodge division by zero.
    } else {
        double dextent = xp1 - xp0;
        return xp0 + soffset*dextent/sextent;
    }
    throw std::logic_error("Control should not reach end of transform");
}

// We need som function that can return low high limits.  Since these
// will almost always go to a transformation they'll be doubles:
struct Extent {
    double low;
    double high;
};

static Rectangle get_drawing_rectangle(int row, int col) {
    auto atts = Xamine_GetDisplayAttributes(row, col);
    auto w    = Xamine_GetDrawingArea(row, col);
    
    auto drect = Xamine_GetSpectrumDrawingRegion(w, atts);
    return drect;
}

// Return the pixel extent on the x axis of a row/column:

static Extent get_xpixel_extent(int row, int col) {
    
    auto drect = get_drawing_rectangle(row, col);
    
    // we want the X extent:
    
    Extent result =  {
        static_cast<double>(drect.xbase),
        static_cast<double>(drect.xmax)
    };
    return result;
    
}
// return the pixel extens of the y axis of a row/column.
// Note these are 'backwards' because the
// the xwin coordinate system has the origin at the top left.
// In fact. high is always 0 (the top of the window) since there
// is no top margin.
//

static Extent get_ypixel_extent(int row, int col) {
    auto drect = get_drawing_rectangle(row, col);
    
    Extent result  = {
        static_cast<double>(drect.ybase),
        0.0
    };
    return result;
}


/**
 * Convert an xpixel value to an x_axis value.  We've (hopefully) removed
 * the axis flip capability so we ignore the flipped attribute.
 * The X axis is, therefore, always linear.  Even so, there are still several cases:
 *   - 1-d/2-d are different attribute blocks which have different interfaces.
 *   - Expanded, the low/high axis limits come from the attribute block.
 *   - Unexpanded, the low/high axis limits come from the shared memory (dispshare.h interface).
 * _BUT_
 *   if not in mapped mode, _and_ unexpanded, the axis limits are channel values
 *   that are [1, nchans)  This allows for the underflow channel and _includes_
 *   the last channel (nchan is the overflow channel).
 *   If expanded an in mapped  mode the channel expansions must be converted to
 *   mapped mode expansions.
 *
 *   @param pix - an x pixel value.
 *   @param row - Pane manager row of the pane to compute for.
 *   @parm col  - Pane manager column of the pane to compute for.
 */
double xpixel_to_axis(int pix, int row, int col) {
    auto pixels = get_xpixel_extent(row, col);   // Pixel range.
    auto attributes =  Xamine_GetDisplayAttributes(row, col);
    
    // Branch between 1d and 2d to figure out channel limits:
    
    int chlow, chhigh;
    // Set the unxpanded values as the default
    
    int nch = xamine_shared->getxdim(attributes->spectrum());
    chlow = 1;
    chhigh = nch;
    
    // Sadly isexpanded is not virtual so:
    
    if (attributes->is1d()) {
        win_1d* at1 = dynamic_cast<win_1d*>(attributes);
        // Get the channel limits:
        
        if (at1->isexpanded()) {
            chlow = at1->lowlimit();
            chhigh= at1->highlimit();
        }
    
    } else {
        // A few method changes but much the same logic for the 2d:
        
        win_2d* at2 = dynamic_cast<win_2d*>(attributes);
        
        if (at2->isexpanded()) {
            chlow = at2->xlowlim();
            chhigh = at2->xhilim();
        
        }
    }
        // Turn these into low/high depending on the mapping state:
    
    double low, high;
    if (attributes->ismapped()) {
        // low/high are transformed chlow, chhigh:
        
        double maplow = xamine_shared->getxmin_map(attributes->spectrum());
        double maphigh= xamine_shared->getxmax_map(attributes->spectrum());
        
        low = transform(chlow, chlow, chhigh, maplow, maphigh);
        high = transform(chhigh, chlow, chhigh, maplow, maphigh);
        
    } else {
        // low/high are chlow, chhigh
        
        low = chlow;
        high= chhigh;
    }
    //Now we can do the pixel to axis transform:
    
    return transform(static_cast<double>(pix), pixels.low, pixels.high, low, high);
        
    
}