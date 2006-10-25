/*
    Thi software is Copyright by the Board of Trustees of Michigan
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

#ifndef __CFITCOMMAND_H
#define __CFITCOMMAND_H

#ifndef __TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

#ifndef __TCLOBJECT_H
#include <TCLObject.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif



class CFitDictionary;
class CSpectrumFit;
class CTCLInterpreter;


/*!
   Implements the fit command.  The fit command is an ensemble command that
   manages/manipulates SpecTcl fits on spectra.  The fits supported
   are exensible via the CFitFactory, and CFitCreator objects.
   See the sources in the Fits subdirectory of the source tree for more information.
   A default set of fit types are registered, but the user can add 
   additional fit types if desired.  
   Fits are retained in a CFitDictionary.   The CFitDictionary supports
   the registration of Observers according to the observer pattern in the
   GOF book (E. Gamma et. al "Design Patterns  Elements of Reusable
   Object Oriented Software) pg 293-304 in the first edition of that book.

   At this point in time, the CHistogrammer object registers an observer  on the
   dictionary.  CHistogrammer uses its observer to keep fitlines registered
   to Xamine up-to-date so that fits are drawn on appropriate spectra and updated
   as the fits are updated.

   The fits used by this command are encapsulated as CSpectrumFit objects.
   These objects wrap a fit object, associated it with a spectrum, and 
   generate the raw fit points from the spectrum.

   Subcommands in the ensemble and their syntax are:

   - create name spectrum low high type   : creates a fit of the specified
     type on the specified spectrum.  The fit is performed between the 
     low and high channel numbers specified.  Note that these are >raw<
     channel numbers not mapped channel numbers, and that the fit will be
     performed in channel number coordinates.  It is up to other layered code
     to know how to convert mapped channels -> raw channels and 
     how to interpret the fit parameters returned in terms of the mapped spectrum.
   - update ?pattern? : Updates the fits that match the optional pattern.
     If not supplied, the pattern defaults to "*".  Updating a fit means
     deleting the set of points it knows about, re-acquiring the fit points
     from the underlying spectrum and re-performing the fit.
     It is possible that the Fit's spectrum has been 'yanked out from underneath it
     in which case, an error will be signalled by the underlying CSpectrumFit
     object.
   - delete name : Deletes the specified fit, both from the fit dictionary
     and from existence in the world.
   - fit list ?pattern?  Lists the fits that match the optional pattern.
     If the pattern is not provided, * it defaults to *.  The fit list is
     a properly formatted Tcl list where each element is a 5 element sublist:
     - Element 0 is the name of the fit.
     - Element 1 is the name of the spectrum the fit is defined on.
     - Element 2 is the type of fit (e.g. "gaussian").
     - Element 3 is a 2 element sublist of the low, high limits of the fit.
     - Element 4 is a property list of the fit parameters. A property list
       is a list of 2 element sublists.  Each sublist has the name of the property
       and its value.  This permits fits to describe their properties over a large
       range of potential fit types.  All fits are supposed to produce a property
       called chisquare which is the goodness of fit estimate.
    - fit proc name  : for the fit named, returns a Tcl procedure named fitline
      which when called with a raw parameter number returns the height of the fit
      at the specified channel.  Note that the proc is >not< defined, the
      text of the proc is returned instead.  The [eval] command can be used
      to actually define the command... See the example below.
      Note that since all fits return a proc named
      'fitline' the Tcl [rename] command can be used to 'uniquify' the proc name.
      The following example creates procs named after each fit:
\verbatim
      foreach fit [fit list] {
      set name [lindex $fit 0]
      set proc [fit proc $name]
      eval $proc
      rename fitline $name
\endverbatim


*/
class CFitCommand : public CTCLObjectProcessor
{
private:
  CFitDictionary&  m_dictionary;
  static int       m_fitId;
public:
  CFitCommand(CTCLInterpreter& interp,
	      STD(string)      name = STD(string)("fit"));
  virtual ~CFitCommand();

  // Illegal cannonicals:
private:
  CFitCommand(const CFitCommand& rhs);
  CFitCommand& operator=(const CFitCommand& rhs);
  int operator==(const CFitCommand& rhs) const;
  int operator!=(const CFitCommand& rhs) const;
public:

  // Public interface:

  static int id();		// Allocate a fit id.


  // Virtual function overrides.

protected:
  virtual int operator()(CTCLInterpreter& interp,
			 STD(vector)<CTCLObject>& objv);


  // Private subcommand executors.. these are named
  // after their commands.

private:
  int Create(CTCLInterpreter& interp,
	     STD(vector)<CTCLObject>& objv);
  int Update(CTCLInterpreter& interp,
	     STD(vector)<CTCLObject>& objv);
  int Delete(CTCLInterpreter& interp,
	     STD(vector)<CTCLObject>& objv);
  int List(CTCLInterpreter& interp,
	   STD(vector)<CTCLObject>& objv);
  int Proc(CTCLInterpreter& interp,
	   STD(vector)<CTCLObject>& objv);

  CTCLObject describeFit(CTCLInterpreter& interp, CSpectrumFit* pFit);
  STD(string) Usage();
};


#endif
