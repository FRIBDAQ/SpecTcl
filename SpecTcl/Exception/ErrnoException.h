

//  CErrnoException.h:
//
//    This file defines the CErrnoException class.
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

#ifndef __ERRNOEXCEPTION_H  //Required for current class
#define __ERRNOEXCEPTION_H
                               //Required for base classes
#ifndef __EXCEPTION_H
#include "Exception.h"
#endif                               
  
#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __CRTL_ERRNO_H
#include <errno.h>
#define __CRTL_ERRNO_H
#endif
                           
#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

  
class CErrnoException  : public CException        
{
  Int_t m_nErrno;  // // Snapshot of errno at construction time.
  
public:

			//Constructor with arguments

  CErrnoException(const char* pszAction) :
    m_nErrno(errno),
    CException(pszAction) {}
  CErrnoException(const std::string& rsAction) :
    m_nErrno(errno),
    CException(rsAction) {}
  ~CErrnoException ( ) { }       //Destructor

  // Copy Constructor:

  CErrnoException (const CErrnoException& aCErrnoException )   : 
    CException (aCErrnoException) 
  {   
    m_nErrno = aCErrnoException.m_nErrno;             
  }                                     
			//Operator= Assignment Operator
     
  CErrnoException& operator= (const CErrnoException& aCErrnoException)
  { 
    if (this == &aCErrnoException) return *this;          
    CException::operator= (aCErrnoException);
    m_nErrno = aCErrnoException.m_nErrno;
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator

  int operator== (const CErrnoException& aCErrnoException)
  { 
    return ((CException::operator== (aCErrnoException)) &&
	    (m_nErrno == aCErrnoException.m_nErrno) 
	    );
  }
  // Selectors:  Note typically these are not needed.
               
public:
  Int_t getErrno() const
  {
    return m_nErrno;
  }
  // Mutating selectors:  For derived classes only.
protected:                   
  void setErrno(Int_t am_nErrno)
  { 
    m_nErrno = am_nErrno;
  }                   

  // Functions of the class:
public:

  virtual   const char* ReasonText () const  ;
  virtual   Int_t       ReasonCode () const  ;
 
};

#endif













