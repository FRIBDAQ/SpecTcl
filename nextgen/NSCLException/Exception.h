/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


//  CException.h:
//
//    This file defines the CException class.
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
/*!
   Change Log:
   $Log$
   Revision 5.1  2004/11/29 16:56:04  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 1.1.1.1  2003/11/03 17:54:41  ron-fox
   Rename Exception -> NSCLException for Cygwin

   Revision 4.2  2003/03/25 12:21:36  ron-fox
   Added Change log comment generated from the CVS $Log$
   Added Change log comment generated from the CVS Revision 5.1  2004/11/29 16:56:04  ron-fox
   Added Change log comment generated from the CVS Begin port to 3.x compilers calling this 3.0
   Added Change log comment generated from the CVS
   Added Change log comment generated from the CVS Revision 1.1.1.1  2003/11/03 17:54:41  ron-fox
   Added Change log comment generated from the CVS Rename Exception -> NSCLException for Cygwin
   Added Change log comment generated from the CVS tag.

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
