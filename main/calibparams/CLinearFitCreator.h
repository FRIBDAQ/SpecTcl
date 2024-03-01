// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

/*!
  \class CLinearFitCreator

  Creates a linear fit.

*/


#ifndef CLINEARFITCREATOR_H  //Required for current class
#define CLINEARFITCREATOR_H

//
// Include files:
//

                               //Required for base classes
#include "./CFitCreator.h"
#include <string>
 
class CCalibLinearFitCreator  : public CCalibFitCreator        
{
public:
    //  Constructors and other canonical operations.

    CCalibLinearFitCreator ();		//!< Constructor.
    virtual  ~ CCalibLinearFitCreator ( ); //!< Destructor.
    CCalibLinearFitCreator (const CCalibLinearFitCreator& rSource ); //!< Copy construction.
    CCalibLinearFitCreator& operator= (const CCalibLinearFitCreator& rhs); //!< Assignment.
    int operator== (const CCalibLinearFitCreator& rhs) const; //!< == comparison.
    int operator!= (const CCalibLinearFitCreator& rhs) const; //!< != comparison.


// Class operations:

public:

  virtual   CCalibFit* operator() () ; 
  virtual   STD(string) DescribeFit();

};

#endif
