// Class: CSpectrumFormatError                     //ANSI C++
//
// Encapsulates a specialized exception class which
// Describes the set of errors on file formats which might
// be encountered when reading a spectrum from file.
// 
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved SpectrumFormatError.h
//

#ifndef __SPECTRUMFORMATERROR_H  //Required for current class
#define __SPECTRUMFORMATERROR_H

                               //Required for base classes
#ifndef __EXCEPTION_H
#include <Exception.h>
#endif

#ifndef __HISTOTYPES_H         
#include <histotypes.h>        //Required for include files
#endif
                               
class CSpectrumFormatError  : public CException        
{ 
public:
  typedef enum _ErrorReason {
    InvalidHeader,
    IncompatibleFormat,
    FileFormatCorrupt,
    InvalidChannels,
    OverflowChannels
  } ErrorReason;

private:                      
			
  ErrorReason   m_eError;	//Why exception was generated.
  static char** m_svErrorStrings; //Pointer to list of error strings.        

protected:

public:

   // Constructors and other cannonical operations:

  CSpectrumFormatError(ErrorReason eReason,
		       const char* pDoing) :
    CException(pDoing), 
    m_eError(eReason)
  {}
   ~ CSpectrumFormatError ( )  // Destructor 
  { }  

  
   //Copy constructor 

  CSpectrumFormatError (const CSpectrumFormatError& aCSpectrumFormatError ) : 
    CException (aCSpectrumFormatError) ,
    m_eError(aCSpectrumFormatError.m_eError)
  { 
    
  }                                     

   // Operator= Assignment Operator 

  CSpectrumFormatError& operator= (const CSpectrumFormatError& aCSpectrumFormatError) {
    if(this != &aCSpectrumFormatError) {
      CException::operator=(aCSpectrumFormatError);
      m_eError = aCSpectrumFormatError.m_eError;
    }
    return *this;
  }
 
   //Operator== Equality Operator 

  int operator== (const CSpectrumFormatError& aCSpectrumFormatError)  {
    return ( CSpectrumFormatError::operator==(aCSpectrumFormatError) &&
	     (m_eError   == aCSpectrumFormatError.m_eError));
    
  }
	
// Selectors:

public:

  ErrorReason getError() const
  { return m_eError;
  }
                       
                       
                       
public:

  virtual   const char* ReasonText () const;
  virtual   Int_t ReasonCode ()    const;
 
protected:

private:

};

#endif
