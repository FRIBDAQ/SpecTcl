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

#include <config.h>
#include "TCLProcessor.h"
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <TCLCompatibilityProcessor.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


///////////////////////////////////////////////////////////////////////////////////
/*!
     Construct using a string  command name and pointer to the interpreter.
     To do this we construct the base class,  with the interpreter,
     construct the string, and then new into existence a compatibility
     processor that will take care of everything.

     \param sCommand : string
        Initial name of the command. Note that this can be renamed by scripts
        and at this level, we won't know about it.
     \param pInterp  : CTCLInterpreter*
        The interpreter on which this command gets registered via Register or
        RegisterAll calls (they are the same in this implementation).
*/
CTCLProcessor::CTCLProcessor(string sCommand, CTCLInterpreter* pInterp) :
  CTCLInterpreterObject(pInterp),
  m_Command(sCommand),
  m_pObjectProcessor(0)
{
  m_pObjectProcessor = new CTCLCompatibilityProcessor(*this);
}
//////////////////////////////////////////////////////////////////////////////////
/*!
   Construct from the char* version of the command name.
*/
CTCLProcessor::CTCLProcessor(const char* pCommand, CTCLInterpreter* pInterp) :
  CTCLInterpreterObject(pInterp),
  m_Command(pCommand),
  m_pObjectProcessor(0)
{
  m_pObjectProcessor = new CTCLCompatibilityProcessor(*this);
}
///////////////////////////////////////////////////////////////////////////////////
/*!
     Destruction requires unregistration, if possible, and 
     deletion of the compatibility processor.
*/
CTCLProcessor::~CTCLProcessor()
{
  UnregisterAll();
  delete m_pObjectProcessor;
}
///////////////////////////////////////////////////////////////////////////////////
/*!
  Return the name of the command. This is the initial command name, not the
  name after any renaming e.g.
*/
string
CTCLProcessor::getCommandName() const
{
  return m_Command;
}
//////////////////////////////////////////////////////////////////////////////////
/*!
   Called when the command is being deleted.  The default action is to do nothing.
*/
void
CTCLProcessor::OnDelete()
{
}
//////////////////////////////////////////////////////////////////////////////////
/*!
  Register the object with the interpreter.  We allow multiple registrations
  (subsequent are no-ops).
*/
void
CTCLProcessor::Register()
{
  try {
    m_pObjectProcessor->Register();
  } 
  catch (...) {
    // Failure is allowed and ignored.
  }
}

////////////////////////////////////////////////////////////////////////////////
/*!
    Register all is the same as register.
*/
void
CTCLProcessor::RegisterAll()
{
  Register();
}
///////////////////////////////////////////////////////////////////////////////
/*!
    Unregister all just does a caught unregister on the adaptor.
*/
void
CTCLProcessor::UnregisterAll()
{
  try {
    m_pObjectProcessor->unregister();
  }
  catch (...) {
    //  Failures are allowed here.
  }
}
void 
CTCLProcessor::Unregister()
{
  UnregisterAll();
}
//////////////////////////////////////////////////////////////////////////
/*!
 Concatenates a parameter set into a single string.
 This can be  used by command interpreters which
  will need to further parse their input (e.g an
  epxression evaluator may want to act globally on the
  expression, rather than a token at a time).  Spaces are put
  between each parameter.


  \param  nArguments:
            Number of parameters to concatenate.
   \param pArguments[] : char*
             Pointer to the set of parameters to concatenate.
   \return string
   \retval String containing the concatenation of the parameters.
   \note
     Command invocations set argv[0] to be the command text.  In order
     to be policy free, we act as if that's not the case, therefore if the
     client really only wants the parameters, they should pass a pointer
     to argv[1]
*/
std::string 
CTCLProcessor::ConcatenateParameters(int nArguments, char* pArguments[]) 
{

  std::string Result;
  std::string leader="";
  for(int i = 0; i < nArguments; i ++) {
    Result += leader;
    Result += pArguments[i];
    leader = " ";		// From now on lead with a space.
  }
  return Result;
}
//////////////////////////////////////////////////////////////////////////
/*!
 Uses the currently bound interpreter to parse
 a parameter as an integer.
 
      \param  pString : char*
            Pointer to the string to parse as an integer.
      \param pInteger : int*
           Pointer to the integer to fill in.
      \return int
      \retval   TCL_OK if ok, 
      \retval  TCL_ERROR If not:
               client should instantiate a 
               CTCLResult object on the bound interpreter to get detailed error
               information.
*/
int 
CTCLProcessor::ParseInt(const char* pString, int* pInteger) 
{


  // Ensure that we're bound to an interpreter:
  // If not return TCL_ERROR, but we can't set a result code since that 
  // depends on an interpreter.
  //

  CTCLInterpreter* pInterpObj = getInterpreter();
  if(pInterpObj == (CTCLInterpreter*)kpNULL)
    return TCL_ERROR;		// Not bound...
  Tcl_Interp*      pInterp    = pInterpObj->getInterpreter();
  if(pInterp == (Tcl_Interp*)kpNULL)
    return TCL_ERROR;

  return Tcl_GetInt(pInterp, (char*)pString, pInteger);

}
//////////////////////////////////////////////////////////////////////////
/*!
  Parses a string as a double using the currently
  bound interpreter.
 
     \param pString: char*
             String to parse.
     \param pDouble: double*
           Points to where to put value to parse.
     \return
     \retval  TCL_OK - If ok.
     \retval TCL_ERROR and a CTCLResult
             can be instantiated on the bound interpreter to 
             retrieve a textual reason for the parse failure.

 */
int 
CTCLProcessor::ParseDouble(const char* pString, double* pDouble) 
{

  CTCLInterpreter* pInterpObj = getInterpreter();
  if(pInterpObj == (CTCLInterpreter*)kpNULL)
    return TCL_ERROR;		// Not bound...
  Tcl_Interp*      pInterp    = pInterpObj->getInterpreter();
  if(pInterp == (Tcl_Interp*)kpNULL)
    return TCL_ERROR;

  return Tcl_GetDouble(pInterp, (char*)pString, pDouble);

}
////////////////////////////////////////////////////////////////////////////////
/*!
  Parses a string as a boolean.
  True values are any of: 1,true,on, or yes
  False values are any of 0, false, off, or no.

  \param  pString: const char*
           Pointer to the string to parse.
  \param  pBool: Bool_t*
           Pointer to the result boolean.
  \return int
  \retval   TCL_OK if parse worked.
  \retval  TCL_ERROR and instantiating a CTCLResult object on the
           currently bound interpreter will enable a textual
           reason for the parse failure to be retrieved.
 */
int 
CTCLProcessor::ParseBoolean(const char* pString, Bool_t* pBoolean) 
{


  CTCLInterpreter* pInterpObj = getInterpreter();
  if(pInterpObj == (CTCLInterpreter*)kpNULL)
    return TCL_ERROR;		// Not bound...
  Tcl_Interp*      pInterp    = pInterpObj->getInterpreter();
  if(pInterp == (Tcl_Interp*)kpNULL)
    return TCL_ERROR;

  int flag;
  int nStatus = Tcl_GetBoolean(pInterp, (char*)pString, &flag);
  *pBoolean = flag;
  return nStatus;
}
////////////////////////////////////////////////////////////////////////////
/*!
   Match a keyword, rValue, against the set of keywords in a MatchTable.
   If no match can be found, the NoMatch value is returned.
   Otherwise the index into the table is returned.
   \param MatchTable : vector<string>&
        vector of legitimate keyword strings.
   \param rValue : const string&
        The keyword to match against the table.
   \param NoMatch : int
        Value to return if no match is found.

   \return int
   \retval the index into MatchTable where a string matched rValue.
   \retval NoMatch if there is no such index.
  
 */
int
CTCLProcessor::MatchKeyword(vector<string>& MatchTable, 
			    const string& rValue, 
			    int NoMatch)
{

  int index = 0;
  vector<string>::iterator i;
  for(i = MatchTable.begin(); i != MatchTable.end(); i++) {
    if(*i == rValue) return index;
    index++;
  }
  return NoMatch;
}
////////////////////////////////////////////////////////////////////////
/*!
   This is a hook that can be overridden by derived classes.
   It is called just prior to the execution of operator().
*/
void
CTCLProcessor::preCommand()
{
}
/////////////////////////////////////////////////////////////////////////
/*!
   This is a hook that can be overiddent by derived classes.
   It is called just after the execution of operator()... success
   or failure.
*/
void
CTCLProcessor::postCommand()
{
}
///////////////////////////////////////////////////////////////
/*!
   Hook for derived classes to get control just prior to OnDelete.
*/
void
CTCLProcessor::preDelete()
{
}
///////////////////////////////////////////////////////////////
/*!
   Hook for derived classes to get control just after OnDelete
*/
void
CTCLProcessor::postDelete()
{
}
/////////////////////////////////////////////////////////////
/*!  Binding must also rebind the adaptor.
 */
CTCLInterpreter*
CTCLProcessor::Bind(CTCLInterpreter* binding)
{
  m_pObjectProcessor->Bind(binding); // Bind the adaptor.
  return   CTCLInterpreterObject::Bind(binding);	// Bind ourselves...

}
CTCLInterpreter*
CTCLProcessor::Bind(CTCLInterpreter& binding)
{
  return Bind(&binding);
}
