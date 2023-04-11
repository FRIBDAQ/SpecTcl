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

/** @file:  transformations.h
 *  @brief: Generic transformations that will centralize all the coordinate transforms in Xamine
 */
#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

// Generic transformation:

double transform(double x, double x0, double x1, double xp0, double xp1);

// Transform Pixels -> Axis value.

double xpixel_to_xaxis(int pix, int row, int col);
double ypixel_to_yaxis(int pix, int row, int col);

// Transform Axis value -> Pixels

int xaxis_to_xpixel(double axis, int row, int col);
int yaxis_to_ypixel(double axis, int row, int col);

// Transform Pixels -> Channel #

int xpixel_to_xchan(int pix, int row, int col);
int ypixel_to_ychan(int pix, int row, int col);

// Transform Channel # -> Pixel

int xchan_to_xpixel(int chan, int row, int col);
int ychan_to_ypixel(int chan, int row, int col);

// Transform axis value -> Channel #

int xaxis_to_xchan(double axis, int row, int col);
int yaxis_to_ychan(double axis, int row, int col);


// Transform channel -> axis value

double xchan_to_xaxis(int chan, int row, int col);
double ychan_to_yaxis(int chan, int row, int col);

#endif