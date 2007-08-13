#include "CModuleCreator.h"    
#include "CModule.h"
#include <TCLInterpreter.h>
#include <TCLResult.h>
/*!
	Construct a module creator.
	\param rType (const string& [in]):  Type of module being constructed.
	\param pCreatorCommand (CModuleCommand* [in]):
	    Pointer to the command that creates modules.
	
*/
CModuleCreator::CModuleCreator (const string&   rType,
				CModuleCommand* pCreatorCommand)
  : m_sType(rType),
    m_pCreatorCommand(pCreatorCommand)
{   
    
         //Initialization of array of 1:M association objects to null association objects
    
} 

/*!
   Destructor
*/
CModuleCreator::~CModuleCreator ( )  //Destructor - Delete dynamic objects
{
}

/*!
	Copy constructor.
	\param rhs (const CModuleCreator& [in]): Reference to the object we duplicate
		during construction.
*/
CModuleCreator::CModuleCreator (const CModuleCreator& rhs ) :
	m_sType(rhs.m_sType)
{
	
} 

/*!
	Assignment operator.
	\param rhs (const CModuleCreator& [in]): The right hand side of the assignment.
	\return a reference to *this to allow operator chaining.*/  
CModuleCreator& 
CModuleCreator::operator= (const CModuleCreator& rhs)
{ 
    if(this != &rhs) {
	m_sType = rhs.m_sType;
    }
    return *this;
}

/*!
    Equality comparison operator.
    \param rhs (const CModuleCreator& [in]): object we'll be comparing to.
    \return 0 if not equal, 1 if equal.
*/
int 
CModuleCreator::operator== (const CModuleCreator& rhs) const
{ 
    return (m_sType == rhs.m_sType);
}

// Functions for class CModuleCreator

/*!  

Determines if this creator can create the
specified module.  Default implementation 
is to match the sModuleType parameter 
against m_sType
\param sModuletype (const string& [in]): The module type we are being requested
    to create.
\return true if able to create the module.

*/
bool
CModuleCreator::Match(const string& sModuletype)  
{ 
    return (m_sType == sModuletype);
}  

