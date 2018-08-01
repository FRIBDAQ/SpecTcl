/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

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


// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 



/*!
  \class CLinearFit

  Performs linear fits.

*/



#ifndef CLINEARFIT_H  //Required for current class
#define CLINEARFIT_H

//
// Include files:
//

                               //Required for base classes
#include "./CFit.h"
 
class CCalibLinearFit  : public CCalibFit        
{
private:
  
  // Private Member data:
  double m_fSlope;      //!< Slope of best fit line.  
  double m_fOffset;     //!< Intercept of best fit line with Y axis.  
  double m_fChiSquare;  //!< Chi-square goodness of the fit.  
  

public:
    //  Constructors and other canonical operations.
    //  You may need to adjust the parameters
    //  and the visibility esp. if you cannot
    // implement assignment/copy construction.
    // safely.
    CCalibLinearFit ();		 //!< Constructor.
    virtual  ~ CCalibLinearFit ( );   //!< Destructor.
    CCalibLinearFit (const CCalibLinearFit& rSource ); //!< Copy construction.
    CCalibLinearFit& operator= (const CCalibLinearFit& rhs); //!< Assignment.
    int operator== (const CCalibLinearFit& rhs) const; //!< == comparison.
    int operator!= (const CCalibLinearFit& rhs) const; //!< != comparison.

  // class functions.

public:

  virtual   void Perform ()   ; 
  virtual   double operator() (double x)   ; 
  virtual   CCalibFit::FitParameterList GetParameters ()   ; 
  virtual   STD(string) Type() const {
    return STD(string)("linear");
  }

  // Overrides for virtual base class functions:
  
  virtual CCalibFit* clone();


};

#endif
