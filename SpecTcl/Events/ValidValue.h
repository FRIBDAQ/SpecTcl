/*
		    GNU GENERAL PUBLIC LICENSE
		       Version 2, June 1991

 Copyright (C) 1989, 1991 Free Software Foundation, Inc.
                       59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

			    Preamble

  The licenses for most software are designed to take away your
freedom to share and change it.  By contrast, the GNU General Public
License is intended to guarantee your freedom to share and change free
software--to make sure the software is free for all its users.  This
General Public License applies to most of the Free Software
Foundation's software and to any other program whose authors commit to
using it.  (Some other Free Software Foundation software is covered by
the GNU Library General Public License instead.)  You can apply it to
your programs, too.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
this service if you wish), that you receive source code or can get it
if you want it, that you can change the software or use pieces of it
in new free programs; and that you know you can do these things.

  To protect your rights, we need to make restrictions that forbid
anyone to deny you these rights or to ask you to surrender the rights.
These restrictions translate to certain responsibilities for you if you
distribute c	     END OF TERMS AND CONDITIONS'

*/
//
// ValidValue.h:
//    Provides a class which contains a value and an indicator that the value
//    is valid (modified).
//    In SpecTcl, this is used to represent a parameter value in the CEvent
//    array.
//
#ifndef __VALIDVALUE_H
#define __VALIDVALUE_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

template <class T>
class CValidValue {
private:
  T m_Value;
  Bool_t m_Valid;
public:
  // Constructors and other cannonicals:
  //
  CValidValue() : m_Valid(kfFALSE)
  {
  }
  CValidValue(T& initValue, Bool_t isValid = kfFALSE) : m_Value(initValue),
							m_Valid(isValid)
  {
  }
  CValidValue(const CValidValue& rRHS) : 
    m_Value(rRHS.m_Value),
    m_Valid(rRHS.m_Valid)
  {
  }
  CValidValue& operator=(const CValidValue& rRHS) {
    if(this != &rRHS) {
      m_Value = rRHS.getValue();
      m_Valid = rRHS.m_Valid;
    }
    return *this;
  }
  int operator==(const CValidValue& rRhs) const {
    if((!m_Valid) || (!rRhs.m_Valid)) return 0;	// Both must be valid.
    return (m_Value == rRhs.m_Value);           // and equal.
  }
  int operator!=(const CValidValue& rRhs) const {
    return !operator==(rRhs);
  }
  //
  //  Selectors:
  //
public:
  T getValue() const {
    if(!m_Valid) {
      throw string("Attempted getValue of unset ValidValue object");
    }
    return m_Value;
  }
  Bool_t isValid() const {
    return m_Valid;
  }
  //
  // Mutators:
  //
protected:
  void setValue(const T& rNew) {
    m_Value = rNew;
  }
  void setValid(const Bool_t Valid) {
    m_Valid = Valid;
  }
  // Object operations.
  //
public:
  void clear() { m_Valid = kfFALSE; }
  CValidValue& operator=(const T& rRhs) {
    m_Value = rRhs;
    m_Valid = kfTRUE;
    return *this;
  }
  operator T&() { 
    if(!m_Valid) {
      throw string("Attempted getValue of unset ValidValue object");
    }
    return m_Value; 
  }
};

#endif
