//  CNamedItem.h:
//
//    This file defines the CNamedItem class.
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

#ifndef __NAMEDITEM_H  //Required for current class
#define __NAMEDITEM_H

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __CRT_STRING_H
#include <string.h>
#define __CRT_STRING_H
#endif
                               
class CNamedItem      
{
  std::string m_sName;  // // Name of the item.
  UInt_t m_nNumber;  // Identification number of the item.
  
public:

  
  //Constructor with arguments
  CNamedItem() : m_nNumber(0) 
  {}
  CNamedItem (  std::string am_sName,  UInt_t am_nNumber  )
    :   m_nNumber (am_nNumber) {                      // Some STL string imps 
      m_sName.resize(strlen(am_sName.c_str()) + 2, 0); // Butcher construction.
      m_sName = am_sName;
  }
        
  virtual ~ CNamedItem ( ) { }       //Destructor
  
  //Copy constructor

  CNamedItem (const CNamedItem& aCNamedItem ) 
  {   
    m_sName   = aCNamedItem.m_sName;
    m_nNumber = aCNamedItem.m_nNumber;
  }                                     
  
  //Operator= Assignment Operator

  CNamedItem operator= (const CNamedItem& aCNamedItem)
  { 
    if (this == &aCNamedItem) return *this;          
  
    m_sName = aCNamedItem.m_sName;
    m_nNumber = aCNamedItem.m_nNumber;
  
    return *this;
  }                                     
  
  //Operator== Equality Operator

  int operator== (const CNamedItem& aCNamedItem) const
  { 
    return (	    
	    (m_sName == aCNamedItem.m_sName) &&
	    (m_nNumber == aCNamedItem.m_nNumber) 
	    );
  }
  // Selectors:

public:

  std::string getName() const
  {
    return m_sName;
  }
  UInt_t getNumber() const
  {
    return m_nNumber;
  }
  //
  // Protected mutators:
  //
protected:
  void setName (const std::string& am_sName)
  { 
    m_sName = am_sName;
  }
  void setNumber (UInt_t am_nNumber)
  { 
    m_nNumber = am_nNumber;
  }
 public:
  void ChangeName(const std::string& rNewName) {
    m_sName = rNewName;
  }
};

//  Since named items are often inserted into maps, 
//  The following predicate object is useful for searches on IDs:
//

class CMatchNamedItemId {
  UInt_t m_Id;
public:
  CMatchNamedItemId(UInt_t id) : m_Id(id) {}
  Bool_t operator()(CNamedItem& item) {	// May need to be item itself.
    return (item.getNumber() == m_Id);
}
};
class CMatchNamedItem {
  string Name;
public:
  CMatchNamedItem(const string& rName) : 
    Name(rName)
  {}
  Bool_t operator()(CNamedItem& item) {
    return (item.getName() == Name);
  }
};

#endif
