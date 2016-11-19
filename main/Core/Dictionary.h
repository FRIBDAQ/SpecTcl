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
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif

#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
#endif

#ifndef __STL_ALGORITHM
#include <algorithm>
#ifndef __STL_ALGORITHM
#define __STL_ALGORITHM
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

// Template class for observers of dictionaries:

template <class T>
class DictionaryObserver 
{
public:
  virtual void onAdd(std::string   name,   T& item) {}
  virtual void onRemove( std::string name,    T& item) {}

};

// Template class for managing observer lists for Dictionaries.
//
//
template <class T>
class CDictionaryObserverManager 
{
  // Need to do a bit of dirty work in due to 
  // stl and C++'s template limitations:

  typedef std::list<void*>        ObserverList;
  typedef ObserverList::iterator  ObserverIterator;

  ObserverList      m_observers;

public:
  void addObserver(DictionaryObserver<T>* observer) {
    m_observers.push_back(observer);
  }
  void removeObserver(DictionaryObserver<T>* observer) {
    ObserverIterator i = m_observers.begin();
    while(i != m_observers.end()) {
      if (*i == (void*)observer) {
	m_observers.erase(i);
	return;
      }
      i++;
    }
  }
  void invokeAdd(const std::string name, T& item) {
    ObserverIterator i = m_observers.begin();
    while (i  != m_observers.end()) {
      DictionaryObserver<T>* obs = static_cast<DictionaryObserver<T>*>(*i);
      obs->onAdd(name, item);
      i++;
    }
  }
  void invokeRemove(const std::string name, T& item) {
    ObserverIterator i = m_observers.begin();
    while (i  != m_observers.end()) {
      DictionaryObserver<T>* obs = static_cast<DictionaryObserver<T>*>(*i);
      obs->onRemove(name, item);
      i++;
    }  
  }


};

// Note this is a template class which maintains a dictionary of named 
// objects.  Objects must have copy constructors, and assignment operators.
// The dictionary provides a simplified adaptor interface to the STL MAP
// container, as well as searching mechanisms.
//
template <class T>
class CDictionary      
{
 public:
  typedef  std::map<std::string,  T> Dictionary;
  typedef typename  Dictionary::iterator DictionaryIterator;
  typedef typename  Dictionary::const_iterator ConstDictionaryIterator;

 private:
  Dictionary     m_Map;
  CDictionaryObserverManager<T>   m_observers;

 public:
  // Constructors
  CDictionary() {}
  virtual ~CDictionary() {}

  // Copy Constructor:
  CDictionary(const CDictionary& r) : m_Map(r.m_Map.begin(), r.m_Map.end()) {}

  // Assignment.
  CDictionary operator=(const CDictionary& aCDictionary) {
    if (this == &aCDictionary) return *this;
    m_Map = aCDictionary.m_Map;
    return *this;
  }

  int operator==(const CDictionary& aCDictionary) const {
    return (m_Map == aCDictionary.m_Map);
  }

  // Operations:
  DictionaryIterator Lookup(std::string sName) {
    return m_Map.find(sName);
  }

  ConstDictionaryIterator Lookup(std::string sName) const {
    return m_Map.find(sName);
  }

  template<class Predicate>
    DictionaryIterator FindMatch(Predicate p) {
    return find_if(m_Map.begin(), m_Map.end(), p);
  }

    template<class Predicate>
    ConstDictionaryIterator FindMatch(Predicate p) const {
        return find_if(m_Map.begin(), m_Map.end(), p);
    }

  // Note: Enter will overwrite any existing with key sName.
  void Enter(std::string sName, T& Item) {
    m_Map[sName] = Item;
    m_observers.invokeAdd(sName, Item);
  }

  void Remove(std::string rsName) {
    DictionaryIterator i = m_Map.find(rsName);
    if(i != m_Map.end()) 
      m_observers.invokeRemove(rsName, i->second);
      m_Map.erase(i);
    // m_Map.erase(rsName); // Erase by name kills my g++ compiler!!!
  }

  DictionaryIterator begin() {
    return m_Map.begin();
  }

  ConstDictionaryIterator begin() const {
    return m_Map.begin();
  }

  DictionaryIterator end() {
    return m_Map.end();
  }

  ConstDictionaryIterator end() const {
    return m_Map.end();
  }

  UInt_t size() const {
    return m_Map.size();
  }
  void addObserver(DictionaryObserver<T>* observer) {
    m_observers.addObserver(observer);
  }
  void removeObserver(DictionaryObserver<T>* observer) {
    m_observers.removeObserver(observer);
  }
};

#endif
