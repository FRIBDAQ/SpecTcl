#ifndef __INTEGRATECOMMAND_H
#define __INTEGRATECOMMAND_H

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
#include <TCLObjectProcessor.h>
#endif

#ifndef __POINT_H
#include <Point.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


class CTCLInterpreter;
class CGateContainer;
class CSpectrum;

/*!
   This file defines the class interface to a command processor
   that implements the SpecTcl integrate command.  The integrate command
   produces sums, centroids and FWHM under gaussian assumption for a region
   of interest in a spectrum.  A region of interest is defined as being
   either a slice(1d)/contour(2d) that is displayable on the spectrum, 
   or a set of points in 
   spectrum 'mapped' coordinates that define the region of interest.  For
   fundamentally 1-d spectra, the points are just a lower/upper limit pair.
   for fundamentally 2-d spectra, the points are Tcl list of x/y pairs.

  Command summary:

  \verbatim
integrate spectrum     gate
integrate spectrum1d   [list low high]
integrate spectrum2d   [list [list x1 y1] [list x2 y2] ...]
   \endverbatim
  
*/
class CIntegrateCommand : public CTCLObjectProcessor
{
public:

  CIntegrateCommand(CTCLInterpreter& interp);
  virtual ~CIntegrateCommand();
  
  // Disallowed canonicals:
private:
  CIntegrateCommand(const CIntegrateCommand&);
  CIntegrateCommand& operator=(const CIntegrateCommand&);
  int operator==(const CIntegrateCommand&);
  int operator!=(const CIntegrateCommand&);
  
public:
  
  virtual int operator()(CTCLInterpreter& interp,
			STD(vector)<CTCLObject>& objv);
  
protected:
  STD(vector)<CPoint> contourPoints(CGateContainer& gate, CSpectrum& spec);
  STD(vector)<int>   sliceLimits(CGateContainer& gate, CSpectrum& spec);
  
  STD(vector)<int> limitsFromList(CTCLInterpreter& interp, 
				  CTCLObject&      obj,
				  CSpectrum&       spectrum);
  STD(vector)<CPoint> roiFromList(CTCLInterpreter& interp, 
				 CTCLObject&      obj,
				 CSpectrum&       spectrum);
  STD(string) Usage();
  

  int integrate1d(CTCLInterpreter&    interp,
		  CSpectrum&          spectrum,
		  STD(vector)<int> limits);
  int integrate2d(CTCLInterpreter&    interp,
	      CSpectrum&          spectrum,
	      STD(vector)<CPoint> points);
  

};


#endif
