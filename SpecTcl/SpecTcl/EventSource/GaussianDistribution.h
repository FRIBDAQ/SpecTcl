

//  CGaussianDistribution.h:
//
//    This file defines the CGaussianDistribution class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __CGAUSSIANDISTRIBUTION_H  //Required for current class
#define __CGAUSSIANDISTRIBUTION_H
                               //Required for base classes
#ifndef __CDISTRIBUTION_H
#include "Distribution.h"
#endif                               

#ifndef __HISTOTTYPES_H
#include <histotypes.h>
#endif
                               
class CGaussianDistribution  : public CDistribution        
{
  Float_t m_fCentroid;		//  Channel where centroid lives.
  Float_t m_fStdDev;		//  Standard deviation of distribution.
  Float_t m_nScale;		// Cut off for distribution.
				// If above, returns max chan.
  static Bool_t m_fSeeded;	// True if drand48 seeded.
  
public:

			//Constructor with arguments
				// There is no default constructor
				// However the parameters have defaults
				// which make it look like a gaussian
				// at half scale of a 4k spectrum.

  CGaussianDistribution (  Float_t am_fCentroid = 2048.0,  
			   Float_t am_fStdDev = 256.0,  
			   Float_t am_nScale = 4095. ) :
    m_fCentroid (am_fCentroid),  
    m_fStdDev (am_fStdDev),  
    m_nScale (am_nScale)  {if(!m_fSeeded) Seed(); }        

  ~CGaussianDistribution ( ) { }       //Destructor
	
			//Copy constructor

  CGaussianDistribution (const CGaussianDistribution& aCGaussianDistribution )
   : CDistribution (aCGaussianDistribution) 
  {
    m_fCentroid = aCGaussianDistribution.m_fCentroid;
    m_fStdDev = aCGaussianDistribution.m_fStdDev;
    m_nScale = aCGaussianDistribution.m_nScale;
    
  }                                     

			//Operator= Assignment Operator

  CGaussianDistribution &
           operator= (const CGaussianDistribution& aCGaussianDistribution)
  { 
    if (this == &aCGaussianDistribution) return *this;          
    CDistribution::operator= (aCGaussianDistribution);
    m_fCentroid = aCGaussianDistribution.m_fCentroid;
    m_fStdDev = aCGaussianDistribution.m_fStdDev;
    m_nScale = aCGaussianDistribution.m_nScale;
    
    return *this;
  }                                     

		        //Operator== Equality Operator

  int operator== (const CGaussianDistribution& aCGaussianDistribution)
  { 
    return (
	    (CDistribution::operator== (aCGaussianDistribution)) &&
	    (m_fCentroid == aCGaussianDistribution.m_fCentroid) &&
	    (m_fStdDev == aCGaussianDistribution.m_fStdDev) &&
	    (m_nScale == aCGaussianDistribution.m_nScale) 
	    );
  }           
  // Selectors (Read).

public:
                       //Get accessor function for attribute
  Float_t getCentroid() const
  {
    return m_fCentroid;
  }

                       //Get accessor function for attribute
  Float_t getStdDev() const
  {
    return m_fStdDev;
  }

                       //Get accessor function for attribute
  Float_t getScale() const
  {
    return m_nScale;
  }
  // Selectors (Write)... in this implementation those can be
  // public to allow the distribution to be dynamically adjusted
  // since the distribution parameters have orthogonal effects.
public:
                       //Set accessor function for attribute
  void setCentroid (Float_t am_fCentroid)
  {
    m_fCentroid = am_fCentroid;
  }

                       //Set accessor function for attribute
  void setStdDev (Float_t am_fStdDev)
  { 
    m_fStdDev = am_fStdDev;
  }

                       //Set accessor function for attribute
  void setScale (Float_t am_nScale)
  { 
    m_nScale = am_nScale;
  }
  // Operations.. The main (only) one produces a random variable drawn
  // from the underlying distribution, as an integer channel.

public:
  virtual   UInt_t operator() ()  ;
  static void      Seed();	// Reseed random number generator.
 
};

#endif
