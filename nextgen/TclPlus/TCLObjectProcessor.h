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

#ifndef __TCLOBJECTPROCESSOR_H
#define __TCLOBJECTPROCESSOR_H

#ifndef __TCLINTERPRETEROBJECT_H
#include <TCLInterpreterObject.h>
#endif

#ifndef __TCLOBJECT_H
#include <TCLObject.h>
#endif

#ifndef __TCL_H
#include <tcl.h>
#ifndef __TCL_H
#define __TCL_H
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __EXCEPTION_H
#include <Exception.h>
#endif

// Forward class definitions:

class  CTCLInterpreter;

/*!
  This class encapsulates the Tcl_Obj command processor interface.
  we needed to start doing this because my understanding is that the argc/argv
  version of the interface is getting deprecated when Tcl/Tk 9.0 comes out.
  If you already  have extensive command sets that use the argc/argv interface,
  fear not, as an adpator has been written to allow these to pretty much work
  once recompiled.

*/
class CTCLObjectProcessor : public CTCLInterpreterObject
{
  // Attributes:

private:
  Tcl_Command m_Token;		// Command token.
  bool        m_fRegistered;	// True if command was registered.
  std::string m_Name;		// Initial name of the command.
public:
  // Constructors and other canonical operations:

  CTCLObjectProcessor(CTCLInterpreter& interp,
		      std::string      name,
		      bool             registerMe=true);
  virtual ~CTCLObjectProcessor();
  // Illegal canonicals:
private:
  CTCLObjectProcessor(const CTCLObjectProcessor& rhs);
  CTCLObjectProcessor& operator=(const CTCLObjectProcessor& rhs);
  int operator==(const CTCLObjectProcessor& rhs) const;
  int operator!=(const CTCLObjectProcessor& rhs) const;
public:

  // class operations:

public:
  void Register();		// Register command on the interpreter.
  void unregister();		// Unregister command from the interp.
  std::string getName() const;	// Return the name of the object.
  Tcl_CmdInfo getInfo() const;	// Return info about the command.

  // Replaceable functionality:0

protected:
  virtual int operator()(CTCLInterpreter& interp,
			 std::vector<CTCLObject>& objv) = 0;
  virtual void onUnregister();

  // Shareable utilities:

protected:
  template<class retType>
  static retType getParameter(CTCLInterpreter& interp,
			      std::vector<CTCLObject>& objv,
			      int which) throw(std::string);
  
  

  // Static callback relays:

private:
  static int commandRelay(ClientData pObject, Tcl_Interp* pInterp,
			  int objc, Tcl_Obj* const* objv);
  static void unregisterRelay(ClientData pObject);


  
};

//// Template function implementations:

/**
 ** Utility function to get a command line item 
 ** throwing an error if that item does not exist.
 ** @param interp - TCL interpreter running the command calling us.
 ** @param objv   - Vector of encapsulated Tcl_Obj*'s that make up the command.
 ** @param which  - Desired item.
 ** @return type requested by caller.
 ** @retval value of that parameter coerced to the requested type.
 ** @throws string - if the parameter can't match.
 **
 */
template<class retType>
retType
CTCLObjectProcessor:: getParameter(CTCLInterpreter& interp,
			     std::vector<CTCLObject>& objv,
			     int which) throw(std::string)
{
  try {
    if (objv.size() <= which) {
      throw std::string("Incorrect number of command line parameters");
    }
    objv[which].Bind(interp);
    retType result = (retType)(objv[which]); // This can throw a CTCLException.
    return result;
  }
  catch (CException &except) {
    std::string msg = except.ReasonText();
    msg += " ";
    msg += except.WasDoing();
    throw msg;
  }
}

#endif
