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


#endif