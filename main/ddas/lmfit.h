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

/** @file:  lmfit.h
 *  @brief: Define the fitting functions and data structures for L-M fits.
 */
#ifndef LMFIT_H
#define LMFIT_H
#include <vector>
#include <stdint.h>
namespace DDAS {
// The following structs are used by the fitting
// functions.

// Describes a single pulse without an offset.

struct PulseDescription {  
   double position;         // Where the pusle is.
   double amplitude;        // Pulse amplitude
   double steepness;        // Logistic steepness factor.
   double decayTime;        // Decay time constant.    
};

// Full fitting information for the single pulse:
    
struct fit1Info {           // Info from single pulse fit:
   unsigned iterations;     // Iterations for fit to converge
   unsigned fitStatus;      // fit status from GSL.
   double chiSquare;
   PulseDescription pulse;
   double  offset;          // Constant offset.

};
    
// Full fitting information for the double pulse:

struct fit2Info {                // info from double pulse fit:
   unsigned iterations;          // Iterations needed to converge.
   unsigned fitStatus;           // Fit status from GSL
   double chiSquare; 
   PulseDescription pulses[2];  // The two pulses
   double offset;               // Ofset on which they sit.
};

// For good measure here's what we append to a DDAS Hit that's
// had its trace fitted.

struct HitExtension {     // Data added to hits with traces:
    fit1Info onePulseFit;
    fit2Info twoPulseFit;
};
// This struct is passed around the fitting subsystem to jacobian
// and function evaluators.
//
struct GslFitParameters {
   const std::vector<std::pair<uint16_t, uint16_t> >* s_pPoints;
};
  

void lmfit1(
   fit1Info* pResult, std::vector<uint16_t>& trace,
   const std::pair<unsigned, unsigned>& limits, uint16_t saturation = 0xffff
);
void lmfit2(
   fit2Info* pResult, std::vector<uint16_t>& trace,
   const std::pair<unsigned, unsigned>& limits,
   fit1Info* pSinglePulseFit = nullptr, uint16_t saturation = 0xffff
);

void lmfit2fixedT(
   fit2Info* pResult, std::vector<uint16_t>& trace,
   const std::pair<unsigned, unsigned>& limits,
   fit1Info* pSinglePulseFit = nullptr, uint16_t saturation = 0xffff
);

}

#endif
