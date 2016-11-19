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

class CFit;

// The class definition.

class CFitCreator      
{

public:
  /// Constructors and other canonical operations.

  CFitCreator ();		//!< Constructor.
  virtual  ~ CFitCreator ( ); //!< Destructor.
  CFitCreator (const CFitCreator& rSource ); //!< Copy construction.
  CFitCreator& operator= (const CFitCreator& rhs); //!< Assignment.
  int operator== (const CFitCreator& rhs) const; //!< == comparison.
  int operator!= (const CFitCreator& rhs) const; //!< != comparison.
  
  
  // Class operations:
  
public:
  
  virtual   CFit* operator() (std::string name, int id=0)   = 0 ; 
  virtual   std::string DescribeFit()  = 0;
};

#endif
