// Class: CStreamIOError                     //ANSI C++
//
// Encapsulates error conditions on an ios
// derived object as an exception.
// Note that objects of this type carry with them
// a reference to the stream on which the condition
// was detected.  This reference may become invalid
//  if the exception propagates up the call stack
//  far enough to cause the stream to be destroyed.
//  I would have ideally liked to carry a copy of the stream,
//  however far too few ios derived classes are derived
//  from ***_withassign to make this practical.
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved StreamIOError.h
//

#ifndef __STREAMIOERROR_H  //Required for current class
#define __STREAMIOERROR_H

                               //Required for base classes
#ifndef __EXCEPTION_H
#include "Exception.h"
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif                
     
class ios;			// Forward definition.

class CStreamIOError  : public CException        
{       
public:				// Data types;
  typedef enum _IoStreamConditions {
    EndFile,
    BadSet,
    FailSet
  } IoStreamConditions;

private:  
  IoStreamConditions m_eReason; //Reason for the throw
  ios&               m_rStream; //Reference to stream <may be invalid>
  char               m_sReasonText[1000]; // Reason for failure built here.
  static char**      m_svErrorMessages; //Pointer to error message table.      


public:

  CStreamIOError(IoStreamConditions eReason,
		 const  char* pDoing, ios& rStream) :
    CException(pDoing),
    m_eReason(eReason),
    m_rStream(rStream)
  {}

  CStreamIOError(IoStreamConditions eReason,
		 const string&  rDoing, ios& rStream) :
    CException(rDoing),
    m_eReason(eReason),
    m_rStream(rStream)
  {}
    
		 
   ~ CStreamIOError ( )  // Destructor 
  { }  
  
   //Copy constructor 

  CStreamIOError (const CStreamIOError& aCStreamIOError )   : 
    CException (aCStreamIOError),
    m_eReason(aCStreamIOError.m_eReason),
    m_rStream(aCStreamIOError.m_rStream)
  { 
  }                                     

   // Operator= Assignment Operator 
  //  The existence of reference member data inhibits this
  //  operator:
  //
private:
  CStreamIOError& operator= (const CStreamIOError& aCStreamIOError);
public:
 
   //Operator== Equality Operator 
  // We'll have to compare references by comparing their
  // addresses:
  //
  int operator== (const CStreamIOError& aCStreamIOError)  {
    return ( CException::operator==(aCStreamIOError)              &&
	     (m_eReason  == aCStreamIOError.m_eReason)             &&
	     (&m_rStream == &aCStreamIOError.m_rStream)
	     );
  }
	
// Selectors:

public:

  IoStreamConditions getReason() const
  { 
    return m_eReason;
  }
  ios& getStream()
  { 
    return m_rStream;
  }
                       
  const char* getErrorMessage() const; // Return just error msg.

                       
// Mutators:

protected:

  void setReason (const IoStreamConditions am_eReason)
  { 
    m_eReason = am_eReason;
  }
                       
public:

 virtual   const char* ReasonText ()    const;
 virtual   Int_t ReasonCode ()    const;
 

};

#endif
