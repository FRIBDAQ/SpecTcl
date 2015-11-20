/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _CSPECTRUMDICTIONARY_H
#define _CSPECTRUMDICTIONARY_H

/**
 * @file CSpectrumDictionary.h
 * @brief Defines the spectrum dictionary class.
 */

#ifndef _COBSERVABLE_H
#include "CObservable.h"       	// We implement the observable interface.
#endif


#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

#ifndef _STD_MAP
#include <map>
#ifndef _STD_MAP
#define _STD_MAP
#endif
#endif


#ifndef _STD_EXCEPTION
#include <exception>
#ifndef _STD_EXCEPTION
#define _STD_EXCEPTION
#endif
#endif

#ifndef _STD_STACK
#include <stack>
#ifndef _STD_STACK
#define _STD_STACK
#endif
#endif
/**
 * Forward definitions.
 */
class CSpectrum;

/**
 * @class CSpectrumDictionary
 *
 *   This class contains the spectra 'indexed' by name.  It is also an observable
 *   which fires when spectra are added to or removed from the dictionary.
 */
class CSpectrumDictionary : public CObservable<CSpectrumDictionary>
{
private:

  typedef std::map<std::string, CSpectrum*> Dictionary;
  typedef Dictionary::iterator DictionaryIterator;
public:
  typedef enum _Op {
    Add, Remove
  } Op;

private:
  static CSpectrumDictionary* m_pInstance;

private:
  Dictionary     m_Dictionary;
  std::stack<Op> m_observationReason;

  // Canonicals

private:
  CSpectrumDictionary();
  ~CSpectrumDictionary();

public:
  static CSpectrumDictionary* instance();

  // Object methods.

  void add(CSpectrum& spectrum);
  CSpectrum* find(const char* pName);
  void remove(const char* pName);


  DictionaryIterator begin() {return m_Dictionary.begin();}
  DictionaryIterator end() {return m_Dictionary.end(); }
  size_t size();

  Op getReason();


  // Observer trigger.
private:
  void fireObservers(Op reason);
};

class spectrum_dictionary_exception : public std::exception
{
  std::string m_msg; 
public:
  spectrum_dictionary_exception(std::string msg) : 
    m_msg(msg) {}
  ~spectrum_dictionary_exception() throw() {}
  const char* what() const throw() {
    return m_msg.c_str();
  }
};
#endif
