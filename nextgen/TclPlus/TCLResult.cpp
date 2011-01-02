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
//  CTCLResult.cpp
// Encapsulates the result protocol
// as an object bound to a specific TCL interpreter.
// Note that while there can be several result objects
// bound to the same interpreter, the nature of Tcl implies
// that they will all be manipulating the same result.
//
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include <config.h>
#include "TCLResult.h"                               
#include "TCLString.h"
#include "TCLInterpreter.h"
#include <tcl.h>
#include <histotypes.h>
#include <assert.h>


using namespace std;



////////////////////////////////////////////////////////////////////////////
/*!
     Constructs a CTCLResult... Results are always bound
     to an interpreter...therefore our constructor requires an interpreter.
     \param pInterp : CTCLInterpreter*   
        The interpreter to which we will be bound.
     \param reset   : bool (true)
        If true, the interpreter's result is reset, otherwise our contents
        are loaded from the interpreter's current result value.
*/
CTCLResult::CTCLResult(CTCLInterpreter* pInterp, bool reset) :
  CTCLObject()
{
  Bind(pInterp);
  if (reset) {
    Tcl_ResetResult(pInterp->getInterpreter());
  } 
  else {
    *this = Tcl_GetStringResult(pInterp->getInterpreter());
  }
}
///////////////////////////////////////////////////////////////////////////
/*!
    Destruction of a result implies commiting the contents of that
    result to the interpreter:
*/
CTCLResult::~CTCLResult()
{
  commit();
}
////////////////////////////////////////////////////////////////////////////
/*!
   Copy construction: we just need to bind ourselves to the same interpreter
   as the rhs... commit the rhs and load ourselves with the result string.

*/
CTCLResult::CTCLResult(const CTCLResult& rhs) :
  CTCLObject()
{
  rhs.commit();
  Bind(rhs.getInterpreter());
  *this = Tcl_GetStringResult(getInterpreter()->getInterpreter());
}
///////////////////////////////////////////////////////////////////////////
/*!
    Assign to *this from another result:
    - commit the rhs.
    - assign to this from the interpreter string (base class assignment).
*/
CTCLResult&
CTCLResult::operator=(const CTCLResult& rhs) 
{
  if(this != &rhs) {
    rhs.commit();
    *this = Tcl_GetStringResult(rhs.getInterpreter()->getInterpreter());
    Bind(rhs.getInterpreter());	// Be sure we're bound to the same interp.
  }
  return *this;
}
///////////////////////////////////////////////////////////////////////////
/*!
    Assign to self from const char*  this is just base class function.
*/
CTCLResult&
CTCLResult::operator=(const char* rhs) 
{
  CTCLObject::operator=(rhs);
  return *this;
}
////////////////////////////////////////////////////////////////////////////
/*!
    equality - comparisons are always true if the interpreters are the sam
    as there should only be a single underlying result string.
*/
int
CTCLResult::operator==(const CTCLResult& rhs)
{
  return *getInterpreter() == *(rhs.getInterpreter());
}
///////////////////////////////////////////////////////////////////////////
/*!
   inequality is just the logical negation of equality:
*/
int
CTCLResult::operator!=(const CTCLResult& rhs)
{
  return !(*this == rhs);
}
///////////////////////////////////////////////////////////////////////////
/*!
    += unfortunately has already been documented to have different semantics
    than that of the base class.. and we really can't do anything about that
    without breaking a bunch-o-code.  Therefore we just live with it.
    here += implies string append.  For element append, see AppendElement.
    \param pString  : const char*
       the string to add.

    \return CTCLResult&
    \retval *this

*/
CTCLResult&
CTCLResult::operator+=(const char* pString)
{
  Tcl_AppendStringsToObj(getObject(), pString, (char*)NULL);
  return *this;

}
//////////////////////////////////////////////////////////////////////////
/*!
   Clear the result string and the interprter result too.
*/
void
CTCLResult::Clear()
{
  *this = "";
  commit();
}
/////////////////////////////////////////////////////////////////////////
/*!
    Append a string as an element.  This is the base class +=
*/
void
CTCLResult::AppendElement(const char* pString)
{
  CTCLObject::operator+=(pString);
}
/////////////////////////////////////////////////////////////////////////
/*!
    Append a std::string as a list elelment (base class ++).
*/
void
CTCLResult::AppendElement(const string& rstring)
{
  CTCLObject::operator+=(rstring.c_str());
}
////////////////////////////////////////////////////////////////////////
/*!
   Commit this to the result string:
*/
void
CTCLResult::commit() const
{
  CTCLObject result(*this);
  getInterpreter()->setResult(result);

}
//////////////////////////////////////////////////////////////////////////
/*!
   Commit and returnn the string.
*/
string
CTCLResult::getString()
{
  commit();
  return string(*this);
}

