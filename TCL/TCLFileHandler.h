//  CTCLFileHandler.h:
//
//    This file defines the CTCLFileHandler class.
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

#ifndef __TCLFILEHANDLER_H	//Required for current class
#define __TCLFILEHANDLER_H
				//Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif                            

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __CRT_UNISTD_H
#include <unistd.h>
#define __CRT_UNISTD_H
#endif

#ifndef __CRT_STDIO_H
#include <stdio.h>
#define __CRT_STDIO_H
#endif
  
#ifndef __CXX_FSTREAM_H
#include <fstream.h>   
#define __CXX_FSTREAM_H
#endif

                          
class CTCLFileHandler  : public CTCLInterpreterObject        
{
  UInt_t m_nFid;  // File descriptor associated with the object.
  
public:
			//Default constructor

  CTCLFileHandler (CTCLInterpreterObject* pInterp,  
		   UInt_t am_nFid = STDIN_FILENO) :       
    CTCLInterpreterObject(pInterp->getInterpreter()),
    m_nFid (am_nFid)  
  { }        
  CTCLFileHandler(CTCLInterpreterObject* pInterp,
		  FILE* pFile) :
    CTCLInterpreterObject(pInterp->getInterpreter()),
    m_nFid(fileno(pFile))
  { }
  CTCLFileHandler(CTCLInterpreterObject* pInterp,
		  fstream& rFile) :
    CTCLInterpreterObject(*pInterp),
    m_nFid(rFile.rdbuf()->fd())
  { }
  CTCLFileHandler(CTCLInterpreter* pInterp,
		  UInt_t am_nFid = STDIN_FILENO) :
    CTCLInterpreterObject(pInterp),
    m_nFid(am_nFid)
  { }
  CTCLFileHandler(CTCLInterpreter* pInterp,
		  FILE* pFile) :
    CTCLInterpreterObject(pInterp),
    m_nFid(fileno(pFile))
  { }
  CTCLFileHandler(CTCLInterpreter* pInterp,
		  fstream& rFile) :
    CTCLInterpreterObject(pInterp),
    m_nFid(rFile.rdbuf()->fd())
  { }
  ~CTCLFileHandler ( ) {Clear(); }       //Destructor	
			//Copy constructor

  CTCLFileHandler (const CTCLFileHandler& aCTCLFileHandler )   : 
    CTCLInterpreterObject (aCTCLFileHandler) 
  {   
    m_nFid = aCTCLFileHandler.m_nFid;
                
  }                                     

			//Operator= Assignment Operator

  CTCLFileHandler& operator= (const CTCLFileHandler& aCTCLFileHandler)
  { 
    if (this == &aCTCLFileHandler) return *this;          
    CTCLInterpreterObject::operator= (aCTCLFileHandler);
    m_nFid = aCTCLFileHandler.m_nFid;
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLFileHandler& aCTCLFileHandler) const
  { return (
	    (CTCLInterpreterObject::operator== (aCTCLFileHandler)) &&
	    (m_nFid == aCTCLFileHandler.m_nFid) 
	    );
  }                             
  // Readonly selectors:        

public:
  UInt_t getFid() const
  {
    return m_nFid;
  }
  // Mutators:

public:                       
  void setFid (UInt_t am_nFid)
  { 
    m_nFid = am_nFid;
  }
  // operations and overridables:

public:
  virtual   void operator() (int mask)   = 0;
  static  void CallbackRelay (ClientData pObject, int mask)  ;  
  void Set (int mask)  ;
  void Clear ()  ;
  

};

#endif
