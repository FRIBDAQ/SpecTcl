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

#ifndef TCLRESULT_H  //Required for current class
#define TCLRESULT_H
                               //Required for base classes
#include "TCLInterpreterObject.h"
#include "TCLString.h"
#include <string>
                               
class CTCLResult  : public CTCLInterpreterObject        
{
  
public:
			//Default constructor

  CTCLResult () :  CTCLInterpreterObject() { } 
  ~CTCLResult ( ) { }       //Destructor

			//Constructor with arguments

  CTCLResult (CTCLInterpreter* pInterp )       :
    CTCLInterpreterObject(pInterp)
  { }        
	
			//Copy constructor

  CTCLResult (const CTCLResult& aCTCLResult )   : 
    CTCLInterpreterObject (aCTCLResult) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CTCLResult& operator= (const CTCLResult& aCTCLResult)
  { 
    if (this == &aCTCLResult) return *this;          
    CTCLInterpreterObject::operator= (aCTCLResult);
  
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLResult& aCTCLResult)
  { 
    return (
	    (CTCLInterpreterObject::operator== (aCTCLResult))
	   );
  }                             
                     
  // Noncannonical operators:
  //
public:
  CTCLResult& operator= (const char* pString);
  CTCLResult& operator= (const std::string& rString) {
    return operator=(rString.c_str());
  }
			 
  CTCLResult& operator+= (const char* pString);
  CTCLResult& operator+= (const std::string& rString) {
    return operator+=(rString.c_str());
  }
  CTCLResult& operator+(const char* pString) {
    return operator+=(pString);
  }
  CTCLResult& operator+(const std::string& rString) {
    return operator+=(rString);
  }
  
  void Clear ()  ;
  
  void AppendElement (const char* pString)  ;
  void AppendElement (const std::string& rString) {
    AppendElement(rString.c_str());
  }
  //
  // Type conversions:
  //
public:
  operator const char* ()  ;
  operator std::string ()  ;
  operator CTCLString ()  ;


};

#endif
