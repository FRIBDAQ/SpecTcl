/******************************************************************************
  Implementation of class BufferTranslator
******************************************************************************/

/*
  Change Log:
   $Log$
   Revision 4.4  2002/08/08 11:50:34  ron-fox
   Add Change logs.

*/

#include "BufferTranslator.h"
#include <stdio.h>

/*-----------------------------------------------------------------------------
  Name:  MyEndianess

  Purpose:  Determine the running system endianess
-----------------------------------------------------------------------------*/

BufferFactory::Endian MyEndianess()
{
  union {
    Int_t  asInt;
    Char_t asArray[sizeof(Int_t)];
  } x;
  x.asInt =1;

  if (x.asArray[0] == 1 ) {
    return BufferFactory::little;
  }
  
  else {
    return BufferFactory::big;
  }
}

/*-----------------------------------------------------------------------------
  Name:  BufferFactory::CreateBuffer

  Purpose:  Examine the lsignature of a buffer header and return a pointer
            to the appropriate BufferTranslator.
-----------------------------------------------------------------------------*/

BufferTranslator*
BufferFactory::CreateBuffer( Address_t pBuffer, Int_t Signature32 )
{
  if( Signature32 == 0x01020304 ) {
    return new NonSwappingBufferTranslator(pBuffer);
  } 
  else {
    return new SwappingBufferTranslator(pBuffer);
  }
}

/*-----------------------------------------------------------------------------
  Name:  SwappingBufferTranslator::GetBlock

  Purpose:  Copy memory from buffer and return swapped
-----------------------------------------------------------------------------*/

void SwappingBufferTranslator::GetBlock( const Address_t pItem, 
					 Int_t size, Int_t iOffset )
{
  UChar_t *pBuff = (UChar_t *) m_pBuffer;
  UChar_t *Item = (UChar_t *) pItem;

  for( Int_t I = 0; I < size; I++ ) {
    Item[I] = pBuff[iOffset+I];
  }

  UChar_t *a = (UChar_t *) pItem, b;
  for( Int_t I = 0; I < size/2; I++ ) {
    b = a[I];
    a[I] = a[(size-1) - I];
    a[(size-1) - I] = b;
  }
}

/*-----------------------------------------------------------------------------
  Name:  NonSwappingBufferTranslator::GetBlock

  Purpose:  Copy memory from buffer and return
-----------------------------------------------------------------------------*/

void NonSwappingBufferTranslator::GetBlock( const Address_t pItem, 
					    Int_t size, Int_t iOffset )
{
  UChar_t *pBuff = (UChar_t *) m_pBuffer;
  UChar_t *Item = (UChar_t *) pItem;
  
  for( Int_t I = 0; I < size; I++ ) {
    Item[I] = pBuff[iOffset+I];
  }
}

/*-----------------------------------------------------------------------------
  Name:  BufferTranslator::GetByte

  Purpose:  Get the byte at offset iOffset from the buffer
-----------------------------------------------------------------------------*/

UChar_t BufferTranslator::GetByte( UInt_t iOffset )
{
  UChar_t Byte;
  GetBlock(&Byte, sizeof(UChar_t), iOffset);
  return (UChar_t)(Byte);  
}

/*-----------------------------------------------------------------------------
  Name:  BufferTranslator::GetWord

  Purpose:  Get the word at offset iOffset from the buffer and byteswap it
            if necessary
-----------------------------------------------------------------------------*/

Short_t BufferTranslator::GetWord( UInt_t iOffset )
{
  Short_t Word;
  GetBlock(&Word, sizeof(Short_t), iOffset);
  return (Short_t)(Word);
}

/*-----------------------------------------------------------------------------
  Name:  BufferTranslator::GetLongword

  Purpose:  Get the longword at offset iOffset from the buffer and byteswap it
            if necessary
-----------------------------------------------------------------------------*/

Long_t BufferTranslator::GetLongword( UInt_t iOffset )
{
  Long_t Longword;
  GetBlock(&Longword, sizeof(Long_t), iOffset);
  return (Long_t)(Longword);
}
