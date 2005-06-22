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

///////////////////////////////////////////////////////////
//  CProjectionCommand.h
//  Implementation of the Class CProjectionCommand
//  Created on:      20-Jun-2005 10:10:54 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CPROJECTIONCOMMAND_H)
#define      __CPROJECTIONCOMMAND_H

#ifndef __TCLPROCESSOR_H
#include <TCLProcessor.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// Forward class definitions.

class CTCLInterpreter;
class CTCLResult;
class CGateContainer;
class CSpectrum;
class CROI;

/**
 * Executes the project command:
 * 
 * project ?-snapshot? old new x|y ?contour?
 * 
 *  This command creates a projection spectrum from an existing
 *  2-d or 2-d gamma spectrum.  The projection can be in either the
 *  X or Y direction and can optionally be confined to the interior of
 *  a contour or gamma contour.  By default the spectrum created is 'live'
 *  that is it is created in such a way that it will incremenet as new data arrives
 *  continuing to faithfully represent the projection requested.  If the -snapshot
 *  switch is given, the spectrum is wrapped in a snapshot spectrum container which
 *   prevents it from being further incremented.
 *
 * @author Ron Fox
 * @version 1.0
 * @created 20-Jun-2005 10:10:54 AM
 */
class CProjectionCommand : public CTCLProcessor
{
protected:
  typedef enum _direction_ {x, y} direction;

public:
  CProjectionCommand(CTCLInterpreter& rInterp);
  virtual ~CProjectionCommand();
  
  // Outlawed functions:
private:
  CProjectionCommand(const CProjectionCommand& rhs);
  CProjectionCommand& operator=(const CProjectionCommand& rhs);
  int operator==(const CProjectionCommand& rhs) const;
  int operator!=(const CProjectionCommand& rhs) const;

public:
  int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult,
		 int argc, char** argv);
  
protected:
  bool isSnapshotRequest(int& argc, char**& argv);
  CSpectrum* getValidatedSourceSpectrum(const char* name);
  direction  getValidatedDirection(const char* pDirection);
  CSpectrum* getValidatedTargetSpectrum(const char* name, CSpectrum* pSource, 
					direction which);
  CGateContainer* getProjectionGate(const char* pGateName, 
				    CSpectrum* pSourceSpectrum);
  void projectX(CSpectrum* sourceSpectrum, CSpectrum* targetSpectrum, 
		CGateContainer* gate);
  void projectY(CSpectrum* sourceSpectrum, CSpectrum* targetSpectrum, 
		CGateContainer* gate);
  
  STD(string) Usage();

  static CROI* selectROI(CSpectrum* pSource, CGateContainer* pGate);
  static void  parameterOrder(int* orderArray, CSpectrum* pSource, CGateContainer* pGate);
  static void  GateTarget(CSpectrum* pSource, CSpectrum* pTarget, 
			  CGateContainer* pROI);
  static void FoldTarget(CSpectrum* pSource, CSpectrum* pTarget);
};


#endif 
