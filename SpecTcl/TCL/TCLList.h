/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

//  CTCLList.h:
//
//    This file defines the CTCLList class.
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

#ifndef __TCLLIST_H  //Required for current class
#define __TCLLIST_H
                               //Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif                               

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

typedef STD(vector)<STD(string)> StringArray;
typedef StringArray::iterator StringArrayIterator;

class CTCLList  : public CTCLInterpreterObject        
{
  char* m_pList;  // Pointer to list storage, dynamically allocated
  
public:
			//Default constructor

  CTCLList (CTCLInterpreter* pInterp) :
    CTCLInterpreterObject(pInterp),
    m_pList(0) 
  { }
  ~CTCLList() { delete []m_pList; };       //Destructor

			//Constructors with arguments

  CTCLList (CTCLInterpreter* pInterp, const  char* am_pList  );       
  CTCLList (CTCLInterpreter* pInterp, const STD(string)& rList);
	
			//Copy constructor

  CTCLList (const CTCLList& aCTCLList ) {
    DoAssign(aCTCLList);
  }


			//Operator= Assignment Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  CTCLList& operator= (const CTCLList& aCTCLList) {
    if(this != &aCTCLList) {
      //      CTCLList::operator=(aCTCLList);
      DoAssign(aCTCLList);
    }
    return *this;
  }                                     

			//Operator== Equality Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  int operator== (const CTCLList& aCTCLList);
  int operator!= (const CTCLList& aCTCLList) {
    return !(operator==(aCTCLList));
  }

  // Selectors:

public:
  const char* getList() const
  {
    return m_pList;
  }
  // Mutators:

protected:                
  void setList (const char* am_pList);
  
  // Operations:
  // 
public:
  // Operations which reflect the Tcl_xxx operations on lists.

  int Split (StringArray& rElements)  ;
  int Split (int& argc, char*** argv);

  const char* Merge (const StringArray& rElements)  ;
  const char* Merge(int argc, char** argv);
  //
  // protected utilities:
  //
protected:
  void DoAssign(const CTCLList& rRhs);

};

#endif
