/*!
  \class: TranslatorPointer
  \file:  TranslatorPointer.h
  
  Encapsulates a TranslatorPointer object. A TranslatorPointer holds
  a pointer to a dynamically allocated BufferTranslator. By wrapping 
  the BufferTranslator in the TranslatorPointer, it can be used as 
  though it were a normal pointer. Note that the BufferTranslator must
  then be deleted by the caller.

  Author:
    Jason Venema
    NSCL
    Michigan State University
    East Lansing, MI 48824-1321
    mailto:venemaja@msu.edu
*/

#ifndef __TRANSLATORPOINTER_H
#define __TRANSLATORPOINTER_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __BUFFERTRANSLATOR_H
#include "BufferTranslator.h"
#endif

#ifndef __RANGEERROR_H
#include <RangeError.h>
#endif

class BufferTranslator;  // forward class declaration

template <class T>
class TranslatorPointer
{
  BufferTranslator& m_rUnderlyingBuffer; /*! Reference to the BufferTrans. */
  UInt_t            m_nOffset;           /*! Offset into the buffer*/
  
 public:

  // Basic constructor for this receives a reference to a BufferTranslator
  // and defaults m_nOffset to 0
  TranslatorPointer<T>(BufferTranslator& Translator) :
    m_rUnderlyingBuffer(Translator),
    m_nOffset(0) { }

  // Constructor which receives an offset
  TranslatorPointer<T>(BufferTranslator& Translator, const UInt_t& Offset) :
    m_rUnderlyingBuffer(Translator),
    m_nOffset(Offset) { }

  // Constructor which receives a BufferTranslator and a address
  // from which to calculate m_nOffset
  TranslatorPointer<T>(BufferTranslator& Translator, Address_t const Addr) :
    m_rUnderlyingBuffer(Translator),
    m_nOffset(((UInt_t)Addr - (UInt_t)Translator.getBuffer()) / sizeof(T))
    { }

  // Copy constructor for this
  TranslatorPointer<T>(const TranslatorPointer<T>& RHS) :
    m_rUnderlyingBuffer(RHS.m_rUnderlyingBuffer),
    m_nOffset(RHS.m_nOffset) { }

  // Assignment operator=
  TranslatorPointer<T> operator=(const TranslatorPointer<T>& RHS)
    {
      m_rUnderlyingBuffer = RHS.m_rUnderlyingBuffer;
      m_nOffset = RHS.m_nOffset;
      return *this;
    }

  // Operator= type conversion operator
  template<class U>
    TranslatorPointer<T>& operator=(const TranslatorPointer<U>& rhs)
    {
      m_rUnderlyingBuffer = rhs.getBuffer();
      m_nOffset = rhs.getOffset() * sizeof(U) / sizeof(T);
      return *this;
    }

  // Member functions for class TranslatorPointer
 public:

  /*!
    \fn T operator*() const

    Purpose: 
       Dereference this at the current offset i.e. return the value
       stored in m_rUnderlyingBuffer at the current m_nOffset.
   */
  inline T operator*() const
    {
      return (m_rUnderlyingBuffer.template Get<T>(m_nOffset));
    }

  /*!
    \fn T operator[](Int_t nOffset) const

    Purpose:
       Indexing operator returns the value stored at position
       nOffset*sizeof(T) in m_rUnderlyingBuffer.
   */
  inline T operator[](Int_t nOffset) const
    {
      return (m_rUnderlyingBuffer.template Get<T>(nOffset));
    }

  /*!
    \fn TranslatorPointer<T> operator++(Int_t)

    Purpose:
       Post-increment operator++ increments the offset held by this, 
       and returns the pre-incremented value of this.

    \param Int_t - dummy argument indicating post-increment form 
  */
  inline TranslatorPointer<T> operator++(Int_t)
    {
      TranslatorPointer<T> before(*this);
      operator++();
      return before;
    }

  /*!
    \fn TranslatorPointer<T> operator--(Int_t)

    Purpose:
       Post-decrement operator-- decrements the offset held by this,
       and returns the pre-decremented value of this.

    \param Int_t - dummy argument indicating post-decrement form
   */
  inline TranslatorPointer<T> operator--(Int_t)
    {
      TranslatorPointer<T> before(*this);
      operator--();
      return before;
    }

  /*!
    \fn TranslatorPointer<T>& operator++()

    Purpose:
       Pre-increment operator++ increments the offset held by this,
       and returns a reference to this.
  */
  inline TranslatorPointer<T>& operator++()
    {
      m_nOffset++;
      return *this;
    }

  /*!
    \fn TranslatorPointer<T>& operator

    Purpose:
       Pre-decrement operator-- decrements the offset held by this, 
       and returns a reference to this.
  */
  inline TranslatorPointer<T>& operator--()
    {
      m_nOffset--;
      return *this;
    }

  /*!
    \fn TranslatorPointer<T> operator+(Int_t Offset)

    Purpose:
       Add an offset to the m_nOffset value of this.

    \param Int_t Offset - an integer to add to m_nOffset
  */
  inline TranslatorPointer<T> operator+(Int_t Offset)
    {
      return TranslatorPointer<T>(m_rUnderlyingBuffer, m_nOffset+Offset);
    }

  /*!
    \fn TranslatorPointer<T> operator-(Int_t Offset)

    Purpose:
       Subtract an offset to the m_nOffset value of this. Throws
       a CRangeError exception if Offset > m_nOffset.

    \param Int_t Offset - an integer value to subtract from m_nOffset
   */
  inline TranslatorPointer<T> operator-(Int_t Offset)
    {
      if(Offset <= m_nOffset) {
	return TranslatorPointer<T>(m_rUnderlyingBuffer, m_nOffset-Offset);
      }
      else {
	CRangeError re(0, m_nOffset, Offset,
	      "TranslatorPointer::operator-() - outside of buffer window");
	throw re;
      }
    }

  /*!
    \fn TranslatorPointer<T>& operator+=(Int_t Offset)

    Purpose:
       Add an offset to the m_nOffset value of this.
       Returns a reference to this.

    \param Int_t Offset - an integer value to add to m_nOffset
  */
  inline TranslatorPointer<T>& operator+=(Int_t Offset)
    {
      m_nOffset += Offset;
      return *this;
    }

  /*!
    \fn TranslatorPointer<T>& operator-=(Int_t Offset)

    Purpose:
       Subtract an offset from the m_nOffset value of this.
       Returns a reference to this or throws a CRangeError
       exception if Offset > m_nOffset.

    \param Int_t Offset - an integer value to subtract from m_nOffset
  */
  inline TranslatorPointer<T>& operator-=(Int_t Offset)
    {
      if(m_nOffset >= Offset) {
	m_nOffset -= Offset;
      }
      else {
	CRangeError re(0, m_nOffset, Offset,
	     "TranslatorPointer<T>& operator-=() - outside of buffer window");
	throw re;
      }
      return *this;
    }

  /*!
    \fn bool operator<<(const TranslatorPointer<T>& RHS)

    Purpose:
       Less than operator. Compare the m_nOffset value of this with the 
       m_nOffset of RHS. Return true if the former is less than the latter.

    \param const TranslatorPointer<T>& RHS - a reference to a TranslatorPointer
                                             to compare with this
   */
  inline bool operator<(const TranslatorPointer<T>& RHS)
    {  
      return (m_rUnderlyingBuffer.template Get<T>(m_nOffset) < 
	      RHS.m_rUnderlyingBuffer.template Get<T>(RHS.m_nOffset));
    } 
 
  /*!
    \fn bool operator==(const TranslatorPointer<T>& RHS)

    Purpose:
       Equality operator. Compare the m_nOffset value of this with the 
       m_nOffset of RHS. Return true if the former is equal to the latter.

    \param const TranslatorPointer<T>& RHS - a reference to a TranslatorPointer
                                             to compare with this
   */
  inline bool operator==(const TranslatorPointer<T>& RHS)
    {
      return (m_rUnderlyingBuffer.template Get<T>(m_nOffset) == 
	      RHS.m_rUnderlyingBuffer.template Get<T>(RHS.m_nOffset));
    }

  /*!
    \fn bool operator>(const TranslatorPointer<T>& RHS)

    Purpose:
       Greater than operator. Compare the m_nOffset value of this with the
       m_nOffset of RHS. Return true if the former is greater than the latter.

    \param const TranslatorPointer<T>& RHS - a reference to a TranslatorPointer
                                             to compare with this
   */
  inline bool operator>(const TranslatorPointer<T>& RHS)
    {
      return (!(*this < RHS) && !(*this == RHS));
    }

  /*!
    bool operator!=(const TranslatorPointer<T>& RHS

    Purpose:
       Inequality operator. Compare the m_nOffset value of this with the
       m_nOffset of RHS. Return true if the former is not equal to the latter.

    \param const TranslatorPointer<T>& RHS - a reference to a TranslatorPointer
                                             to compare with this
   */
  inline bool operator!=(const TranslatorPointer<T>& RHS)
    {
      return !(*this == RHS);      
    }

  // Accessor functions:
 public:
  UInt_t getOffset() const
    {
      return m_nOffset;
    }
  BufferTranslator& getBuffer() const
    {
      return m_rUnderlyingBuffer;
    }
};

#endif
