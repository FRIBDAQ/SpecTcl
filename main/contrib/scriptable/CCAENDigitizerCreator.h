/*!
Creator for CAEN digitizers (common format).  For more information about
the unpackig of CAEN 32 channel digitizers, see CCAENDigitizerModule.

This module is registered in a module command object to allow it to recognize
and create unpackers for the 785, 775, 792 and compatibles.
Normally this module is registered to recognize several module types in order
to be compatible with the readout software.

*/


#ifndef __CCAENDIGITIZERCREATOR_H  //Required for current class
#define __CCAENDIGITIZERCREATOR_H

//
// Include files:
//

                               //Required for base classes
#ifndef __CMODULECREATOR_H     //CModuleCreator
#include "CModuleCreator.h"
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// forward class definitions:

class CModule;
class CTCLInterpreter;
class CTCLResult;
class CModuleCommand;


class CCAENDigitizerCreator  : public CModuleCreator        
{

public:
	// Constructors and other cannonical operatios.
    CCAENDigitizerCreator (const std::string& name, CModuleCommand* pCreator);
    ~ CCAENDigitizerCreator ( );  
    CCAENDigitizerCreator (const CCAENDigitizerCreator& rhs );
    CCAENDigitizerCreator& operator= (const CCAENDigitizerCreator& rhs);
    int operator== (const CCAENDigitizerCreator& rhs) const;
    int operator!= (const CCAENDigitizerCreator& rhs) const {
	return !(operator==(rhs));
    }

public:

    virtual  CSegmentUnpacker* Create  (CTCLInterpreter& rInterp, const std::string& rName);
    virtual std::string   TypeInfo() const;
};

#endif
