//  CTCLHashTableItem.h:
//
//    This file defines the CTCLHashTableItem class.
//      T Must have the following defined:
//         Copy Ctor,
//         operator=
//         operator==
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

#ifndef __CTCLHASHTABLEITEM_H  //Required for current class
#define __CTCLHASHTABLEITEM_H

template <class T>		// T Must have copy ctor & Assignment defined.
class CTCLHashTableItem
{
  T m_Item;			// Item to store in the hash table.
  
public:

  CTCLHashTableItem (T am_Item )       
  :   m_Item (am_Item)  { }        
  virtual ~CTCLHashTableItem ( ) { } //Destructor

			//Copy constructor

  CTCLHashTableItem (const CTCLHashTableItem& aCTCLHashTableItem ) :
    m_Item(aTCLHashTableItem.m_Item)
  {
  }                                     

			//Operator= Assignment Operator

  CTCLHashTableItem operator= (const CTCLHashTableItem& aCTCLHashTableItem)
  { 
    if (this == &aCTCLHashTableItem) return *this;          
  
    m_Item = aCTCLHashTableItem.m_Item;        
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLHashTableItem& aCTCLHashTableItem)
  {
    return ((m_Item == aCTCLHashTableItem.m_Item));
  }                             
  // Selectors:
  //
public:

  T getItem() const
  {
    return m_Item;
  }
  // Mutators:

protected:
  void setm_Item (T am_Item)
  {
    m_Item = am_Item;
  }
                       
  T& operator* () 
  { 
    return m_Item; 
  }

  T* operator-> () {
    return &m_Item;
  }
 
};

#endif
