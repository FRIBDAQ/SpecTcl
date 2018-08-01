// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 
/*!
  \class CFitCommand

  Command processor to execute the "fit" command.
  The fit command has the following syntaxes:

\verbatim
   fit ?-create? type name
   fit -list ?pattern?
   fit -delete name
   fit -perform name
   fit -add name point1 ?point2...?
   fit -evaluate name x
\endverbatim

Where
The switches act as subcommands:
- -create Creates a new fit.
- -list   Lists the fits that match the pattern.
- -delete deletes the single named fit.
- -perform performs the fit on the current set of data points.
- -add adds points to the fit.  
- -evaluate evaluates the fit at a given point.

The parameters mean:
- type is the type of a fit to be created e.g. linear
- name is the name to be assigned to a fit and used to identify it in
  future operations.
- pattern is an optional pattern that is used to match the strings.
  pattern may contain glob wildcard characters and defaults to * if
  not supplied.
- pointn are points of the form {x y} where x is an independent parameter
  and y is a dependent parameter e.g. {65 0.557}
- x is an independent parameter for which the fit will be evaluated

Fits are have two states, accepting points and performed.  Fits are 
created in the accepting points state. Once a fit to a set of points
is successfully performed, the fit is in the performed state and can 
be evaluated.  If points are later added to the fit, it returns to the
accepting points state and refuses to be evaluated until a new
perform is done.

At this time there's no way to edit the set of points in a fit except
to delete the fit and re-create it.


*/



#ifndef CFITCOMMAND_H  //Required for current class
#define CFITCOMMAND_H


//
// Include files:
//



#include <TCLProcessor.h>
#include <Point.h>
#include <vector>

                               //Required for 1:1 association classes

// Forward class definitions (convert to includes if required):

class CTCLInterpreter;
class CTCLResult;

class CFitCommand : public CTCLProcessor     
{
public:
    //  Constructors and other canonical operations.
    //  You may need to adjust the parameters
    //  and the visibility esp. if you cannot
    // implement assignment/copy construction.
    // safely.
  CFitCommand (CTCLInterpreter* pInterp); //!< Constructor.
  virtual  ~ CFitCommand ( ); //!< Destructor.
private:
  CFitCommand (const CFitCommand& rSource ); //!< Copy construction.
  CFitCommand& operator= (const CFitCommand& rhs); //!< Assignment.
  int operator== (const CFitCommand& rhs) const; //!< == comparison.
  int operator!= (const CFitCommand& rhs) const; //!< != comparison.
public:
  
  // Class operations:
  
public:
  
  virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			    int argc, char** argv)   ; 

#ifndef _DEBUG			// Test define _DEBUG and these really
    protected:			// should not be externally callable.
#endif
  int Create_parse (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		    int argc, char** argv)   ; 
  int List_parse (CTCLInterpreter& rInterp, CTCLResult& rResult, int 
		  argc, char** argv)   ; 
  int Delete_parse (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		    int argc , char** argv)   ; 
  int Perform_parse (CTCLInterpreter& rInterp, CTCLResult& rResult,
		     int argc, char** argv)   ; 
  int AddPoints_parse (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		       int argc, char** argv)   ; 
  int Evaluate_parse (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		      int argc, char** argv); 
  
protected:
  
  std::string Usage ()   ;		//!< Return the function usage string.

  // Utilities:
private:
  std::vector<FPoint> ParsePoints(CTCLInterpreter& rInterp,
			     int argc, char** argv);
  
};

#endif
