///////////////////////////////////////////////////////////
//  CFoldCommand.h
//  Implementation of the Class CFoldCommand
//  Created on:      22-Apr-2005 12:54:15 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#ifndef CFOLDCOMMAND_H
#define CFOLDCOMMAND_H

#include "TCLObjectProcessor.h"
#include <MPITclCommand.h>
#include "GateContainer.h"
#include <histotypes.h>


// Forward definitions:

class CTCLInterpreter;
class CTCLObject;
class CGateContainer;
class CSpectrum;

// In mpiSpecTcl fold commands must be executed in the event sink thread as they
// modify how spectra are incremented by the histogramer.  We therefore encapsulate 
// them in a CMPITclCommand and within that command ensure the histogramer can be 
// gotten in order to do something.  

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
class CFoldCommandActual : public CTCLObjectProcessor
{
  
public:
  virtual ~CFoldCommandActual();
  CFoldCommandActual(CTCLInterpreter* pInterp);
  int operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv);
  int applyFold(CTCLInterpreter& rInterp,int argc, const char** argv);
  int listFolds(CTCLInterpreter& rInterp, int argc, const char** argv);
  int removeFold(CTCLInterpreter& rInterp, int argc, const char** argv);
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


// The command that will be registered is a container for CFoldCommandActual:

class CFoldCommand : public CMPITclCommand {
public:
  CFoldCommand(CTCLInterpreter* pInterp);
  ~CFoldCommand() {}
};
#endif
