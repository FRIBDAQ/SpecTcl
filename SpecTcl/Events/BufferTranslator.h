/******************************************************************************
  Interface for class BufferTranslator
******************************************************************************/

#include <buffer.h>
#include <histotypes.h>
#include "TranslatorPointer.h"

#ifndef BUFFERTRANSLATOR_
#define BUFFERTRANSLATOR_

/*-----------------------------------------------------------------------------
  Definition of class BufferTranslator
-----------------------------------------------------------------------------*/

class BufferTranslator
{
 public:
  
  UChar_t GetByte(UInt_t);
  Short_t GetWord(UInt_t);
  Long_t GetLongword(UInt_t);

  virtual Address_t getBuffer() = 0;
  virtual void GetBlock(const Address_t, Int_t, Int_t) = 0;

  template <class T>
    T Get( Int_t );
};

template <class T>
T BufferTranslator::Get(Int_t iOffset) {
  iOffset *= sizeof( T );                 // Scale the offset
  T Result;
  GetBlock(&Result, sizeof(Result), iOffset);  // Get the block
  return Result;
}

/*-----------------------------------------------------------------------------
  class SwappingBufferTranslator will contain byteswapped data
-----------------------------------------------------------------------------*/

class SwappingBufferTranslator: public BufferTranslator
{
 public:
  
  SwappingBufferTranslator(Address_t pB = 0) {m_pBuffer = pB;}
  
  void GetBlock(const Address_t, Int_t, Int_t);
  Address_t getBuffer() { return m_pBuffer; }

 private:
  
  Address_t m_pBuffer;
  
};

/*-----------------------------------------------------------------------------
  class NonSwappingBufferTranslator non-byteswapped data
-----------------------------------------------------------------------------*/

class NonSwappingBufferTranslator: public BufferTranslator
{
 public:

  NonSwappingBufferTranslator(Address_t pB = 0) {m_pBuffer = pB;}

  void GetBlock(const Address_t, Int_t, Int_t);
  Address_t getBuffer() { return m_pBuffer; }

 private:

  Address_t m_pBuffer;

};

/*-----------------------------------------------------------------------------
  Definition of factory class TranslatorFactory
-----------------------------------------------------------------------------*/

class TranslatorFactory
{
 public: 
  
  enum Endian {little, big};
  
  static BufferTranslator* 
    CreateTranslator(Address_t pBuffer, Endian eBufferOrdering);

};

//  MyEndianess returns the endianess of the running system
TranslatorFactory::Endian MyEndianess();

class BufferFactory
{
 public:
  
  static BufferTranslator* CreateBuffer(Address_t pBuffer, Int_t Signature32);
};

#endif

