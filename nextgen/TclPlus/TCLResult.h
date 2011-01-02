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

//  CTCLResult.h:
//
//    This file defines the CTCLResult class.
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

#ifndef __TCLRESULT_H  //Required for current class
#define __TCLRESULT_H
                               //Required for base classes
#ifndef __TCLOBJECT_H
#include "TCLObject.h"
#endif

#ifndef __TCLSTRING_H
#include "TCLString.h"
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


/*!
   Encapsulates an interpreter result.  We derive this from a CTCLObject in order
   to have the full expressive power of that class when building up the result 
   string.  The idea is that at construction the interpreter result will be reset
   or optionally our object loaded with its current value.   Command processors
   will produce their result string here and then either at destruction time or
   explicitly, we will commit our contents to the result.

*/
class CTCLResult  : public CTCLObject
{
  
public:
  // Constructors and other canonicals

  CTCLResult (CTCLInterpreter* pInterp, bool reset=true );
  virtual ~CTCLResult ( );
  CTCLResult (const CTCLResult& aCTCLResult );
  CTCLResult& operator= (const CTCLResult& aCTCLResult);
  CTCLResult& operator= (const char* rhs);
  CTCLResult& operator=(std::string    rhs) {
    return operator=(rhs.c_str());
  }

			//Operator== Equality Operator

  int operator== (const CTCLResult& aCTCLResult) ;
  int operator!= (const CTCLResult& rhs);

  

  


public:
  // These are semantically different than in the base class.
  // This is >BAD< but we're stuck with it at this time.

  CTCLResult& operator+= (const char* pString);
  CTCLResult& operator+= (const std::string& rString) {
    return operator+=(rString.c_str());
  }

  
  // Named object operations:
  
public:
  void Clear ()  ;  
  void AppendElement (const char* pString)  ; // Base class +=
  void AppendElement (const std::string& rString); // Base class +=
  void commit() const;		// Commit this -> Result.
  std::string getString();      // Commit and return the result.


};


#endif
