//  TCLHashTableIterator.h:
//
//    This file defines the CTCLHashTableIterator class.
//    Note that this is a template class where T is the type of thing
//    stored in a hash table entry.  T must have assignment and copy 
//    constructors.
//  In STL terms, this is a forward iterator.  There is no going backwards.
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
/*
 *   Change Log:
 *     $Log$
 *     Revision 4.2  2002/08/08 15:20:16  ron-fox
 *     Fix typo for Tcl_Hashtable -> Tcl_HashTable
 *
 * Revision 1.3  2002/08/08  15:17:15  fox
 * Fix type on Tcl_Hashtable -> Tcl_HashTable
 *
 */
#ifndef __TCLHASHTABLEITERATOR_H  //Required for current class
#define __TCLHASHTABLEITERATOR_H

#ifndef __TCLHASHTABLEITEM_H
#include "TCLHashTableItem.h"
#endif

                               //Required for 1:1 associated classes
#ifndef __TCL_TCL_H
#include <tcl.h>
#define __TCL_TCL_H
#endif

template <class T>
class CTCLHashTableIterator
{
  typedef CTCLHashTableItem<T> CTCLTHashTableItem, *pCTCLTHashTableItem;
  
  Tcl_HashSearch      m_Context; // Search context used by Tcl_NextHashEntry
  pCTCLTHashTableItem m_pCurrentEntry; // Pointer to current entry.
  Tcl_HashTable*      m_pHashTable;
  
public:

			//Constructors with arguments

  CTCLHashTableIterator (Tcl_HashTable*   pTable) :
    m_pCurrentEntry(0),
    m_pHashTable(pTable)
  {
    Initialize();
  }
  virtual ~ CTCLHashTableIterator ( ) { }       //Destructor
	
			//Copy constructor

  CTCLHashTableIterator (const CTCLHashTableIterator& aCTCLHashTableIterator ) 
  {
    m_Context      = aCTCLHashTableIterator.m_Context;
    m_pCurrentEntry = aCTCLHashTableIterator.m_pCurrentEntry;
    m_pHashTable    = aCTCLHashTableIterator.m_pHashTable;
                
  }                                     

			//Operator= Assignment Operator

  CTCLHashTableIterator operator= 
                     (const CTCLHashTableIterator& aCTCLHashTableIterator)
  { 
    if (this == &aCTCLHashTableIterator) return *this;

    m_Context      = aCTCLHashTableIterator.m_Context;
    m_pCurrentEntry = aCTCLHashTableIterator.m_pCurrentEntry;
    m_pHashTable    = aCTCLHashTableIterator.m_pHashTable;
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLHashTableIterator& aCTCLHashTableIterator)
  { return (
	    (m_pCurrentEntry == aCTCLHashTableIterator.m_pCurrentEntry) &&
	    (m_pHashTable    == aCTCLHashTableIterator.m_pHashTable)
	    );
  }         
  // Selectors:

public:

  Tcl_HashSearch getContext() const
  {
    return m_Context;
  }


  CTCLHashTableItem<T>* getCurrentEntry() const
  {
    return m_pCurrentEntry;
  }
  Tcl_HashTable* getHashTable() const
  {
    return m_pHashTable;
  }
  // Mutators... don't use these unless you know exactly what you're doing.
public:
                       //Set accessor function for attribute

  void setContext (Tcl_HashSearch am_Context)
  { 
    m_Context = am_Context;
  }

                       //Set accessor function for attribute

  void setCurrentEntry (CTCLHashTableItem<T>* am_pCurrentEntry)
  { 
    m_pCurrentEntry = am_pCurrentEntry;
  }
                       
                       //Set accessor function for 1:1 association
  void setHashTable (Tcl_HashTable* am_pHashTable)
  { 
    m_pHashTable = am_pHashTable;
  }
  //
  // operations on hash table iterators:
  //
public:
  CTCLHashTableIterator& operator++ () {
    m_pCurrentEntry = Tcl_NextHashEntry(&m_Context);
    return *this;
  }
  CTCLHashTableIterator operator++ (int i) {
    CTCLHashTableIterator<T> copy(*this);
    ++this;
    return copy;
  }

  CTCLHashTableItem<T>& operator* () {
    return *m_pCurrentEntry;
  }
  CTCLHashTableItem<T>* operator->() {
    return m_pCurrentEntry;
  }
  
  
  //  Protected utility functions:
protected:
  void Initialize() {
    m_pCurrentEntry = Tcl_FirstHashEntry(m_pTable, &m_Current);
  }
};

#endif
