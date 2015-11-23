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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
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
#include <config.h>
#include "TCLObject.h"    				
#include "TCLInterpreter.h"
#include "TCLList.h"
#include "TCLException.h"

#include <string.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

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

  return operator=(aCTCLObject.m_pObject);

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

  return operator=(rSource.c_str());
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
  
  NewIfMust();			// Don't overwrite shared obj.
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
  
  NewIfMust();			// Don't ovewrite shared obj.
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
  
  NewIfMust();			// Don't ovewrite shared obj.
  Tcl_SetStringObj(m_pObject, (char*)rList.getList(), -1);
  Bind(rList.getInterpreter());
  
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
 
  NewIfMust();			// Dont' ovewrite shared obj.
  Tcl_SetDoubleObj(m_pObject, dSource);
  return *this;
}
///////////////////////////////////////////////////////////////////////////////
/*!
    Assign to *this from another object.
*/
CTCLObject&
CTCLObject::operator=(Tcl_Obj* rhs)
{

  if (rhs != m_pObject) {	// If we don't check this rhs could get released.
    Tcl_DecrRefCount(m_pObject);
    m_pObject = rhs;
    Tcl_IncrRefCount(m_pObject);
  }
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
		  "CTCLObject::operator int - Conversion to int failed");
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
  // Converts object to a TCLList  

  //

  int len;

  return CTCLList(getInterpreter(), (string)*this);  
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
  
  return operator+=(rItem.c_str());
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
  
  CTCLObject result(Tcl_DuplicateObj(m_pObject));
  result.Bind(*getInterpreter());
  return result;
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
////////////////////////////////////////////////////////////////////////////
/*!
    Get a range of characters from the object.  See Tcl_GetRange
    for the full semantics of this as all we will do is return
    an object constructed from the return value of Tcl_GetRange.

    \param first : int
        First character position to copy.
    \param last   : int 
        Last character position (inclusive) to copy.
  
    \return CTCLObject
    \retval  A CTCLObject that is constructed from the output of
             Tcl_GetRange.

    \note This function will operate on the unicode representation of the string
          according to the Tcl_GetRange manpage.
*/
CTCLObject
CTCLObject::getRange(int first, int last)
{
  CTCLObject result(Tcl_GetRange(m_pObject, first, last));
  result.Bind(getInterpreter());
  return result;
  
}
////////////////////////////////////////////////////////////////////////////
/*!
    Concatente a rhs object that is assumed to be a list into
    *this... see Tcl_ListObjAppendList for more information about this
    function.   Note that if Tcl_ListObjAppendList returns an error,
    this is mapped to a CTCLException of the appropriate type.

    \param rhs  : CTCLObject&
         The list of elements that will be concatted with *this.
         note that to avoid failure, both *this and rhs must be convertible
         to a valid list format.
    \return CTCLObject&
    \retval *this

    \throws  CTCLException if Tcl_ListObjeAppendList returns an error.

    \note *this must be bound to an interpreter.

*/
CTCLObject&
CTCLObject::concat(CTCLObject& rhs)
{
  CTCLInterpreter* pInterp = AssertIfNotBound();
  DupIfMust();			// Don't destroy this if shared.
  int status = Tcl_ListObjAppendList(pInterp->getInterpreter(),
				     m_pObject, rhs.m_pObject);
  if(status != TCL_OK) {
    throw CTCLException(*pInterp, status, 
			"CTCLObject::concat");
  }
  return *this;
}
///////////////////////////////////////////////////////////////////////////
/*!
    Treating *this as a Tcl List, return its elements as a vector
    of objects.  This is a wrapper for 
    Tcl_ListObjGetElements(). 

    \return vector<CTCLObject>
    \retval elements of *this.

    \throws  CTCLException if Tcl_ListObjGetElements returns an error.
    \note    *this must be convertible to a properly formatted Tcl list.
    \note    we require that the object be bound to an interpreter...although
             strictly speaking the Tcl lib does not require it.
*/
vector<CTCLObject>
CTCLObject::getListElements()
{
  int              count;
  Tcl_Obj**        elements;
  CTCLInterpreter* pInterp = AssertIfNotBound();

  int status = Tcl_ListObjGetElements(pInterp->getInterpreter(),
				      m_pObject, &count, &elements);

  if (status != TCL_OK) {
    throw CTCLException(*pInterp, status, "CTCLObject::getListElements");
  }
  vector<CTCLObject> theList;
  for(int i =0; i < count; i++) {
    CTCLObject obj(elements[i]);
    obj.Bind(getInterpreter());
    theList.push_back(obj);
  }
  return theList;
}
/////////////////////////////////////////////////////////////////////////
/*!
   set the object to a list of elements.  This is an object oriented
   wrapper for Tcl_SetListObj  

   \param elements : vector<CTCLObject&>
      The elements that will be used to form this list.

   \return CTCLObject&
   \retval *this
*/
CTCLObject&
CTCLObject::setList(vector<CTCLObject> elements)
{

  int   objc  = elements.size();
  Tcl_Obj** objects = new Tcl_Obj*[objc];
  for (int i =0; i < objc; i++) {
    objects[i] = elements[i].m_pObject;
  }
  Tcl_Obj* newObject = Tcl_NewListObj(objc, objects);

  // Now replace the object we hold with the one we just created.

  Tcl_DecrRefCount(m_pObject);
  Tcl_IncrRefCount(newObject);
  m_pObject = newObject;

  // Kill off our temp storage and...
  delete [] objects;

  // Return ourselves.

  return *this;
}
///////////////////////////////////////////////////////////////////////////
/*!
   Retrieve the length of the list this object represents.

   \return int
   \retval length of list

   \throws  CTCLException if *this cannot be recast as an appropriately
            formatted list.

   \note we require that *this be bound to an interpreter... somewhat
         more restrictive a requirement than Tcl_ListObjectLength.
*/
int
CTCLObject::llength()
{
  CTCLInterpreter* pInterp = AssertIfNotBound();
  int length;
  int status = Tcl_ListObjLength(pInterp->getInterpreter(), m_pObject, &length);
  if (status != TCL_OK) {
    throw CTCLException(*pInterp,
			status, "CTCLObject::llength()");

  }
  return length;
			
}
///////////////////////////////////////////////////////////////////////////
/*!
    Get the element of a list at the specified index.
    If the index is out of range, an empty element is returned.

    \param index : integer
       The element number in the list (numbered from 0).

    \return CTCLObject
    \retval The element returned or an empty object if index is not valid.

    \throws CTCLException if *this cannot be recast as a list.

    \note  We require that *this be bound to an interpreter
*/
CTCLObject
CTCLObject::lindex(int index)
{
  CTCLInterpreter* pInterp = AssertIfNotBound();
  Tcl_Obj*         result(0);

  int status   = Tcl_ListObjIndex(pInterp->getInterpreter(),
				  m_pObject, index, &result);

  // Conversion failed....

  if (status != TCL_OK) {
    throw CTCLException(*pInterp, status, "CTCLObject::lindex");
  }
  // Could be a null object... or not:

  CTCLObject objResult;
  objResult.Bind(pInterp);
  if(result) {
    objResult = result;
  }
  return objResult;
  
}
///////////////////////////////////////////////////////////////////////////
/*!
    Replace a range of elements of this treated as a list with
    another set of objects treated as list elements.
    
    \param  first : int
       First element of the set to replace.
    \param count  : int
       Number to replace.
    \param newElements : vector<CTCLObject&>
       Elements to replace with.
  
    \return CTCLObject&
    \retval *this

    \throws CTCLException  If *this cannot be converted to a valid list format.

    \note  *this must be bound.
*/
CTCLObject&
CTCLObject::lreplace(int first, int count, vector<CTCLObject> newElements)
{
  CTCLInterpreter* pInterp = AssertIfNotBound();
  int              objc    = newElements.size();
  Tcl_Obj**        objv    = new Tcl_Obj*[objc];

  /// Marshall the objv:

  for(int i =0; i < objc; i ++) {
    objv[i] = newElements[i].m_pObject;
  }
  // Do the replace:

  DupIfMust();			// If shared split from original..

  int status = Tcl_ListObjReplace(pInterp->getInterpreter(),
				   m_pObject, 
				   first, count, objc, objv);
  // Kill off temp storage...

  delete []objv;

  // Throw if the status was bad or return *this if it was good:

  if (status != TCL_OK) {
    throw CTCLException(*pInterp, status, "CTCLObject::lreplace");
  }
  

  return *this;
  
}
