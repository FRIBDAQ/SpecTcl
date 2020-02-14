/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

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

/** @file:  functions.h
 *  @brief: Provide code to evaluate various functions for the DDAS Fit.
 */
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <vector>
#include <stdint.h>

namespace DDAS {
double logistic(double A, double k, double x1, double x);
double decay(double A, double k, double x1, double x);
double switchOn(double x1, double x);

double singlePulse(
    double A1, double k1, double k2, double x1, double C, double x
);
double doublePulse(
    double A1, double k1, double k2, double x1,
    double A2, double k3, double k4, double x2,
    double C, double x
);

double pulseAmplitude(double A, double k1, double k2, double x0);

double chiSquare1(
    double A1, double k1, double k2, double x1, double C,
    const std::vector<uint16_t>& trace, int low = 0 , int high = -1
);

double chiSquare1(
    double A1, double k1, double k2, double x1, double C,
    const std::vector<std::pair<uint16_t, uint16_t> >& points
);

double chiSquare2(
    double A1, double k1, double k2, double x1,
    double A2, double k3, double k4, double x2,
    double C,    
    const std::vector<uint16_t>& trace, int low = 0, int high = -1
);
double chiSquare2(
    double A1, double k1, double k2, double x1,
    double A2, double k3, double k4, double x2,
    double C,
    const std::vector<std::pair<uint16_t, uint16_t> >& points
);
void writeTrace(
   const char* filename, const char* title,
   const std::vector<uint16_t>& trace
);
void writeTrace2(
   const char* filename, const char* title,
   const std::vector<uint16_t>& trace1, const std::vector<uint16_t>& trace2
);


}
#endif