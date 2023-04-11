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
#include <stdexcept>
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