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


 
// Class: CTCLObject                     //ANSI C++
//

//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved TCLObject.h
//

#ifndef __TCLOBJECT_H  //Required for current class
#define __TCLOBJECT_H

#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif

#ifndef __TCLLIST_H
#include "TCLList.h"
#endif

#ifndef __TCL_TCL_H
#include <tcl.h>
#ifndef __TCL_TCL_H
#define __TCL_TCL_H
#endif
#endif

#ifndef __CRT_ASSERT_H
#include <assert.h>
#define __CRT_ASSERT_H
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


class CTCLObject : public CTCLInterpreterObject
{                       
private:
			
  Tcl_Obj* m_pObject; //Pointer to Tcl maintained object        


   // Constructors and other cannonical operations:
public:
  //  Default constructor:

  CTCLObject ()    : m_pObject(0) 
  { 
    m_pObject = Tcl_NewObj();
    assert(m_pObject != 0);
    Tcl_IncrRefCount(m_pObject);
    
  } 
  virtual  ~CTCLObject ( )		// Destructor 
  {
    Tcl_DecrRefCount(m_pObject);
  }  
  // Construct for existing C TCL OBject:

  CTCLObject (Tcl_Obj* am_pObject) 
     
  { 
    
     m_pObject = am_pObject; 
     assert(m_pObject != 0);
     Tcl_IncrRefCount(m_pObject); // Don't let caller kill us.
  }      
  
   //Copy constructor Since this is usually used for temps,
   // we share the object with the caller:

  CTCLObject (const CTCLObject& aCTCLObject )  :
    CTCLInterpreterObject(aCTCLObject)
  {
    m_pObject = aCTCLObject.m_pObject;
    Tcl_IncrRefCount(m_pObject); // Will be decr. on destruction.
    Bind(aCTCLObject.getInterpreter());
  }                                     

   // Operator= Assignment Operator 

  CTCLObject& operator= (const CTCLObject& aCTCLObject);
 
   //Operator== Equality Operator  (string comparison).

  int operator== (const CTCLObject& aCTCLObject) const;
	
// Selectors:

public:

  Tcl_Obj* getObject() 		// Don't ensure constness.
  { 
    return m_pObject;
  }
  const Tcl_Obj* getObject() const
  {
    return m_pObject;
  }
                       
// Mutators:

protected:

  void setObject (Tcl_Obj* am_pObject)
  { 
    Tcl_DecrRefCount(m_pObject); // Release prior object.
    m_pObject = am_pObject;
    Tcl_IncrRefCount(m_pObject); // Put the grab on the new one.
  }
       
public:

  // Assignments to this object 

   CTCLObject& operator= (const STD(string)& rSource)    ;
   CTCLObject& operator= (const char* pSource)    ;
   CTCLObject& operator= (int nSource)    ;
   CTCLObject& operator= (const CTCLList& rList)    ;
   CTCLObject& operator= (double dSource)     ;
   CTCLObject&  operator=(Tcl_Obj* rhs);

  // Implcit conversions which access the object's ports.

  operator STD(string) ()    ;
  operator int ()    ;
  operator CTCLList ()    ;
  operator double ()    ;
 

  //  Append operations which append the item to the object
  //  as a list element.

  CTCLObject& operator+= (const CTCLObject& rObject)    ;
  CTCLObject& operator+= (int nItem)    ;
  CTCLObject& operator+= (const STD(string)& rItem)    ;
  CTCLObject& operator+= (const char* pItem)    ;
  CTCLObject& operator+= (double  Item)    ;

  //  Return a true copy of the object using Tcl_DuplicateObj

  CTCLObject clone ()    ;

  // Evaulate the object as a script returning the result STD(string) as
  // an object.  throws TCLException on error.

  CTCLObject operator() ()    ;
 
  // String based functions:

  CTCLObject   getRange(int first, int last);

  // List based functions:


  CTCLObject&  concat(CTCLObject& rhs);	// Concat lists.
  STD(vector)<CTCLObject>  getListElements();
  CTCLObject&  setList(STD(vector)<CTCLObject> elements);
  int          llength();
  CTCLObject   lindex(int index);
  CTCLObject&  lreplace(int first, int count, STD(vector)<CTCLObject> newElements);

protected:
  void DupIfMust() {		// Duplicates object if needed for eval.
    if(Tcl_IsShared(m_pObject)) {
      Tcl_Obj* old = m_pObject;
      m_pObject    = Tcl_DuplicateObj(old);
      Tcl_IncrRefCount(m_pObject);
      Tcl_DecrRefCount(old);
    }
  }
  void NewIfMust() {		// Make new object if needed for replacement.
    if(Tcl_IsShared(m_pObject)) {
      Tcl_Obj* old = m_pObject;
      m_pObject    = Tcl_NewObj();
      Tcl_DecrRefCount(old);
      Tcl_IncrRefCount(m_pObject);
    }
  }
};

#endif













