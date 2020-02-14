#ifndef INTEGRATECOMMAND_H
#define INTEGRATECOMMAND_H

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
#include <TCLObjectProcessor.h>
#include <Point.h>
#include <string>


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
    typedef struct _integration1d {
        double s_sum;
        double s_centroid;
        double s_fwhm;
    } Integration1d, *pIntegration1d;
    typedef struct _integration2d {
        double s_sum;
        double s_xCentroid;
        double s_yCentroid;
        double s_xFwhm;
        double s_yFwhm;
    } Integration2d, *pIntegration2d;
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
 
  static Integration1d integrate1d(CSpectrum* spectrum, std::vector<int> limits);
  static Integration2d integrate2d(CSpectrum* spectrum, std::vector<CPoint> points);
  
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
