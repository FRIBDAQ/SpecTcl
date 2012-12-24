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


#ifndef __CLINEARFITCREATOR_H  //Required for current class
#define __CLINEARFITCREATOR_H

//
// Include files:
//

                               //Required for base classes
#ifndef __CFITCREATOR_H     //CFitCreator
#include "./CFitCreator.h"
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif
 
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
