//
//  CDictionary.h:
//
//    This file defines the CDictionary class.
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

#ifndef __DICTIONARY_H  //Required for current class
#define __DICTIONARY_H

#ifndef __STL_MAP
#include <map>
#define __STL_MAP
#endif

#ifndef __STL_ALGORITHM
#include <algorithm>
#define __STL_ALGORITHM
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

// Note this is a template class which maintains a dictionary of named 
// objects.  Objects must have copy constructors, and assignment operators.
// The dictionary provides a simplified adaptor interface to the STL MAP
// container, as well as searching mechanisms.



template <class T>
class CDictionary      
{
public:
  typedef map<std::string, T> Dictionary;
  typedef Dictionary::iterator DictionaryIterator;

private:
  Dictionary     m_Map;


public:

  // Constructor

  CDictionary () {} 
  virtual ~CDictionary ( ) { }       //Destructor

  // Copy Constructor:

  CDictionary(const CDictionary& r) :
    m_Map(r.m_Map.begin(), r.m_Map.end())
  {  }

  // Assignment.

  CDictionary operator= (const CDictionary& aCDictionary)
  { 
    if (this == &aCDictionary) return *this;          
    m_Map = aCDictionary.m_Map;
    return *this;
  }

  int operator== (const CDictionary& aCDictionary) const
  { 
    return (m_Map == aCDictionary.m_Map);
  }                            
  //  Operations:
  //
  DictionaryIterator Lookup (const std::string& sName) {
    return m_Map.find(sName);
  }
  template<class Predicate>
  DictionaryIterator FindMatch(Predicate p) {
    return find_if(begin(), end(), p);
  }
  //
  // Note: Enter will overwrite any existing with key sName.
  //
  void Enter (const std::string& sName, const T& Item) {
    m_Map[sName] = Item;
  }
  //
  // 
  void Remove (const std::string& rsName) {
    DictionaryIterator i = m_Map.find(rsName);
    if(i != m_Map.end()) 
      m_Map.erase(i);
//    m_Map.erase(rsName);       // Erase by name kills my g++ compiler!!!
  }

  DictionaryIterator begin () {
    return m_Map.begin();
  }
  DictionaryIterator end () {
    return m_Map.end();
  }
  UInt_t size () {
    return m_Map.size();
  }
};

#endif
