/******************************************************************************
  Interface for class BufferPointer
******************************************************************************/

#ifndef TRANSLATORPOINTER_
#define TRANSLATORPOINTER_

#include <histotypes.h>
#include "BufferTranslator.h"

/*-----------------------------------------------------------------------------
  Definition of class TranslatorPointer
-----------------------------------------------------------------------------*/

template <class T>
class TranslatorPointer
{
  BufferTranslator& m_rUnderlyingBuffer;
  UInt_t      m_nOffset;

 public:

  TranslatorPointer<T>( BufferTranslator& Translator ) :
    m_rUnderlyingBuffer( Translator ),
    m_nOffset(0) { }

  TranslatorPointer<T>( BufferTranslator& Translator,
			const UInt_t& OffsetOfBody ) :
    m_rUnderlyingBuffer( Translator ),
    m_nOffset( OffsetOfBody ) { }

  TranslatorPointer<T>( BufferTranslator& Translator,
			Address_t const EventPointer ) :
    m_rUnderlyingBuffer(Translator),
    m_nOffset(((UInt_t)EventPointer - 
	       (UInt_t)Translator.getBuffer()) / sizeof(T) ) { }

  TranslatorPointer<T>( const TranslatorPointer<T>& RHS ) :
    m_rUnderlyingBuffer( RHS.m_rUnderlyingBuffer ),
    m_nOffset( RHS.m_nOffset ) { }

  T operator*() const;
  T operator[]( Int_t ) const;

  // Post-increment
  TranslatorPointer<T> operator++( Int_t );
  TranslatorPointer<T> operator--( Int_t );

  // Pre-increment
  TranslatorPointer<T>& operator++();
  TranslatorPointer<T>& operator--();

  // Add or subtract an offset
  TranslatorPointer<T>& operator+( Int_t Offset );
  TranslatorPointer<T>& operator-( Int_t Offset );

  // Add or subtract an offset
  TranslatorPointer<T>& operator+=( Int_t Offset );
  TranslatorPointer<T>& operator-=( Int_t Offset );

  // Relational operators
  bool operator<( const TranslatorPointer<T>& RHS );  
  bool operator==( const TranslatorPointer<T>& RHS );
  bool operator>( const TranslatorPointer<T>& RHS );
  bool operator!=( const TranslatorPointer<T>& RHS );

  // Informational functions:

   UInt_t getOffset() const
    {
      return m_nOffset;
    }
};

/*-----------------------------------------------------------------------------
  Operators for class TranslatorPointer
-----------------------------------------------------------------------------*/

template <typename T>
T TranslatorPointer<T>::operator*() const {
  Int_t nOffset = m_nOffset;
  return m_rUnderlyingBuffer.template Get<T>( nOffset );
}

template <typename T>
T TranslatorPointer<T>::operator[]( Int_t nOffset ) const {
  Int_t nTotalOffset = m_nOffset + nOffset;
  return m_rUnderlyingBuffer.template Get<T>( nTotalOffset );
}

// Pre-increment operator++
template <typename T>
TranslatorPointer<T>& TranslatorPointer<T>::operator++() {
  m_nOffset++;
  return *this;
}

// Post-increment operator++
template <typename T>
TranslatorPointer<T> TranslatorPointer<T>::operator++( Int_t j ) {
  TranslatorPointer<T> before(*this);
  operator++();
  return before;
}

// Pre-increment operator--
template <typename T>
TranslatorPointer<T>& TranslatorPointer<T>::operator--() {
  m_nOffset--;
  return *this;
}

// Post-increment operator--
template <typename T>
TranslatorPointer<T> TranslatorPointer<T>::operator--( Int_t j ) {
  TranslatorPointer<T> before(*this);
  operator--();
  return before;
}

template <typename T>
TranslatorPointer<T>& TranslatorPointer<T>::operator+( Int_t Offset )
{
  m_nOffset += Offset;
  return *this;
}

template <typename T>
TranslatorPointer<T>& TranslatorPointer<T>::operator-( Int_t Offset )
{
  m_nOffset -= Offset;
  return *this;
}

template <typename T>
TranslatorPointer<T>&
TranslatorPointer<T>::operator+=( Int_t Offset ) {
  m_nOffset += Offset;
  return *this;
}

template <typename T>
TranslatorPointer<T>&
TranslatorPointer<T>::operator-=( Int_t Offset ) {
  m_nOffset -= Offset;
  return *this;
}

/*-----------------------------------------------------------------------------
  Relational operators for class TranslatorPointer
-----------------------------------------------------------------------------*/

template <typename T>
bool TranslatorPointer<T>::operator<
( const TranslatorPointer<T>& RHS ) {
  return ( m_rUnderlyingBuffer.template Get<T>( m_nOffset ) < 
	   RHS.m_rUnderlyingBuffer.template Get<T>( RHS.m_nOffset ) );
}

template <typename T>
bool TranslatorPointer<T>::operator>
( const TranslatorPointer<T>& RHS ) {
  return ( !( *this < RHS ) && !( *this == RHS ) );
}

template <typename T>
bool TranslatorPointer<T>::operator==
( const TranslatorPointer<T>& RHS ) {
  return ( m_rUnderlyingBuffer.template Get<T>( m_nOffset ) == 
	   RHS.m_rUnderlyingBuffer.template Get<T>( RHS.m_nOffset ) );
}

template <typename T>
bool TranslatorPointer<T>::operator!=
( const TranslatorPointer<T>& RHS ) {
  return !( *this == RHS );
}

#endif
