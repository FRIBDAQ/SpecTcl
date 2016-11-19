/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

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

/*!
  Change Log:
  $Log$
  Revision 5.3  2006/09/22 11:40:55  ron-fox
  - Cleaned up license text
  - Added support for m2 in SpectrumType_t enum and operator <<>>'s.

  Revision 5.2  2005/06/03 15:19:35  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:28  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:17  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.3.4.1  2004/09/24 11:44:00  ron-fox
  Trust STL String constructors to do the right thing.. what we were doing
  was giving Rational/Purify some fits.

  Revision 4.3  2003/08/25 16:11:56  ron-fox
  Merge with kanayo's code

  Revision 4.2  2003/03/25 12:03:17  ron-fox
  Added Change log comment generated from the CVS $Log$
  Added Change log comment generated from the CVS Revision 5.3  2006/09/22 11:40:55  ron-fox
  Added Change log comment generated from the CVS - Cleaned up license text
  Added Change log comment generated from the CVS - Added support for m2 in SpectrumType_t enum and operator <<>>'s.
  Added Change log comment generated from the CVS
  Added Change log comment generated from the CVS Revision 5.2  2005/06/03 15:19:35  ron-fox
  Added Change log comment generated from the CVS Part of breaking off /merging branch to start 3.1 development
  Added Change log comment generated from the CVS
  Added Change log comment generated from the CVS Revision 5.1.2.1  2004/12/21 17:51:28  ron-fox
  Added Change log comment generated from the CVS Port to gcc 3.x compilers.
  Added Change log comment generated from the CVS
  Added Change log comment generated from the CVS Revision 5.1  2004/11/29 16:56:17  ron-fox
  Added Change log comment generated from the CVS Begin port to 3.x compilers calling this 3.0
  Added Change log comment generated from the CVS
  Added Change log comment generated from the CVS Revision 4.3.4.1  2004/09/24 11:44:00  ron-fox
  Added Change log comment generated from the CVS Trust STL String constructors to do the right thing.. what we were doing
  Added Change log comment generated from the CVS was giving Rational/Purify some fits.
  Added Change log comment generated from the CVS
  Added Change log comment generated from the CVS Revision 4.3  2003/08/25 16:11:56  ron-fox
  Added Change log comment generated from the CVS Merge with kanayo's code
  Added Change log comment generated from the CVS tag.

*/
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
    :
    m_sName(am_sName),
    m_nNumber (am_nNumber) {

  }
        
  virtual ~CNamedItem () {}       //Destructor
  
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
class CMatchNamedItemId {
  UInt_t m_Id;
 public:
  CMatchNamedItemId(UInt_t id) : m_Id(id) {}
  Bool_t operator()(CNamedItem& item) {	// May need to be item itself.
    return (item.getNumber() == m_Id);
  }
};

class CMatchNamedItem {
  std::string Name;
 public:
  CMatchNamedItem(const std::string& rName) : 
  Name(rName)
  {}
  Bool_t operator()(CNamedItem& item) {
    return (item.getName() == Name);
  }
};

#endif
