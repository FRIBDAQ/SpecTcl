
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
#include "CFit.h"
 
class CLinearFit  : public CFit        
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
    CLinearFit (std::string name, int id=0);		 //!< Constructor.
    virtual  ~ CLinearFit ( );   //!< Destructor.
    CLinearFit (const CLinearFit& rSource ); //!< Copy construction.
    CLinearFit& operator= (const CLinearFit& rhs); //!< Assignment.
    int operator== (const CLinearFit& rhs) const; //!< == comparison.
    int operator!= (const CLinearFit& rhs) const; //!< != comparison.

  // class functions.

public:

  virtual   void Perform ()   ; 
  virtual   double operator() (double x)   ; 
  virtual   CFit::FitParameterList GetParameters ()   ; 
  virtual   std::string Type() const {
    return std::string("linear");
  }
  virtual  std::string makeTclFitScript();


  // Overrides for virtual base class functions:
  
  virtual CFit* clone();


};

#endif
