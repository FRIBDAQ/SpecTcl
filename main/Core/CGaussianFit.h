/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef CGAUSSIANFIT_H
#define CGAUSSIANFIT_H

#include "CFit.h"


/*!
  This is a fit that is capable of performing a gaussian fit with constant background.
  The equation fit is  f(x) = b + p * e^[-0.5*((x - c)/w)^2].
  where: 
  - b is some constant background.
  - p is proportional to the area of the peak after background subtraction
  - c is the centroid of the peak.
  - w is the standard deviation (sigma).
 
  To be precise about the area of the background subtracted peak,
  A = p/(w*sqrt(2*pi)).

  The implementation of this class makes use of fitgsl.{c,h} included
  in SpecTcl with permission from 
  Tony Denault, and Leilehua Maly of the NASA IRTF Telescope.

*/
class CGaussianFit : public CFit
{
private:
  // member data is defined if the fit was performed.

  float m_baseline;		// b
  float m_peakHeight;		// p
  float m_centroid;		// c
  float m_sigma;		// w.
  float m_chisquare;		// chi square goodnes..

  // Constructors and canonicals
public:
  CGaussianFit(std::string name, int id = 0);
  virtual ~CGaussianFit();
  CGaussianFit(const CGaussianFit& rhs);
  CGaussianFit& operator=(const CGaussianFit& rhs);
  int operator==(const CGaussianFit& rhs) const;
  int operator!=(const CGaussianFit& rhs) const;


  // Virtual function overrides:
public:
  virtual void Perform();
  virtual double operator()(double x);
  virtual CFit::FitParameterList GetParameters();
  virtual std::string Type() const;
  virtual std::string makeTclFitScript();
  virtual CFit* clone();

  // Private utilities:

private:
  void CopyIn(const CGaussianFit& rhs);
  float ChiSquare();
  
};

#endif
