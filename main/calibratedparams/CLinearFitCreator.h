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
#include "CFitCreator.h"
#include <string>
 
class CLinearFitCreator  : public CFitCreator        
{
public:
    //  Constructors and other canonical operations.

    CLinearFitCreator ();		//!< Constructor.
    virtual  ~ CLinearFitCreator ( ); //!< Destructor.
    CLinearFitCreator (const CLinearFitCreator& rSource ); //!< Copy construction.
    CLinearFitCreator& operator= (const CLinearFitCreator& rhs); //!< Assignment.
    int operator== (const CLinearFitCreator& rhs) const; //!< == comparison.
    int operator!= (const CLinearFitCreator& rhs) const; //!< != comparison.


// Class operations:

public:

  virtual   CFit* operator() () ; 
  virtual   std::string DescribeFit();

};

#endif
