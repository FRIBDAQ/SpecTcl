/******************************************************************************
  Interface for class BufferTranslator
******************************************************************************/

/*
  Change log:
   $Log$
   Revision 4.4  2002/08/08 11:50:34  ron-fox
   Add Change logs.

*/

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

#include <iostream.h>

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
  virtual void GetBlock( const Address_t, Int_t, Int_t ) = 0;
};

/*-----------------------------------------------------------------------------
  Definition of class SwappingBufferTranslator
-----------------------------------------------------------------------------*/

class SwappingBufferTranslator: public BufferTranslator
{
  Address_t m_pBuffer;  /*! A pointer to the buffer this holds */
  
 public:

  // Default Constructor
  SwappingBufferTranslator( Address_t pB = 0 ) {m_pBuffer = pB;}

  // Accessor functions
  virtual void GetBlock( const Address_t, int, int );
  Address_t getBuffer() { return m_pBuffer; } 
};

/*-----------------------------------------------------------------------------
  Definition of class NonSwappingBufferTranslator
-----------------------------------------------------------------------------*/

class NonSwappingBufferTranslator: public BufferTranslator
{
  Address_t m_pBuffer;   /*! A pointer to the buffer this holds */

 public:

  // Default constrcutor
  NonSwappingBufferTranslator( Address_t pB = 0 ) {m_pBuffer = pB;}

  // Accessor functions
  virtual void GetBlock( const Address_t, int, int );
  Address_t getBuffer() { return m_pBuffer; }
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
