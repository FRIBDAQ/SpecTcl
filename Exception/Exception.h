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


#ifndef __EXCEPTION_H  //Required for current class
#define __EXCEPTION_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

                               
class CException      
{
	// Attributes:
private:
  char m_szAction[kACTIONSIZE] ;   // Saved action in progress when
					               // exception was thrown.
  
public:
			//Default constructor

  virtual ~ CException ( ) { }       //Destructor

			//Constructors with arguments

  CException (const char* pszAction  );
  CException (const std::string& rsAction);
  
			//Copy constructor

  CException (const CException& aCException );

			//Operator= Assignment Operator

  CException& operator= (const CException& aCException);
  
			//Operator== Equality Operator
  int operator== (const CException& aCException);
  int operator!= (const CException& rException) {
    return !(operator==(rException));
  }
                       //Get accessor function for attribute

  // Selectors:

public:
	  const char*  getAction() const
  {
	  return m_szAction;
  }
                       
  //Set accessor function for attribute (protected)

protected:
  void setAction (const char* pszAction);
  void setAction (const std::string& rsAction);

  // Selectors which depend on the actual exception type:

public:
  virtual const char* ReasonText () const  ;
  virtual Int_t ReasonCode () const  ;
  const char* WasDoing () const  ;

  // Utility functions:
protected:

  virtual void DoAssign(const CException& rhs);
};

#endif
