/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

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

Change Log:
   $Log$
   Revision 5.3  2005/07/07 14:19:45  ron-fox
   Add i64 adjustments from 3.0 branch

   Revision 5.1.2.2  2005/07/07 13:26:19  ron-fox
   Fix another casting issue

   Revision 5.1.2.1  2004/12/21 17:51:18  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:55:56  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.8.6.2  2004/07/08 15:25:52  ron-fox
   Fix some small issues with translator pointer arithmetic I didn't catch at first (operator+ and operator- were wrong).

   Revision 4.8.6.1  2004/07/02 11:31:24  ron-fox
   - Made the offset a byte offset and corrected the code in all members to deal
   with that.

   - Added a new constructor of the form:

       template <class U>
            TranslatorPointer<T>(TranslatorPointer<U>& rhs)

   	 This supports code like:


   	    TranslatorPointer<UShort_t> p1(.... );
   	        ...
   		    TranslatorPointer<ULong_t> pl(p1);

   		    Where now pl will translate longs from the buffer starting at the same offset as
   		    p1.  This simplifies the case where you must pull longword data from the event
   		    buffer.

   Revision 4.8  2003/01/02 17:11:32  venema
   Major version upgrade to 2.0 includes support for arbitrary user coordinate mapping and sticky print options.

   Revision 4.7  2002/08/08 13:16:57  venema
   Fixed counter-intuitive indexing operator to index relative to the current offset, instead of the beginning of the buffer.
   VS: ----------------------------------------------------------------------

   Revision 4.6  2002/08/08 11:48:27  ron-fox
   Add change log.

*/

#ifndef TRANSLATORPOINTER_H
#define TRANSLATORPOINTER_H

#include <histotypes.h>
#include "BufferTranslator.h"
#include <RangeError.h>
#include <stdint.h>

class BufferTranslator;  // forward class declaration

template <class T>
class TranslatorPointer
{
  BufferTranslator& m_rUnderlyingBuffer; /*! Reference to the BufferTrans. */
  ULong_t            m_nOffset;           //!< Byte offset into the buffer.
  
 public:

  // Basic constructor for this receives a reference to a BufferTranslator
  // and defaults m_nOffset to 0
  TranslatorPointer<T>(BufferTranslator& Translator) :
    m_rUnderlyingBuffer(Translator),
    m_nOffset(0) { }

  // Constructor which receives an offset
  TranslatorPointer<T>(BufferTranslator& Translator, const UInt_t& Offset) :
    m_rUnderlyingBuffer(Translator),
    m_nOffset(Offset*sizeof(T)) { }

  // Constructor which receives a BufferTranslator and a address
  // from which to calculate m_nOffset
  TranslatorPointer<T>(BufferTranslator& Translator, Address_t const Addr) :
    m_rUnderlyingBuffer(Translator),
    m_nOffset(((uint8_t*)Addr - (uint8_t*)Translator.getBuffer()))
    { }

  // Constructor from any other type of translator pointer:

  template <class U>
    TranslatorPointer<T>(const TranslatorPointer<U>&RHS) :
    m_rUnderlyingBuffer(RHS.getBuffer()),
    m_nOffset(RHS.getOffset())
    {}

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
      m_nOffset = rhs.getOffset();
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
      T Result;
      m_rUnderlyingBuffer.GetBlock(&Result, sizeof(T), m_nOffset);
      return Result;
    }

  /*!
    \fn T operator[](Int_t nOffset) const

    Purpose:
       Indexing operator returns the value stored at position
       (m_nOffset+nOffset)*sizeof(T) in m_rUnderlyingBuffer.
   */
  inline T operator[](Int_t nOffset) const
    {
      T Result;
      m_rUnderlyingBuffer.GetBlock(&Result, sizeof(T), 
				   m_nOffset + nOffset*sizeof(T));
      return (T)(Result);
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
      m_nOffset+=sizeof(T);
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
      m_nOffset -= sizeof(T);
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
      TranslatorPointer<T>  p(m_rUnderlyingBuffer);
      p.m_nOffset  = m_nOffset + Offset*sizeof(T);
      return p;
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
      TranslatorPointer<T> p(m_rUnderlyingBuffer);
      p.m_nOffset = m_nOffset - Offset*sizeof(T);
      if(Offset >= 0) {
	return p;
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
      m_nOffset += Offset*sizeof(T);
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
      Offset *= sizeof(T);
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
    \fn bool operator<(const TranslatorPointer<T>& RHS)

    Purpose:
       Less than operator. Compare the m_nOffset value of this with the 
       m_nOffset of RHS. Return true if the former is less than the latter.

    \param const TranslatorPointer<T>& RHS - a reference to a TranslatorPointer
                                             to compare with this
   */
  inline bool operator<(const TranslatorPointer<T>& RHS)
    {  
      return m_nOffset < RHS.m_nOffset;

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
      return m_nOffset == RHS.m_nOffset;

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
