   

/*
	Implementation file for CPacketCreator for a description of the
	class see CPacketCreator.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include "CPacketCreator.h"    
#include "CPacket.h"				
#include "CModuleDictionary.h"
#include "CModuleCommand.h"
#include <assert.h>

// Static attribute storage and initialization for CPacketCreator

/*!
    Create an object of type CPacketCreator
    \param pModuleCreator (CModuleCommand* [in]):
       The module command processor we use this to get the
       module dictionary.
*/
CPacketCreator::CPacketCreator (const string& rName, 
				CModuleCommand* pModuleCreator) :
  CModuleCreator(rName, pModuleCreator)
{ 
  assert(pModuleCreator);
  m_pModules = pModuleCreator->getDictionary();
  assert(m_pModules);

} 

/*!
    Called to destroy an instance of CPacketCreator
*/
 CPacketCreator::~CPacketCreator ( )
{
}
/*!
   Called to create an instance of CPacketCreator that is a
   functional duplicate of another instance.
   \param rSource (const CPacketCreator& ):
      The object that we will dupliate.
*/
CPacketCreator::CPacketCreator (const CPacketCreator& aCPacketCreator ) 
  : CModuleCreator (aCPacketCreator),
    m_pModules(aCPacketCreator.m_pModules)
 
{

} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.
  \param rhs (const CPacketCreator& rhs ):
     The object that will be functionally copied to *this.
 */
CPacketCreator& CPacketCreator::operator= (const CPacketCreator& rhs)
{ 
  if(this != &rhs) {

    CModuleCreator::operator=(rhs);
    m_pModules = rhs.m_pModules;
  }
  return *this;

}
/*!
  Compare *this for functional equality with another object of
  type CPacketCreator.
  \param rhs (const CPacketCreator& rhs ):
     The object to be compared with *this.

 */
int 
CPacketCreator::operator== (const CPacketCreator& rhs) const
{
  return (CModuleCreator::operator==(rhs)      &&
          (m_pModules == rhs.m_pModules));
}
/*!
   Compare *this for functional inequality with another object
   of type CPacketCreator.  Functional inequality is defined as
   the boolean inverse of functional equality.
   \param rhs (const CPacketCreator& rhs ):
      The object to compare with *this.
*/
int
CPacketCreator::operator!= (const CPacketCreator& rhs) const
{
  return !(*this == rhs);
}

// Functions for class CPacketCreator

/*! 

Description:

Creates a new CPacket module.
The CPacket is new'd into existence.
Our m_pModule member is passed to the
constructor as the dictionary parameter.


\pre none

\post none

Parameters:

\param rInterp (CTCLInterpreter& [in]):
   The interpreter on which the module will be registered.
    Recall that each module created is a new Tcl command.
\param sName (const string& [in]):
  The new module's name.  The new module name also becomes
   the new module's command.



\return CModule*
\retval 0  - Unable to create the new module.
\retval <> 0 - Pointer to the newly created CPacket object.

\throw  

Pseudo code:

\verbatim
return new CPacket(yadayada)
\endverbatim

*/
CSegmentUnpacker* 
CPacketCreator::Create(CTCLInterpreter& rInterp, const string& rName)  
{
  return new CPacket(rName,
		     rInterp, m_pModules);
}  


/*! 

Description:

Returns a string that documents what the unpackers
we create do.

\pre none

\post none

Parameters:

none

\return string
\retval "Unpackes both tagged and untagged packets"

\throw  

Pseudo code:

\verbatim
return string("Unpackes both tagged and untagged packets");
\endverbatim

*/
string 
CPacketCreator::TypeInfo() const  
{ 
  return string("Unpacks both tagged and untagged packets");
}
