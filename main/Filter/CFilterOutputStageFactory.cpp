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
#include "CFilterOutputStageFactory.h"
#include "CFilterOutputStageCreator.h"
#include "CXdrFilterOutputStageCreator.h"


using namespace std;


// This is the singleton instance of the class.  Since constructors are private, the
// class itself has control over when it is instantiated, and it refuses to instantiate
// unless/until getInstance is called the first time.

CFilterOutputStageFactory* CFilterOutputStageFactory::m_pInstance(0);



/**********************************************************************/
/*!
   The constructor is static in order to enforce the singleton 
   discipline.
*/
CFilterOutputStageFactory::CFilterOutputStageFactory()
{
  registerStockCreators();
}

/******************************************************************/
/*!
    Destructor really should never be called, however I'll implement
    it just in case the user decides that some day....
*/
CFilterOutputStageFactory::~CFilterOutputStageFactory()
{
  for (CreatorIterator i = m_creators.begin(); i != m_creators.end(); i++) {
    CFilterOutputStageCreator* p = *i;
    delete p;
  }
}
/******************************************************************/
/*!
  Returns an instance to the one and only factory.
  \return CFilterOutputStageFactory&
  \retval CFilterOutputStageFactory::m_pInstance which will be non-null by the
          time we return it.
*/
CFilterOutputStageFactory&
CFilterOutputStageFactory::getInstance()
{
  if(!m_pInstance) {
    m_pInstance  = new CFilterOutputStageFactory;
  }
  return *m_pInstance;
}

/*****************************************************************/
/*!
   Create a new filter output stage.
   \param type   - A string identifying the type of output stage
                   we want to create.

  \return CFilterOutputstage*
  \retval NULL - no matching type.
  \retval other - Pointer to an instance of the specific object.  This is
                  dynamically created and the caller must delete it.
*/
CFilterOutputStage*
CFilterOutputStageFactory::create(string type) const
{

  for (ConstCreatorIterator i = m_creators.begin(); i != m_creators.end(); i++) {
    CFilterOutputStageCreator* p            =  *i;
    CFilterOutputStage*        pOutputStage =  (*p)(type);
    if (pOutputStage)  {
      return pOutputStage;
    }
  }

  return reinterpret_cast<CFilterOutputStage*>(0);
}
/*****************************************************************/
/*!
   Register a new creator with the factory.  See the documentation
of the CFilterOutputStageCreator class for information about the 
responsibilities of that class.  Briefly, however, a filter output 
stage creator is supposed to be a generator of filter output stage
objects for a specific type of output stage.

   \param creator  - Reference to a CFilterOutputStageCreator we
                     will add to the list create consults.
                     A clone of this will be inserted so if the
		     caller has dynamically the creator it can be
		     released.

*/
void
CFilterOutputStageFactory::Register(CFilterOutputStageCreator& creator)
{
  CFilterOutputStageCreator* pCreator = creator.clone();
  m_creators.push_back(pCreator);
}
/*******************************************************************/
/*!
   Produce a string that documents the creators.  Each creator has a 
   document member that is supposed to produce a string describing itself.
   These are concatenated separated by newlines and returned.
   \return std::string
   \retval documentation strings of all the creator separated by a \n
*/
string
CFilterOutputStageFactory::document() const
{
  string result;
  ConstCreatorIterator     i = m_creators.begin();
  while(i != m_creators.end()) {
    const CFilterOutputStageCreator* pCreator = *i;
    result += pCreator->document();
    i++;
    if (i != m_creators.end()) {
      result += "\n";
    }
  }
  return result;
}


/********************************************************************************/
/*
** Register the stock output stage creators:
*/
void
CFilterOutputStageFactory::registerStockCreators()
{
  CXdrFilterOutputStageCreator xdr;
  Register(xdr);		// Can do this since there's a clone
}
