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
#include "CFitCreator.h"
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif
 
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

  virtual   CFit* operator() (STD(string) name, int id=0) ; 
  virtual   STD(string) DescribeFit();

};

#endif
