//  CTCLString.h:
//
//    This file defines the CTCLString class.
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

#ifndef __TCLSTRING_H  //Required for current class
#define __TCLSTRING_H

#ifndef __TCL_TCL_H
#include <tcl.h>
#define __TCL_TCL_H
#endif             

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __CRT_STRING_H
#include <string.h>
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
               
class CTCLString      
{
  Tcl_DString m_String;  // Dynamic string maintained by TCL allocation/free.
  
public:
			//Default constructor

  CTCLString ()    { 
    Tcl_DStringInit(&m_String);
  } 
  ~ CTCLString () { 
  Tcl_DStringFree(&m_String);
  }       //Destructor

			//Constructors with arguments

  CTCLString (const char* pString  )       
   { 
     Tcl_DStringInit(&m_String);
     Tcl_DStringAppend(&m_String, pString, -1);
   }
  CTCLString(const std::string& rString) {
    Tcl_DStringInit(&m_String);
    Tcl_DStringAppend(&m_String, rString.c_str(), -1);
  }
  CTCLString(const Tcl_DString& rString) {
    Tcl_DStringInit(&m_String);
    Tcl_DStringAppend(&m_String, Tcl_DStringValue(&rString), -1);
  }
	
			//Copy constructor

  CTCLString (const CTCLString& aCTCLString ) 
  { 
    Tcl_DStringInit(&m_String);	// So free operates correctly in doassign.
    DoAssign(aCTCLString);
  }                                     

			//Operator= Assignment Operator

  CTCLString& operator= (const CTCLString& aCTCLString)
  { 
    if (this == &aCTCLString) return *this;          
  
    DoAssign(aCTCLString);
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLString& aCTCLString)
  { 
    return(strcmp(Tcl_DStringValue(&m_String),
		  Tcl_DStringValue(&aCTCLString.m_String)) == 0);
  }
  int operator!= (const CTCLString& aCTCLString) {
    return !(operator==(aCTCLString));
  }
  int operator> (const CTCLString& aCTCLString) {
    return(strcmp(Tcl_DStringValue(&m_String),
		  Tcl_DStringValue(&aCTCLString.m_String)) > 0);

  }
  int operator< (const CTCLString& aCTCLString) {
    return(strcmp(Tcl_DStringValue(&m_String),
		  Tcl_DStringValue(&aCTCLString.m_String)) < 0);

  }
  int operator>=(const CTCLString& aCTCLString) {
    return (operator>(aCTCLString) || operator==(aCTCLString));
  }
  int operator<=(const CTCLString& aCTCLString) {
    return (operator<(aCTCLString) || operator==(aCTCLString));
  }
  // Selectors:
public:
                       //Get accessor function for attribute
  Tcl_DString& getString()
  {
    return m_String;
  }
  // Mutators:
  //
                       //Set accessor function for attribute
  void setString (const Tcl_DString& am_String)
  { 
    operator=(am_String);
  }
  // Operations:
  //
public:
  // Append subchunk of strings:
  //
  CTCLString& Append (const std::string& rString, Int_t nLength=-1) {
    return Append(rString.c_str(), nLength);
  }
  CTCLString& Append (const CTCLString&  rString, Int_t nLength=-1) {
    return Append(Tcl_DStringValue(&rString.m_String), nLength);
  }
  CTCLString& Append (Tcl_DString&       pString, Int_t nLength=-1) {
    return Append(Tcl_DStringValue(&pString), nLength);
  }
  CTCLString& Append (const char*        pString, Int_t nLength=-1);
  //
  // Append a list element to a string:
  //
  CTCLString& AppendElement (const Tcl_DString*      pRhs) {
    return AppendElement(Tcl_DStringValue(pRhs));
  }
  CTCLString& AppendElement (const CTCLString&       rRhs) {
    return AppendElement(Tcl_DStringValue(&rRhs.m_String));
  }
  CTCLString& AppendElement (const std::string&      rRhs) {
    return AppendElement(rRhs.c_str());
  }
  CTCLString& AppendElement (const char*             pRhs);
  //
  // List/sublist bracketing calls:
  //
  CTCLString& StartSublist ()  ;
  CTCLString& EndSublist ()  ;
  UInt_t  Length () const ;
  CTCLString& Truncate (UInt_t nNewLength)  ;
  Bool_t isCommand () const  ;
  //
  //   Match a substring in the string - using glob rules
  //
  Bool_t Match (const char*       pPattern) const;
  Bool_t Match (std::string&      rPattern) const {
    return Match(rPattern.c_str());
  }
  Bool_t Match (const CTCLString& rPattern) const {
    return Match(Tcl_DStringValue(&rPattern.m_String));
  }

  operator const char* () const {
    return Tcl_DStringValue(&m_String);
  }
  operator std::string () const {
    return std::string(Tcl_DStringValue(&m_String));
  }
  operator Tcl_DString* () {
    return &m_String;
  }
  CTCLString& operator+= (const CTCLString& rRhs) {
    return Append(rRhs);
  }
  CTCLString operator+ (const CTCLString& rRhs) ;
  
  // Protected utility functions:
  
protected:
  void DoAssign(const CTCLString& rRhs);

};

#endif
