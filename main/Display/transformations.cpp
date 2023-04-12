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
#include <math.h>

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

// Clip to limits.  Note that we make no assumptions about which limit is which:

static double clip(double val, double l1, double l2) {
    // order the limits:
    
    if (l1 > l2) {
        auto temp = l2;
        l2 = l1;
        l1 = temp;
    }
    if (val < l1) return l1;
    if (val > l2) return l2;
    return val;
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
double xpixel_to_xaxis(int pix, int row, int col) {
    auto pixels = get_xpixel_extent(row, col);   // Pixel range.
    auto attributes =  Xamine_GetDisplayAttributes(row, col);
    
    // Branch between 1d and 2d to figure out channel limits:
    
    int chlow, chhigh;
    // Set the unxpanded values as the default
    
    int nch = xamine_shared->getxdim(attributes->spectrum());
    chlow = 0;
    chhigh = nch-2;
    
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
        
        low = transform(chlow, 0.0, nch-2, maplow, maphigh);
        high = transform(chhigh, 0.0, nch-2, maplow, maphigh);
        
    } else {
        // low/high are chlow, chhigh
        
        low = chlow;
        high= chhigh;
    }
    //Now we can do the pixel to axis transform:
    
    auto raw_result = transform(static_cast<double>(pix), pixels.low, pixels.high, low, high);
    return clip(raw_result, low, high);
    
}
/**
 *  ypixel_to_yaxis
 *    Convert a y pixel value to a y axis value.  This has even one more
 *    case than the x axis - if the spectrum is 1d its counts axis could be
 *    a log scale in which case the transformatin is not linear in the counts
 *    scale though it is linear in log scale.  To simplify this,
 *    we maintain the pixel value as a double so that we can take its
 *    log10
 *
 *    @param pix - the ypixel value (note that coordinates ascend top to bottom).
 *    @param row - Pane row for which we're computing this.
 *    @param col - pane column  for which we'e computing.
 *    
 */
double ypixel_to_yaxis(int pix, int row, int col) {
    auto pixels = get_ypixel_extent(row, col);   // Pixel range.
    auto attributes =  Xamine_GetDisplayAttributes(row, col);
    
    double pixel = static_cast<double>(pix);    // For log scaling.
    
    // Branch between 1d and 2d to figure out channel limits:
    
    double chlow, chhigh;        // Again for log scaling.
    
    
    
    if (attributes->is1d()) {
        // Note 1d spectra mapping is not a problem but log scale it.
        
        win_1d* at1 = dynamic_cast<win_1d*>(attributes);
        
        // I think even autoscale sets the fsvalue
        
        double top = at1->getfsval();
        double bottom = 0.0;                     // Default unless:
        if (at1->hasfloor()) bottom = at1->getfloor();
        if (at1->hasceiling()) top = at1->getceiling();
        
        // Now what we do depends on if y is linear or log:
        
        if (at1->islog()) {
            // pixel value of 0 is top:
            
            if (pixel == 0.0) return top;
            
            // if bottom is nonzero :
            
            if (bottom > 0.0)  bottom = log10(bottom);
            top =  log10(top);
            pixel = log10(pixel);
            
            double logvalue = transform(pixel, pixels.low, pixels.high, bottom, top);
            logvalue  = clip(logvalue, bottom, top);
            return exp10(logvalue);
            
            
        } else {
            // Simple linear:
            
            return clip(transform(
                pixel, pixels.low, pixels.high, bottom, top),
                bottom, top
            );
        }
        
    } else {
        // 2d this is a 'channel' axis:
        // Set the unxpanded values as the default
    
        int nch = xamine_shared->getydim(attributes->spectrum());
        chlow = 0;
        chhigh = static_cast<double>(nch-2);
        
        win_2d* at2 = dynamic_cast<win_2d*>(attributes);
        
        if (at2->isexpanded()) {
            chlow = at2->xlowlim();
            chhigh = at2->xhilim();
        
        }
        // Turn these into low/high depending on the mapping state:
        
        double low, high;
        if (attributes->ismapped()) {
            // low/high are transformed chlow, chhigh:
            
            double maplow = xamine_shared->getymin_map(attributes->spectrum());
            double maphigh= xamine_shared->getymax_map(attributes->spectrum());
            
            low = transform(chlow, chlow, chhigh, maplow, maphigh);
            high = transform(chhigh, chlow, chhigh, maplow, maphigh);
            
        } else {
            // low/high are chlow, chhigh
            
            low = chlow;
            high= chhigh;
        }
        //Now we can do the pixel to axis transform:
        
        return clip(
            transform(static_cast<double>(pix), pixels.low, pixels.high, low, high),
            low, high
        );
        
    }
}
/**
 *  Transform x axis value to pixels, this is the inverse transform of xpixel_to_xaxis.
 *
 *  @param axis - axis value.
 *  @param row  - Pane manager row of the spectrum we're operating on.
 *  @param col - pane manager columne of the spectrum we're operating on.
 *  @return int - nearest corresponsding pixel value.
 */
 
int xaxis_to_xpixel(double axis, int row, int col) {
    auto pixels = get_xpixel_extent(row, col);   // Pixel range.
    auto attributes =  Xamine_GetDisplayAttributes(row, col);
    
    // Branch between 1d and 2d to figure out channel limits:
    
    int chlow, chhigh;
    // Set the unxpanded values as the default
    
    int nch = xamine_shared->getxdim(attributes->spectrum());
    chlow = 0;
    chhigh = nch-2;
    
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
    //Now we can do the axis to pixel transformation:
    
    return static_cast<int>(nearbyint(
        clip(transform(axis, low, high, pixels.low, pixels.high), low, high)
    ));
}
/**
 * transform a yaxis value into a pixel value.  This is the inverse transform
 * of ypixel_to_yaxis
 *
 * @param axis - axis coordinate value.
 * @param row - Pane manager row holding the spectrum we're working in.
 * @param col - Pane manager column holding the spectrum we're working in.
 * @retun int - nearest y pixel correspnding to that  axis coordinate position.
 */
int yaxis_to_ypixel(double axis, int row, int col) {
auto pixels = get_ypixel_extent(row, col);   // Pixel range.
    auto attributes =  Xamine_GetDisplayAttributes(row, col);
    
    // Branch between 1d and 2d to figure out channel limits:
    
    double chlow, chhigh;        // Again for log scaling.
    
    
    
    if (attributes->is1d()) {
        // Note 1d spectra mapping is not a problem but log scale it.
        
        win_1d* at1 = dynamic_cast<win_1d*>(attributes);
        
        // I think even autoscale sets the fsvalue
        
        double top = at1->getfsval();
        double bottom = 0.0;                     // Default unless:
        if (at1->hasfloor()) bottom = at1->getfloor();
        if (at1->hasceiling()) top = at1->getceiling();
        
        // Now what we do depends on if y is linear or log:
        
        if (at1->islog()) {
            // axis value of 0.0 in log scale is on the x axis:
            
            if (axis == 0.0) return pixels.high;
            
            // if bottom is nonzero :
            
            if (bottom > 0.0)  bottom = log10(bottom);
            top =  log10(top);
            axis = log10(axis);
            
            double logvalue = clip(transform(axis,  bottom, top, pixels.low, pixels.high), pixels.low, pixels.high);
            return static_cast<int>(nearbyint(exp10(logvalue)));
            
            
        } else {
            // Simple linear:
            
            return static_cast<int>(nearbyint(
                clip(transform(axis, bottom, top, pixels.low, pixels.high), pixels.low, pixels.high)
            ));
        }
        
    } else {
        // 2d this is a 'channel' axis:
        // Set the unxpanded values as the default
    
        int nch = xamine_shared->getydim(attributes->spectrum());
        chlow = 0.0;
        chhigh = static_cast<double>(nch-2);
        
        win_2d* at2 = dynamic_cast<win_2d*>(attributes);
        
        if (at2->isexpanded()) {
            chlow = at2->xlowlim();
            chhigh = at2->xhilim();
        
        }
        // Turn these into low/high depending on the mapping state:
        
        double low, high;
        if (attributes->ismapped()) {
            // low/high are transformed chlow, chhigh:
            
            double maplow = xamine_shared->getymin_map(attributes->spectrum());
            double maphigh= xamine_shared->getymax_map(attributes->spectrum());
            
            low = transform(chlow, chlow, chhigh, maplow, maphigh);
            high = transform(chhigh, chlow, chhigh, maplow, maphigh);
            
        } else {
            // low/high are chlow, chhigh
            
            low = chlow;
            high= chhigh;
        }
        //Now we can do the transform
        
        return static_cast<int>(nearbyint(
            clip(
                 transform(axis, low, high, pixels.low, pixels.high),
                 pixels.low, pixels.high
            )
        ));
    }       
}
/**
 * Convert an X pixel into a channel number.  Channels all cover a range of
 *  [low, high) with the next channel picking up the high of the last channel.
 *  The right most channel's high is not part of the spectrum.
 *
 *  @param pix  - The x pixel coordinate.
 *  @param row  - Row of the pane we're working in.
 *  @param col  - Column of the pane we're working in.
 *  @return int - x channel number.
 */
int xpixel_to_xchan(int pix, int row, int col) {
    
    // The strategy is simple:  First convert the
    // pixel to an axis coordinate.  If the spectrum is not mapped, we're done,
    // if the specxtrum is displayed in mapped mode, we need to
    // use the mapping and current expansion (if any) to convert the
    // axis value ot a channel number.
    
    double axis = xpixel_to_xaxis(pix, row, col);
    auto attributes =  Xamine_GetDisplayAttributes(row, col);
    if (!attributes->ismapped()) return static_cast<int>(nearbyint(axis));  // Axis coords are channels.
    
    int specid = attributes->spectrum();
    
    // We can simplify by using the full mapped extent and channel extent:
    
    
    int chanlow = 0;                      // omit underflow
    int chanhi = xamine_shared->getxdim(specid) -2;  // Just to the overflow
    double axlow = xamine_shared->getxmin_map(specid);
    double axhigh = xamine_shared->getxmax_map(specid);
    
    return static_cast<int>(nearbyint(
        clip(transform(axis, axlow, axhigh, chanlow, chanhi), chanlow, chanhi)
    ));
}
/**
 * Convert a y pixel to a channel number or counts (if 1d).
 * -   For 1d this is just ypixel_to_yaxis
 * -   For unmapped 2d thisi s just ypixel_to_yaxis
 * -   For mapped 2d this is ypxiel_to_yaxis transformed from
 * axis to channel coords using the mapping information.
 *
 *  @param pix - pixel position.
 *  @param row - Pane row number
 *  @param col - pane colunmn number
 *  @return int - y channel number.
 */
int ypixel_to_ychan(int pix, int row, int col) {
    double axis = ypixel_to_yaxis(pix, row, col);
    auto attributes = Xamine_GetDisplayAttributes(row, col);
    if (attributes->is1d() || (!attributes->ismapped()))
        if (axis < 0.0) axis = 0.0;      // CLip to origin.
        return static_cast<int>(nearbyint(axis));
    
    // Need to use mapping info to transform to channels
    
    int specid = attributes->spectrum();
    int chanlow = 0;                      // omit underflow
    int chanhi = xamine_shared->getydim(specid) -2 ;  // Just to the overflow
    double axlow = xamine_shared->getymin_map(specid);
    double axhigh = xamine_shared->getymax_map(specid);
    
    return static_cast<int>(nearbyint(
        clip(transform(axis, axlow, axhigh, chanlow, chanhi), chanlow, chanhi)
    ));
    
}
/**
 * Convert x channel number to pixel.  This is a matter of converting
 * the channel # to an axis coordinate and then converting that to
 * pixel space.
 * The conversion to axis coordinates is a unit transform if the
 * spectrum is not in mapped mode else we can use the mapping information
 * to do the conversion.
 *
 * @param chan - channel number to transofmr.
 * @param row, col - coordinates of the pane in which we're operating.
 * @return int - X Pixel coordinate of corresponding to the channel in the window.
 */
int xchan_to_xpixel(int chan, int row, int col) {
    // FIrst convert the channel to axis coordinates, then the resulting
    // xaxis position to pixels.
    
    auto xaxis = xchan_to_xaxis(chan, row, col);
    return xaxis_to_xpixel(xaxis, row, col);
}
/**
 * similarly for y channel to pixel
 * 
* @param chan - channel number to transofmr.
* @param row, col - coordinates of the pane in which we're operating.
* @return int - Y Pixel coordinate of corresponding to the channel in the window.
*/
int ychan_to_ypixel(int chan, int row, int col) {
    auto yaxis = ychan_to_yaxis(chan, row, col);
    return yaxis_to_ypixel(yaxis, row, col);
}
/**
 * Convert an X axis coordinate to channel coordinates.
 * - if the spectrum is not displayed mapped this is a unit transform.
 * - If mapped, use the full range of channels and the xmapping information
 * to perform the transformation.
 *
 * @param axis - Axis coordinate value to transform.
 * @param row, col - coordinates of the pane we're operating in.
 * @return  int  - correspondig x channel number.
 */
int xaxis_to_xchan(double axis, int row, int col) {
    auto attributes = Xamine_GetDisplayAttributes(row, col);
    int spid = attributes->spectrum();
    if (!attributes->ismapped()) return static_cast<int>(nearbyint(axis));
    
    // mapped so we need the extent of the channels anb axis:
    
    int chanlow = 0;
    int chanhi  = xamine_shared->getxdim(spid) - 2;
    double xlow = xamine_shared->getxmin_map(spid);
    double xhigh = xamine_shared->getxmax_map(spid);
    
    return static_cast<int>(nearbyint(
       clip(transform(axis, xlow, xhigh, chanlow, chanhi), chanlow, chanhi) 
    ));
    
}
/**
 * convert a Y axis coordinate to channel coordinates
 * 
 * @param axis - Axis coordinate value to transform.
 * @param row, col - coordinates of the pane we're operating in.
 * @return  int  - correspondig y channel number.
 */
int yaxis_to_ychan(double axis, int row, int col) {
    auto attributes = Xamine_GetDisplayAttributes(row, col);
        throw std::logic_error("Attempting yaxis->ychan transform on 1d spectrum");
    if (attributes->is1d()) {
        int spid = attributes->spectrum();
        if (!attributes->ismapped()) return static_cast<int>(nearbyint(axis));
        
        // mapped so we need the extent of the channels anb axis:
        
        int chanlow = 0;
        int chanhi  = xamine_shared->getydim(spid) - 2;
        double ylow = xamine_shared->getymin_map(spid);
        double yhigh = xamine_shared->getymax_map(spid);
        
        return static_cast<int>(nearbyint(
           clip(transform(axis, ylow, yhigh, chanlow, chanhi), chanlow, chanhi) 
        ));
    }
}

/**
 * convert the an X channel number to an x axis value.
 * If not mapped, the channel number is the axis coordinate.
 * IF mapped run the transform using the mapping data.
 *
 * @param chan - channel to transform.
 * @param row, col  - Coordinates of the pane in which the transform is being done.
 * @return double  - The axis coordinates.
 */
double xchan_to_xaxis(int chan, int row, int col) {
    auto attributes = Xamine_GetDisplayAttributes(row, col);
    if (!attributes->ismapped()) return static_cast<double>(chan);
    
    int spid = attributes->spectrum();
    double clow = 0.0;
    double chigh = xamine_shared->getxdim(spid) -2;
    double xlow = xamine_shared->getxmin_map(spid);
    double xhigh= xamine_shared->getxmax_map(spid);
    
    return clip(transform(chan, clow, chigh, xlow, xhigh), xlow, xhigh);
}
/**
 * Convert a y channel to a y axis value.  Only legal for 2d spectra.
 * 
 *  @param chan - channel number.
 *  @param row, col - coordinates of the pane.
 *  @return double - corresponding axis coordinate.
 */
double ychan_to_yaxis(int chan, int row, int col) {
    auto attributes = Xamine_GetDisplayAttributes(row, col);
    if (attributes->is1d()) {
        throw std::logic_error("Attempting ychan -> yaxs transform on a 1d");
    }
    if (!attributes->ismapped()) return static_cast<double>(chan);
    int spid = attributes->spectrum();
    double clow = 0.0;
    double chigh = xamine_shared->getydim(spid) -2;
    double ylow = xamine_shared->getymin_map(spid);
    double yhigh= xamine_shared->getymax_map(spid);
    
    return clip(transform(chan, clow, chigh, ylow, yhigh), ylow, yhigh);
}