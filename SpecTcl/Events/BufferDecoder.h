//  CBufferDecoder.h:
//
//    This file defines the CBufferDecoder class.
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

#ifndef __BUFFERDECODER_H  //Required for current class
#define __BUFFERDECODER_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __BUFFERTRANSLATOR
#include <BufferTranslator.h>
#define __BUFFERTRANSLATOR
#endif

#ifndef __TRANSLATORPOINTER
#include <TranslatorPointer.h>
#define __TRANSLATORPOINTER
#endif

//
// Forward Class References:
// 
class CAnalyzer;
//
//  The abstract base class for buffer decoders.  These know about
//  the global structure of data buffers from a data acquisition system.
//
                               
class CBufferDecoder      
{
  Address_t m_pBuffer;  // Pointer to the buffer itself
  BufferTranslator* m_pTranslator;  // Pointer to the buffer translator
  UInt_t    m_nSize;    // Number of bytes in buffer.

public:
			//Default constructor
			//Update to access base class attributes 
			//Update to access 1:1 part class attributes 
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes
			//Ensure initial values entered
  CBufferDecoder () :  m_pBuffer(0), m_pTranslator(0), m_nSize(0)   { } 
  virtual  ~ CBufferDecoder ( ) { }       //Destructor

	
			//Copy constructor
				// This is allowed since the 
				// pointer is already supposed to be
				// maintained by a caller.
				// 
  CBufferDecoder (const CBufferDecoder& aCBufferDecoder ) 
  {   
    m_pBuffer = aCBufferDecoder.m_pBuffer;                
    m_pTranslator = aCBufferDecoder.m_pTranslator;
  }                                     

			//Operator= Assignment Operator

  CBufferDecoder& operator= (const CBufferDecoder& aCBufferDecoder)
  { 
    if (this == &aCBufferDecoder) return *this;          
    m_pBuffer = aCBufferDecoder.m_pBuffer;        
    m_pTranslator = aCBufferDecoder.m_pTranslator;
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CBufferDecoder& aCBufferDecoder)
  { 
    return (
	    (m_pBuffer == aCBufferDecoder.m_pBuffer) 
	    );
  }                             
    
  // Selectors:
public:
                  //Get accessor function for attribute
  const Address_t getBuffer() const
  {
    return m_pBuffer;
  }
  BufferTranslator* getBufferTranslator()
  {
    return m_pTranslator;
  }
  // Mutating selectors  protected from other than derived classes.
  //
protected:

  void setBuffer (Address_t am_pBuffer)
  { 
    m_pBuffer = am_pBuffer;
  }
  //
  // The pure virtual functions below are very  selector like.  However
  // it requires the sort of detailed knowledge of the underlying buffer
  // structures objects of classes derived from this are supposed to have
  // to implement them so they must be pure:
  //
public:
  virtual   const Address_t getBody ()    = 0;
  virtual   UInt_t getBodySize ()    = 0;
  virtual   UInt_t getRun ()    = 0;
  virtual   UInt_t getEntityCount ()    = 0;
  virtual   UInt_t getSequenceNo ()    = 0;
  virtual   UInt_t getLamCount ()    = 0;
  virtual   UInt_t getPatternCount ()    = 0;
  virtual   UInt_t getBufferType ()    = 0;
  virtual   void getByteOrder (Short_t& Signature16, 
			       Int_t& Signature32)   = 0;
  virtual   string getTitle() = 0;
  //
  // Default behavior for this operation can be supplied in terms of
  // the virtual selectors.
  // 
  virtual   void operator() (UInt_t nBytes, Address_t pBuffer, 
			     CAnalyzer& rAnalyzer)  ;

  // These are called on attach/detach:

  virtual void OnAttach(CAnalyzer& rAnalyzer) {}
  virtual void OnDetach(CAnalyzer& rAnalyzer) {}
  
  // Utilities available to derived classes:

protected:
  void ThrowIfNoBuffer(const char* pszWhatImDoing);
  
  //template <class T>
  // TranslatorPointer<T> getTranslatorPointer(const BufferTranslator&,
  //				      const Int_t nOffset);
};

#endif

