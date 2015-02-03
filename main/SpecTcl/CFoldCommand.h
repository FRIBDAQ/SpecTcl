///////////////////////////////////////////////////////////
//  CFoldCommand.h
//  Implementation of the Class CFoldCommand
//  Created on:      22-Apr-2005 12:54:15 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CFOLDCOMMAND_H)
#define __CFOLDCOMMAND_H

#ifndef __TCLPROCESSOR_H
#include "TCLProcessor.h"
#endif

#ifndef  __GATECONTAINER_H
#include "GateContainer.h"
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif


// Forward definitions:

class CTCLInterpreter;
class CTCLResult;
class CGateContainer;
class CSpectrum;

/**
 * Implements the fold command.  The fold command has the following forms: 
\verbatim
 fold -apply  gate spectrum ... Applies a gamma gate to a gamma spectrum as a fold.
 fold -list ?pattern?    For all gamma spectra that match pattern (* if omitted),
                         lists the spectra with folds and the folds applied to
                         them in the form of a  list containing elements of 
			 the form {name fold}. 
  fold -remove spectrum  For the
                         gamma spectrum 'spectrum' removes  the fold that is 
			 applied to it.
\endverbatim
 * @created 22-Apr-2005 12:54:15 PM
 * @author Ron Fox
  * @version 1.0
 * @updated 22-Apr-2005 02:40:16 PM
 */
class CFoldCommand : public CTCLProcessor
{
  
public:
  virtual ~CFoldCommand();
  CFoldCommand(CTCLInterpreter* pInterp);
  int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult,
		 int argc, char** argv);
  int applyFold(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		int argc, char** argv);
  int listFolds(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		int argc, char** argv);
  int removeFold(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		 int argc, char** argv);
  Bool_t isGammagate(CGateContainer* pGateContainer);
  
protected:
  std::string Usage();
  Bool_t isGammaSpectrum(CSpectrum* pSpectrum);
  
private:
  /**
   * Container with a T gate.
   */
  static CGateContainer Unfolded;
  
};


#endif
