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

#ifndef CPROJECTIONCOMMAND_H
#define CPROJECTIONCOMMAND_H
#include <TCLObjectProcessor.h>
#include <MPITclCommand.h>
#include <string>

// Forward class definitions.

class CTCLInterpreter;
class CTCLObject;
class CGateContainer;
class CSpectrum;
class CROI;

// In mpiSpecTcl, the project command must run in the event sink pipeline rank.
// as it has to have access to the actual spectrum data/dictionary.

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
class CProjectionCommandActual : public CTCLObjectProcessor
{
protected:
  typedef enum _direction_ {x, y} direction;

public:
  CProjectionCommandActual(CTCLInterpreter& rInterp);
  virtual ~CProjectionCommandActual();
  
  // Outlawed functions:
private:
  CProjectionCommandActual(const CProjectionCommandActual& rhs);
  CProjectionCommandActual& operator=(const CProjectionCommandActual& rhs);
  int operator==(const CProjectionCommandActual& rhs) const;
  int operator!=(const CProjectionCommandActual& rhs) const;

public:
  int operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv);
  
protected:
  bool isSnapshotRequest(int& argc, const char**& argv);
  CSpectrum* getValidatedSourceSpectrum(const char* name);
  direction  getValidatedDirection(const char* pDirection);
  CSpectrum* getValidatedTargetSpectrum(const char* name, CSpectrum* pSource, 
					direction which, CGateContainer* pGate);
  CGateContainer* getProjectionGate(const char* pGateName, 
				    CSpectrum* pSourceSpectrum);
  void projectX(CSpectrum* sourceSpectrum, CSpectrum* targetSpectrum, 
		CGateContainer* gate);
  void projectY(CSpectrum* sourceSpectrum, CSpectrum* targetSpectrum, 
		CGateContainer* gate);
  
  std::string Usage();

  static CROI* selectROI(CSpectrum* pSource, CGateContainer* pGate);
  static void  parameterOrder(int* orderArray, CSpectrum* pSource, CGateContainer* pGate);
  static void  GateTarget(CSpectrum* pSource, CSpectrum* pTarget, 
			  CGateContainer* pROI);
  static void FoldTarget(CSpectrum* pSource, CSpectrum* pTarget);
  static CGateContainer* isValid2DmGate(CSpectrum* pSource, CGateContainer* pGate);
};


class CProjectionCommand : public CMPITclCommand {
public:
  CProjectionCommand(CTCLInterpreter& rInterp);
  ~CProjectionCommand() {}
};

#endif 
