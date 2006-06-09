
#include "CCAENDigitizerCreator.h"    				
#include "CModule.h"
#include "CCAENDigitizerModule.h"
#include <TCLInterpreter.h>
#include <TCLResult.h>

/*!
  Construct a digitizer creator for CAEN7xx modules.
*/
CCAENDigitizerCreator::CCAENDigitizerCreator (const string& name,
					      CModuleCommand* pCreator) :
	CModuleCreator(name, pCreator)
{   
    
} 
/*!  Destructor
*/
 CCAENDigitizerCreator::~CCAENDigitizerCreator ( )  //Destructor - Delete dynamic objects
{
}
/*!
 Copy construction
 \param rhs (const CCAENDigitizerCreator& [in]): Reference to the object our construction
 duplicates.
 */
CCAENDigitizerCreator::CCAENDigitizerCreator (const CCAENDigitizerCreator& rhs) 
  : CModuleCreator (rhs) 
 
{

} 
/*!
   Assignment.
   \param rhs (const CCAENDigitizerCreator& [in]): Right hand side of the assignment
   operator.
   \return CCAENDigitizerCreator& reference to *this.
   */
CCAENDigitizerCreator& 
CCAENDigitizerCreator::operator= (const CCAENDigitizerCreator& rhs)
{ 
  if(this != &rhs) {
    CModuleCreator::operator=(rhs);
  }
  return *this;
}

/*!
    Comparison.
    \param rhs (const CCAENDigitizerCreator& [in]): Item we are compared to.
    \return nonzero if equal.
*/
int 
CCAENDigitizerCreator::operator== (const CCAENDigitizerCreator& rhs) const
{
    return CModuleCreator::operator==(rhs);
}

// Functions for class CCAENDigitizerCreator

/*!  

Creates a CAEN digitizer unpacking module.
\param rInterp (CTCLInterpreter& [in]): The interpreter that should be used to
   create the digitizer.
\param rResult (CTCLResult& [out]): Any errors from this creation are reported here.
\param  rName (const string& [in]): Name of the new module (command name too).
\return POinter to the new module or null if could not create.
*/
CSegmentUnpacker* 
CCAENDigitizerCreator::Create(CTCLInterpreter& rInterp, 
				         const string& rName)
{ 
    return new CCAENDigitizerModule(rInterp, rName);
}
/*!
  \return string containing a description of what we create.
*/
string
CCAENDigitizerCreator::TypeInfo() const
{
	return string(" Unpacks common event format for CAEN 7xx digitizers");
}
