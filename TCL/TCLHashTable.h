//  CTCLHashTable.h:
//
//    This file defines the CTCLHashTable class.
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

#ifndef __TCLHASHTABLE_H  //Required for current class
#define __TCLHASHTABLE_H

#ifndef __TCLHASHTABLEITEM_H
#include "TCLHashTableItem.h"
#endif

#ifndef __TCLHASHTABLEITERATOR_H
#include "TCLHashTableIterator.h"
#endif


#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
          
#ifndef __TCL_TCL_H                     
#include <tcl.h>
#define __TCL_TCL_H
#endif

#ifndef __CRT_ASSERT_H
#include <assert.h>
#define __CRT_ASSERT_H
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

template <class T>
class CTCLHashTable      
{
  typedef CTCLHashTableItem<T> CTCLTHashTableItem,
                             *pCTCLTHashTableItem,
                             &rCTCLTHashTableItem;

  typedef CTCLHashTableIterator<T> CTCLTHashTableIterator,
                                 *pCTCLTHashTableIterator,
                                 &rCTCLTHashTableIterator;

  Tcl_HashTable  m_HashTable;  // Pointer to underlying TCL/TK hashtable.
  
public:
			//Default constructor

  CTCLHashTable () 
  {
    Tcl_InitHashTable(&m_HashTable, TCL_STRING_KEYS);
  } 
  virtual ~CTCLHashTable ( ) {        //Destructor
    Tcl_DeleteHashTable(&m_Table);
  }
			//Constructor with arguments:
                        // Must be string type keys.
  CTCLHashTable (  Tcl_HashTable am_HashTable  )       
  :   m_HashTable (am_HashTable)  
  { }        
	
			//Copy constructor

  CTCLHashTable (const CTCLHashTable& aCTCLHashTable ) 
  {   m_HashTable = aCTCLHashTable.m_HashTable;
                
  }                                     

			//Operator= Assignment Operator

  CTCLHashTable operator= (const CTCLHashTable& aCTCLHashTable)
  { 
    if (this == &aCTCLHashTable) return *this;          
    m_HashTable = aCTCLHashTable.m_HashTable;
        
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator
			//Update to access 1:M associated class attributes      
  int operator== (const CTCLHashTable& aCTCLHashTable)
  { 
    return (
	    (m_HashTable == aCTCLHashTable.m_HashTable) 
	    );
  }           
  // selectors:                  
public:

  Tcl_HashTable* getHashTable() const
  {
    return &m_HashTable;
  }
  // Mutators:

protected:

  void setHashTable (Tcl_HashTable* am_HashTable)
  { 
    m_HashTable = am_HashTable;
  }
  // Operations:
  //
public:                       
  void Enter (const std::string& rKey, rCTCLTHashTableItem rValue) {
    Int_t          nNew;
    Tcl_HashEntry* pEntry = Tcl_CreateHashEntry(&m_Table,
						rKey.c_str(),
						&nNew);
    assert(pEntry != kpNULL);
    Tcl_SetHashValue(pEntry, (ClientData)&rValue);
    
  }
  const CTCLTHashTableItem* Find (const std::string& rsKeyword) const {
    Tcl_HashEntry* pEntry = Tcl_FindHashEntry(&m_Table,
					      rsKeyword.c_str());
    if(pEntry == kpNULL) return (pCTCLTHashTableItem) kpNULL;
    
    return (pCTCLTHashTableItem)(Tcl_GetHashValue(pEntry));
  }
  
  CTCLTHashTableItem* Delete (const std::string& rsKeyword) {
    Tcl_HashEntry* pEntry = Tcl_FindHashEntry(&m_Table,
					      rsKeyword.c_str());
    if(pEntry != kpNULL) {
      pCTCLTHashTableItem pItem = (pCTCLTHashTableItem)
	Tcl_GetHashValue(pEntry);
      Tcl_DeleteHashEntry(pEntry);
      return pItem;
    }
    else {
      return (pCTCLTHashTableItem) kpNULL;
    }
  }
  CTCLTHashTableIterator begin () {
    CTCLTHashTableIterator i(&m_Table);
    return i;
  }
  
  CTCLTHashTableIterator end () {
    CTCLTHashTableIterator i(&m_Table);
    i.SetCurrentEntry((pCTCLTHashTableItem)kpNULL);

    return i;
  }
  
  std::string Statistics () {
    std::string s(Tcl_HashStats(&m_Table));
    return s;
  }
  

};

#endif







