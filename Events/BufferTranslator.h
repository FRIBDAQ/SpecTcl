/******************************************************************************
  Interface for class BufferTranslator
******************************************************************************/

#ifndef __BUFFERTRANSLATOR_H
#define __BUFFERTRANSLATOR_H

#ifndef __BUFFER_H
#include <buffer.h>
#endif

#ifndef __TRANSLATORPOINTER_H
#include "TranslatorPointer.h"
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

/*-----------------------------------------------------------------------------
  Definition of class BufferTranslator
-----------------------------------------------------------------------------*/

class BufferTranslator
{
 public:
  
  UChar_t GetByte( UInt_t );
  Short_t GetWord( UInt_t );
  Long_t GetLongword( UInt_t );

  virtual Address_t getBuffer() = 0;
  virtual void GetBlock( Address_t pResult, 
			 Int_t nOffset, Int_t nSize ) = 0;
};

/*-----------------------------------------------------------------------------
  Definition of class SwappingBufferTranslator
-----------------------------------------------------------------------------*/

class SwappingBufferTranslator: public BufferTranslator
{
 public:
  
  SwappingBufferTranslator( Address_t pB = 0 ) {m_pBuffer = pB;}
  
  void GetBlock( Address_t pResult, Int_t nOffset, Int_t nSize );
  Address_t getBuffer() { return m_pBuffer; }

 private:
  
  Address_t m_pBuffer;
  
};

/*-----------------------------------------------------------------------------
  Definition of class NonSwappingBufferTranslator
-----------------------------------------------------------------------------*/

class NonSwappingBufferTranslator: public BufferTranslator
{
 public:

  NonSwappingBufferTranslator( Address_t pB = 0 ) {m_pBuffer = pB;}

  void GetBlock( Address_t pResult, Int_t nOffset, Int_t nSize );
  Address_t getBuffer() { return m_pBuffer; }

 private:

  Address_t m_pBuffer;

};

/*-----------------------------------------------------------------------------
  Definition of factory class BufferFactory
-----------------------------------------------------------------------------*/

class BufferFactory
{
 public:
  enum Endian {little, big};
  static BufferTranslator* 
    CreateBuffer(Address_t pBuffer, Int_t Signature32);
};

//  MyEndianess returns the endianess of the running system
BufferFactory::Endian MyEndianess();

#endif
