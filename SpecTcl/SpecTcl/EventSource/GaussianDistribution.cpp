//  CGaussianDistribution.cpp
// Specific distribution which produces
// parameters which have a gaussian distribution
// about some channel with some characteristic
// width.
//
//   The gaussian random variants are produced by first generating
//   a normally distributed random using Teichrow's method.  Once one has
//   a normally distributed random variable; X, a random variable Y from
//   a gaussian with mean m and std. dev s can is:
//
//             Y = m + sX           
//
//   Teichrow's method is a relatively quick, approximate method which is fine
//   for us since we're really producing integers.  In that method, we
//   produce 12 independent uniform randoms U[0] .. U[12].
//          R <-- (sum(U[i]) -6)/4
//          D <-- R*R
//          X <-- R(a1 + D(a2 + D(a3 + D(a4 + a5D))))
//   Where a1..a4 are the 'funny numbers':
//
//      a1 = 3.949846138
//      a2 = 0.252408784
//      a3 = 0.076542912
//      a4 = 0.008355968
//      a5 = 0.029899776
//
//  For more on this, see:
//     The Art of Computer Programming, Semi-Numerical Algorithms 
//     D. Knuth, pg. 112 - 113.
//     Addison Wesley
//
//    drand48(3) is used to get uniformly distributed randoms.
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file///////////////////////////////////////////

//
// Header Files:
//

#include "GaussianDistribution.h"                               
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>


static const char* Copyright = 
"CGaussianDistribution.cpp: Copyright 1999 NSCL, All rights reserved\n";


// Constants:

static const double a[5] = {	// The multipliers for Teichrow's method
  3.949846138,
  0.252408784,
  0.076542912,
  0.008355968,
  0.029899776
};

Bool_t CGaussianDistribution::m_fSeeded =kfFALSE;

// Functions for class CGaussianDistribution

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t operator() (  )
//  Operation Type:
//     
//
UInt_t 
CGaussianDistribution::operator()() 
{
// Returns a channel by randomizing
// with a distribution which matches the
// specified distribution parameters.

  // First compute R and D:
 
  DFloat_t R = 0.0;
  
  for(Int_t i = 0; i < 12; i++) {
    R += drand48();
  }
  R = (R-6.0)/4.0;
  DFloat_t D;
  D = R*R;
  //
  // Now do the sum which computes the normal variant:
  //
  DFloat_t X = a[4]*D;
  X = D*(X + a[3]);
  X = D*(X + a[2]);
  X = D*(X + a[1]);
  X = R*(X + a[0]);
  
  // Adjust the random into our density function:
  //
  DFloat_t Y = (m_fCentroid + m_fStdDev * X);

  // What we return is a clipped version of this:

  if( Y < 0.0 ) Y = 0.0;
  if( Y > (double)m_nScale) Y = (double)m_nScale;

  return ((UInt_t)Y);
}


////////////////////////////////////////////////////////////////////////
//
// Function:
//    void Seed()
// Operation Type:
//    Initialization.
void
CGaussianDistribution::Seed()
{
  // Re-seeds the randomizer used by drand48().  Automatically invoked
  // on first object construction.  May also be invoked by the user.
  // the time is used to generate a 48 bit seed.

  time_t  now;
  tm      nowstruct;
  time(&now);
  memcpy(&nowstruct, localtime(&now), sizeof(tm));

  // seed48 requires 3 unsigned shorts we'll get them from the 
  //  tm_sec, tm_min and tm_mday for the heck of it.
  //
  unsigned short seed[3];
  seed[0] = nowstruct.tm_sec;
  seed[1] = nowstruct.tm_min;
  seed[2] = nowstruct.tm_mday;

  seed48(seed);
  m_fSeeded = kfTRUE;
}
