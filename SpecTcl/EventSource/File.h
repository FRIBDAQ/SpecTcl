//  CFile.h:
//
//    This file defines the CFile class.
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

#ifndef __FILE_H  //Required for current class
#define __FILE_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
  
#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif
                             
class CFile      
{
   UInt_t m_nFd;		  // File descriptor open on the file.
   FileState_t m_eState;  // Current state of the file.
  
public:

	// Constructors:
	// Note that since Open is virtual, we cannot do a 
    // constructor based on the name of a file.
    //
		
  CFile () :  m_nFd(0),  m_eState(kfsClosed)   { }  // Default
  CFile (  UInt_t am_nFd)                           // From fd.
  :   m_nFd (am_nFd),  m_eState (kfsOpen)  { }
  virtual  ~ CFile ( ) { 
	  if(m_eState == kfsOpen) 
		  Close(); 
  }													//Destructor


			//Copy constructor

  CFile (const CFile& aCFile ) ;

			//Operator= Assignment Operator

  CFile& operator= (const CFile& aCFile);

			//Operator== Equality Operator

  Int_t operator== (const CFile& aCFile);
  Int_t operator!= (const CFile& aCFile) { return (!operator==(aCFile)); }
  
	// Read selectors:


  UInt_t getFd() const
  {
	  return m_nFd;
  }

  FileState_t getState() const
  {
	  return m_eState;
  }
      // Write selectors are protected to allow access by 'smart'
	  // derived classes:
                       
  void setFd (UInt_t am_nFd)
  {
	  m_nFd = am_nFd;
  }

  void setState (FileState_t am_eState)
  {
	  m_eState = am_eState;
  }

	// Class operations:

  virtual   Int_t Read (Address_t pBuffer, UInt_t nSize)  ;
  virtual   Int_t Write (const Address_t pBuffer, UInt_t nBytes)  ;
  virtual   void Open (const std::string& rsFilename, UInt_t nAccess)  ;
  virtual   void Open(UInt_t nFd);
  virtual   void Close ()  ;
  virtual   Bool_t IsReadable(UInt_t nMs) const;

  // Utility functions available to derived classes:

protected:
  virtual void DoAssign(const CFile& rFile);
  void         AssertOpen() const;
 
};

#endif
