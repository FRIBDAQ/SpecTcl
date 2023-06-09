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

#ifndef BUFFERDECODER_H  //Required for current class
#define BUFFERDECODER_H

#include <histotypes.h>
#include <string>
#include <BufferTranslator.h>
#include <TranslatorPointer.h>

// Forward Class References:
class CAnalyzer;

//  The abstract base class for buffer decoders.  These know about
//  the global structure of data buffers from a data acquisition system.
class CBufferDecoder {
  Address_t m_pBuffer;  // Pointer to the buffer itself
  BufferTranslator* m_pTranslator;  // Pointer to the buffer translator
 protected:
  UInt_t    m_nSize;    // Number of bytes in buffer.

 public:
  //Default constructor
  //Update to access base class attributes 
  //Update to access 1:1 part class attributes 
  //Update to access 1:M part class attributes
  //Update to access 1:1 associated class attributes
  //Update to access 1:M associated class attributes
  //Ensure initial values entered
  CBufferDecoder() : m_pBuffer(0), m_pTranslator(0), m_nSize(0) {}
  virtual ~CBufferDecoder() {
    delete m_pTranslator;	/* They get dynamically created. */
  } // Destructor.

  // Copy constructor not allowed since there's no good way to
  // copy construct the buffer translator yet (would need a clone
  // operator).
private:
  CBufferDecoder(const CBufferDecoder& aCBufferDecoder);
  CBufferDecoder& operator=(const CBufferDecoder& aCBufferDecoder);
public:

  //Operator== Equality Operator
  int operator==(const CBufferDecoder& aCBufferDecoder) {
    return (
	    (m_pBuffer == aCBufferDecoder.m_pBuffer)
	    );
  }

  // Selectors:
 public:
  //Get accessor function for attribute
  const Address_t getBuffer() const {
    return m_pBuffer;
  }

  virtual BufferTranslator* getBufferTranslator() {
    return m_pTranslator;
  }

  // Mutating selectors  protected from other than derived classes.
 protected:
  void setBuffer (Address_t am_pBuffer) {
    m_pBuffer = am_pBuffer;
  }

  // The pure virtual functions below are very  selector like.  However
  // it requires the sort of detailed knowledge of the underlying buffer
  // structures objects of classes derived from this are supposed to have
  // to implement them so they must be pure:
 public:
  virtual const Address_t getBody() = 0;
  virtual UInt_t getBodySize() = 0;
  virtual UInt_t getRun() = 0;
  virtual UInt_t getEntityCount() = 0;
  virtual UInt_t getSequenceNo() = 0;
  virtual UInt_t getLamCount() = 0;
  virtual UInt_t getPatternCount() = 0;
  virtual UInt_t getBufferType() = 0;
  virtual void getByteOrder(Short_t& Signature16,
			    Int_t& Signature32) = 0;
  virtual std::string getTitle() = 0;
  //
  // Default behavior for this operation can be supplied in terms of
  // the virtual selectors.
  // 
  virtual void operator() (UInt_t nBytes,
			   Address_t pBuffer,
			   CAnalyzer& rAnalyzer);

  virtual void OnAttach(CAnalyzer& rAnalyzer);
  virtual void OnDetach(CAnalyzer& rAnalyzer);
  virtual bool blockMode();	// True if data source must deliver fixed sized blocks.
  
  // Data source events:
  
  virtual void OnSourceAttach();
  virtual void OnSourceDetach();
  virtual void OnEndFile();

  // Utilities available to derived classes:
 protected:
  void ThrowIfNoBuffer(const char* pszWhatImDoing);

  //template <class T>
  // TranslatorPointer<T> getTranslatorPointer(const BufferTranslator&,
  //				      const Int_t nOffset);
};

#endif
