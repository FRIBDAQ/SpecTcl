// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

/*!
  \class CFitCreator


  ABC for fit creationals. 

*/


#ifndef __CFITCREATOR_H  //Required for current class
#define __CFITCREATOR_H

//
// Include files:
//

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


// Forward class definitions.

class CCalibFit;

// The class definition.

class CCalibFitCreator      
{

public:
  /// Constructors and other canonical operations.

  CCalibFitCreator ();		//!< Constructor.
  virtual  ~ CCalibFitCreator ( ); //!< Destructor.
  CCalibFitCreator (const CCalibFitCreator& rSource ); //!< Copy construction.
  CCalibFitCreator& operator= (const CCalibFitCreator& rhs); //!< Assignment.
  int operator== (const CCalibFitCreator& rhs) const; //!< == comparison.
  int operator!= (const CCalibFitCreator& rhs) const; //!< != comparison.
  
  
  // Class operations:
  
public:
  
  virtual   CCalibFit* operator() ()   = 0 ; 
  virtual   STD(string) DescribeFit()  = 0;
};

#endif
