
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
#define __STL_STRING
#endif

// forward class definitions:

class CModule;
class CTCLInterpreter;
class CTCLResult;
 
/*!
Creator for CAEN digitizers (common format).
*/
class CCAENDigitizerCreator  : public CModuleCreator        
{

public:
	// Constructors and other cannonical operatios.
    CCAENDigitizerCreator ();
    ~ CCAENDigitizerCreator ( );  
    CCAENDigitizerCreator (const CCAENDigitizerCreator& rhs );
    CCAENDigitizerCreator& operator= (const CCAENDigitizerCreator& rhs);
    int operator== (const CCAENDigitizerCreator& rhs) const;
    int operator!= (const CCAENDigitizerCreator& rhs) const {
	return !(operator==(rhs));
    }

public:

    CModule* Create (CTCLInterpreter& rInterp, const string& rName);
    virtual string TypeInfo() const;
};

#endif
