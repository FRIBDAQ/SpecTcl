//  CTCLException.h:
//
//    This file defines the CTCLException class.
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

#ifndef __CTCLEXCEPTION_H  //Required for current class
#define __TCLEXCEPTION_H
                               //Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif

#ifndef __TCLRESULT_H
#include "TCLResult.h"
#endif

                               //Required for base classes
#ifndef __EXCEPTION_H
#include "Exception.h"
#endif  
                           
#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h
#endif

                               
class CTCLException  : public CTCLInterpreterObject ,public CException
{
   Int_t m_nReason;  // Reason the exception was thrown
				// TCL_ERROR    - Error in script.
				// TCL_BREAK    - Break from loop.
				// TCL_CONTINUE - continue loop.
				// TCL_RETURN   - fuction return.
				// NOTE: Really no business throwing anything
				//       but TCL_ERRORs.
public:
			//Default constructor

  CTCLException (CTCLInterpreter& am_rInterpreter,  
		 Int_t am_nReason,
		 const char* pString) :
    CTCLInterpreterObject(&am_rInterpreter), 
    CException(pString),
    m_nReason(am_nReason)
  {
  }
  CTCLException(CTCLInterpreter& am_rInterpreter,
		Int_t am_nReason,
		const std::string& rString) : 
    CTCLInterpreterObject(&am_rInterpreter),
    CException(rString),
    m_nReason(am_nReason)
  {
  }
  virtual ~CTCLException ( ) { }       //Destructor
	
			//Copy constructor

  CTCLException (const CTCLException& aCTCLException )   : 
    CTCLInterpreterObject (aCTCLException),   
    CException (aCTCLException) 
  {   
    m_nReason = aCTCLException.m_nReason;            
  }                                     

			//Operator= Assignment Operator

  CTCLException operator= (const CTCLException& aCTCLException)
  { 
    if (this == &aCTCLException) return *this;          
    CTCLInterpreterObject::operator= (aCTCLException);
    CException::operator= (aCTCLException);

    m_nReason = aCTCLException.m_nReason;
    
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLException& aCTCLException)
  { return (
	    (CTCLInterpreterObject::operator== (aCTCLException)) &&
	    (CException::operator== (aCTCLException)) &&
	    (m_nReason == aCTCLException.m_nReason) 
	    );
  }                  
  // Selectors:  - Stick to generic CException and CTCLInterpreterObject
  //               interfaces unless you really know what you're doing.
  //
public:

  Int_t getReason() const
  {
    return m_nReason;
  }
  //
  //  Mutators:   Available to derivce classes only:
  //
protected:
                       
          
  void setInterpreter (CTCLInterpreter& am_rInterpreter)
  { 
    Bind(am_rInterpreter);
  }
  void setReason (Int_t am_nReason)
  { 
    m_nReason = am_nReason;
  }
  // TCL Specific interface:
  //
public:
  void AddErrorInfo (const char* pMessage)  ;
  void AddErrorInfo(const string& rMessage) {
    AddErrorInfo(rMessage.c_str());
  }
  void AddErrorInfo(const CTCLString& rMessage) {
    AddErrorInfo((const char*)(rMessage));
  }

  void SetErrorCode (const char* pMessage, 
		     const char* pMnemonic="???", 
		     const char* pFacility="TCL", 
		     const char* pSeverity="FATAL")  ;
  void SetErrorCode(const string rMessage,
		    const string &rMnemonic=string("???"),
		    const string &rFacility=string("TCL"),
		    const string &rSeverity=string("FATAL")) {
    SetErrorCode(rMessage.c_str(), rMnemonic.c_str(),
		 rFacility.c_str(), rSeverity.c_str());
  }

  CTCLResult GetResult ()  const;  
  //
  // CException generic interface:
  //
  virtual   const char* ReasonText () const;
  virtual   Int_t ReasonCode () const  ;
};

#endif
