// Class: CTCLObject

//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "TCLObject.h"    				
#include "TCLInterpreter.h"
#include "TCLList.h"
#include "TCLException.h"

#include <string.h>

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved TCLObject.cpp \n";

// Functions for class CTCLObject

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   int operator== (const CTCLObject& aCTCLObject) const
// Operation Type:
//   Comparison.
//
int
CTCLObject::operator==(const CTCLObject& aCTCLObject) const
{
  // Compares this with aCTCObject.  If the string representations
  // of the two objects are the same, returns nonzero.
  int len;
  return (strcmp(Tcl_GetStringFromObj(m_pObject,             &len),
		 Tcl_GetStringFromObj(aCTCLObject.m_pObject, &len)) == 0);
}

//////////////////////////////////////////////////////////////////////////////
//
//  Function:
//     CTCLObject& operator= (const CTCLObject& aCTCLObject)
// Operation Type:
//    Assignment
//
CTCLObject&
CTCLObject::operator= (const CTCLObject& aCTCLObject)
{
  // Assigns This object from the aCTCLObject and returns *this.
  // This assignment is a non sharing one.

  if(this != &aCTCLObject) {
    CTCLInterpreterObject::operator=(aCTCLObject);
    Tcl_DecrRefCount(m_pObject);
    m_pObject = Tcl_DuplicateObj(aCTCLObject.m_pObject);
    Tcl_IncrRefCount(m_pObject);
  }

  return *this;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator=(const string& rSource)
//  Operation Type: 
//     assignment
CTCLObject& CTCLObject::operator=(const string& rSource)  
{
  // Assigns a string to the object.
  //
  // Formal Parameters:
  //    const string& rSource

  DupIfMust();
  Tcl_SetStringObj(m_pObject, (char*)(rSource.c_str()), -1);
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator=(const char* pSource)
//  Operation Type: 
//     assignment
CTCLObject& CTCLObject::operator=(const char* pSource)  
{
  // Assigns a character string (asciz) to
  // the object:
  //
  // Formal Parameters:
  //    const char* pSource
  
  Tcl_SetStringObj(m_pObject, (char*)pSource, -1);
  return *this;

}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator=(int nSource)
//  Operation Type: 
//     Assignment
CTCLObject& CTCLObject::operator=(int nSource)  
{
  // Assigns an int to the object
  //
  // Formal Paramters:
  //    int nSource
  
  DupIfMust();
  Tcl_SetIntObj(m_pObject, nSource);
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator=(const CTCLList& rList)
//  Operation Type: 
//     Assignment
CTCLObject& CTCLObject::operator=(const CTCLList& rList)  
{
  // Assigns a TCL List to the object as a list object.
  //   The list is set to the string value
  //   of the source list.
  //
  // Formal Parameters:
  //     const CTCLList& rList
  
  DupIfMust();
  Tcl_SetStringObj(m_pObject, (char*)rList.getList(), -1);
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator=(double dSource)
//  Operation Type: 
//     Assignment
CTCLObject& CTCLObject::operator=(double dSource)  
{
  // Assigns a double to the object.
 
  DupIfMust();
  Tcl_SetDoubleObj(m_pObject, dSource);
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator string()
//  Operation Type: 
//     Implicit conversion
CTCLObject::operator string()  
{
  // Converts object to string representation.

  int len;
  return string(Tcl_GetStringFromObj(m_pObject, &len));
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator int()
//  Operation Type: 
//     Conversion
CTCLObject::operator int()  
{
  // Converts object to integer representation
  int result;
  CTCLInterpreter* pInterp = AssertIfNotBound();
  int Retval = Tcl_GetIntFromObj(pInterp->getInterpreter(), 
				 m_pObject, &result);
  if(Retval != TCL_OK) {
    throw CTCLException(*pInterp, Retval, 
		  "CTCLObject::oeprator int - Conversion to int failed");
  }
  return result;
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator CTCLList()
//  Operation Type: 
//     Conversion
  CTCLObject::operator CTCLList()  
{
  // Converts object to a TCLList  Note that this object will not
  // be bound to an interpreter.
  //

  int len;

  return CTCLList(0, Tcl_GetStringFromObj(m_pObject, &len));  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     double()
//  Operation Type: 
//     conversion
CTCLObject::operator double()  
{
  // Converts an object to double floating
  // representation.
  //
  double dbl;
  CTCLInterpreter* pInterp = AssertIfNotBound();
  int rval = Tcl_GetDoubleFromObj(pInterp->getInterpreter(), m_pObject, &dbl);
  if(rval != TCL_OK) {
    throw CTCLException(*pInterp, rval,
	"CTCLObject::operator double - failed to parse as dbl.");
  }
  return dbl;
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator+=(const CTCLObject& rObject)
//  Operation Type: 
//     append to list
CTCLObject& CTCLObject::operator+=(const CTCLObject& rObject)  
{
  // Appends a TCL Object to this object as a TclList element.

  DupIfMust();
  CTCLInterpreter* pInterp = AssertIfNotBound();
  Tcl_ListObjAppendElement(pInterp->getInterpreter(),
			   m_pObject, rObject.m_pObject);
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator+=(int nItem)
//  Operation Type: 
//     append
CTCLObject& CTCLObject::operator+=(int nItem)  
{
  // Appends an integer to the object as an element.
  // This is only legal if the TCL Object is bound.
  // otherwise an assertion fails.

  DupIfMust();
  CTCLInterpreter* pInterp = AssertIfNotBound();
  CTCLObject obj;
  obj = nItem;

  Tcl_ListObjAppendElement(pInterp->getInterpreter(),
			   m_pObject, obj.m_pObject);
  return *this;
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator+=(const string& rItem)
//  Operation Type: 
//     Append
CTCLObject& CTCLObject::operator+=(const string& rItem)  
{
  //  Appends a string to this object as a list element.
  
  DupIfMust();
  CTCLInterpreter* pInterp = AssertIfNotBound();
  CTCLObject obj;
  obj = rItem;
  Tcl_ListObjAppendElement(pInterp->getInterpreter(),
			   m_pObject, obj.m_pObject);
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator+=(const char* pItem)
//  Operation Type: 
//     append
CTCLObject& CTCLObject::operator+=(const char* pItem)  
{
  // Append an ASCIZ string to the object
  // as a list element
  
  DupIfMust();
  CTCLInterpreter* pInterp = AssertIfNotBound();
  CTCLObject obj;
  obj = pItem;
  Tcl_ListObjAppendElement(pInterp->getInterpreter(),
			   m_pObject, obj.m_pObject);
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     oeprator+=(double  Item)
//  Operation Type: 
//     Append
CTCLObject& CTCLObject::operator+=(double  Item)  
{
  // Appends a double as a list item.
  //
  DupIfMust();
  CTCLInterpreter* pInterp = AssertIfNotBound();
  CTCLObject obj;
  obj = Item;
  Tcl_ListObjAppendElement(pInterp->getInterpreter(),
			   m_pObject, obj.m_pObject);
  return *this;  
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     clone()
//  Operation Type: 
//     duplicator
CTCLObject CTCLObject::clone()  
{
  // Produces a reference to a termporary object
  // which is a duplicate of this, but is not shared.
  
  return CTCLObject(Tcl_DuplicateObj(m_pObject));
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()()
//  Operation Type: 
//     Evaluator
CTCLObject CTCLObject::operator()()  
{
  // Evaluates this object as a script.
  // On success returns the result.
  // on failure throws a CTCLException
  //
  // Returns the result object.(unbound)
  
  CTCLInterpreter* pInterp = AssertIfNotBound();
  Tcl_Interp* pInt = pInterp->getInterpreter();
  int Status;
  if((Status = Tcl_GlobalEvalObj(pInt, m_pObject)) != TCL_OK) {
    CTCLObject result(Tcl_GetObjResult(pInt));
    throw CTCLException(*pInterp, Status,
	   result);
  }
  else {
    return CTCLObject(Tcl_GetObjResult(pInt));
  }
}














