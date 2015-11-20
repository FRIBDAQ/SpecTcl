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
#include <config.h>
#include "CConfigurableObject.h"
#include <CInvalidArgumentException.h>

//////////////////////////////////////////////////////////////////////
//
// Constructors and canonicals:

CConfigurableObject::CConfigurableObject() :
  m_pConfiguration(0),
  m_deleteOnDestroy(false)
{}

/*!
  Copy construction.
*/
CConfigurableObject::CConfigurableObject(const CConfigurableObject& rhs) :
  m_pConfiguration(new CItemConfiguration(*(rhs.m_pConfiguration))),
  m_deleteOnDestroy(true)
{
  
}

CConfigurableObject::~CConfigurableObject()
{
  if (m_deleteOnDestroy) {
    delete m_pConfiguration;
  }
  m_pConfiguration = 0;
}

/*!  

  Objects assign much like a copy construction, however the old configuration
  may need to be deleted.
*/
CConfigurableObject&
CConfigurableObject::operator=(const CConfigurableObject& rhs)
{
  if (this != &rhs) {
    if (m_deleteOnDestroy) {
      delete m_pConfiguration;
    }
    m_pConfiguration = new CItemConfiguration(*(rhs.m_pConfiguration));
    m_deleteOnDestroy = true;
    
  }
  return *this;
}

/*!
   Equality means the configuration is identical:
   
*/
int
CConfigurableObject::operator==(const CConfigurableObject& rhs) const
{
  // If we don't have a configuration yet, we're equal only if the 
  // other object doesn't.

  if (!m_pConfiguration) {
    return !(rhs.m_pConfiguration);
  }
  // If the other side doesn't we're not equal:

  if (!rhs.m_pConfiguration) {
    return false;
  }
  // Now we know that both sides have a configuration:

  return *m_pConfiguration == *(rhs.m_pConfiguration);
}
/*!
  Inequality is simpler.. just negate equality compare:
*/
int
CConfigurableObject::operator!=(const CConfigurableObject& rhs) const
{
  return !(*this == rhs);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Attachment

/*!
  Once constructed, at some point the oustide world attaches a configuration to the
  object. This does that and then invokes the onAttach member function which provides
  concrete subclasses the chance to create the configuration parameters.
  \param pConfiguration - Pointer to the configuration to attach.
  \param dynamic        - If true, pConfiguration point to a dynamcially allocated
                          object that should be deleted in our destructors.

  \note - no assumption is made about only attaching once.

*/
void
CConfigurableObject::Attach(CItemConfiguration* pConfiguration, bool dynamic)
{
  if (m_deleteOnDestroy) {
    delete m_pConfiguration;
  }
  m_pConfiguration  = pConfiguration;
  m_deleteOnDestroy = dynamic;

  onAttach();			// Set up the configuration.
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Proxy the configuration functions to the outside world.
//

/*!
   Configure the value of the defined parameter.  This can throw
   for the following reasons;
   - The delegated function throws,
   - m_pConfiguration is null.
*/
void
CConfigurableObject::configure(std::string name, std::string value)
{
  throwIfNull("CConfigurableObject::configure");
  m_pConfiguration->configure(name, value);
}
///////////////////////////////////////////////////////////////////////////////////////////
//
// Proxy the inquiry function useful to the outside world.
//

/*!
   Get the name of the configuration item which is usually the name of the object.
   \return std::string
   \retval Name of the configuration.
*/
std::string
CConfigurableObject::getName() const
{
  throwIfNull("CConfigurableObject::getName");
  return m_pConfiguration->getName();
}
/*!
  \param name - Name of a configuration parameteter.
  \return string
  \retval Value of that parameter. 

  \throw CInvalidArgumentException
  \throw string - message if the parameter name does not exist.
*/
std::string
CConfigurableObject::cget(std::string name)
{
  throwIfNull("CConfigurableobject::cget(name)");

  return m_pConfiguration->cget(name);
}
/*!
   \return CItemConfiguration::ConfigurationArray 
   \retval the entire configuration for this module.
  
   \throw CInvalidArgumentException if no configuration.
*/
CItemConfiguration::ConfigurationArray
CConfigurableObject::cget()
{
  throwIfNull("ConfugrableObject::cget()");
  return m_pConfiguration->cget();
}

////////////////////////////////////////////////////////////////////////////////////
//
// Utilities:

void
CConfigurableObject::throwIfNull(const char* msg)  const
{
  if (!m_pConfiguration) {
    throw CInvalidArgumentException("NULL", "Non-NULL",
				    msg);
  }
}
