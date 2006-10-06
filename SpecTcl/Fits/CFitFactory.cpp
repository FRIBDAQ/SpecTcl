   

/*
	Implementation file for CFitFactory for a description of the
	class see CFitFactory.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>

#include "CFitFactory.h"    				
#include "CFit.h"
#include "CFitCreator.h"

#include <DesignByContract.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

using namespace DesignByContract;

// Static attribute storage and initialization for CFitFactory

// m_mapCreators is a <string, CFitCreator*> map that
// contains creators for all the fit types we support.
// 
CFitFactory::FitCreatorMap CFitFactory::m_mapCreators;



/*! 

Description:

Adds a new fit creator to m_mapFitTypes.  
Fit creators associate a fit type (e.g. "linear") with
a creational object (CLinearFit) 
for fits of that type (e.g. CLinearFit).  This 
mechanism allows the set of fit types to be 
extensible by both the SpecTcl programming team
and user application programmers.



\pre   A fit creator by this name does not yet exist.
\post  The map shoud contain our new creator.

\param sType  (const string& [in])
    The name of the fit type (e.g. "linear").

\param pCreator (CFitCreator* [in]):
    Pointer to the fit creational object.


\throw  DesignByContract::PreConditionException


*/
void 
CFitFactory::AddFitType(const string & rType, CFitCreator* pCreator)  
{
  // Check preconditions
  REQUIRE((FindFitCreator(rType) == endCreators()),
	  "This fit creator already exists.");

  // Now make the entry in the map:

  m_mapCreators[rType] = pCreator;

  CHECK((FindFitCreator(rType) != endCreators()),
	"Could not find map already entered");
}  

/*! 


Creates a new fit. 
 

Parameters:

\param sFitType (string)
   The type of fit to be created e.g. "linear"
\param sFitName (string)
   The name of the fit to be created e.g. "calibration1"


\return CFit*
\return CFit*
\retval NULL    - Indicates the fit could not be created.
                          this is usually because the fit type is illegal.
\retval !NULL  - Pointer to the created fit.

*/
CFit* 
CFitFactory::Create(string sFitType, string sFitName)  
{


  // Locate the creator..

  FitCreatorIterator i = FindFitCreator(sFitType);
  if(i == endCreators()) {	// no creator.
    return (CFit*)NULL;
  }
  CFitCreator* pCreator = i->second;
  CFit* pFit = (*pCreator)();
  CHECK(pFit, "Null Fit created");


  return pFit;
}  

/*!
  Return an iterator to the begining of the fit creator map.

  \return FitCreatorIterator

 */
CFitFactory::FitCreatorIterator 
CFitFactory::beginCreators()
{
  return m_mapCreators.begin();
}
/*!
  Return an end of iteration iterator to the fit creator map.
*/
CFitFactory::FitCreatorIterator 
CFitFactory::endCreators()
{
  return m_mapCreators.end();
}
/*!
  Return the number of fit creators that are defined.
*/
int    
CFitFactory::numberOfCreators()
{
  return m_mapCreators.size();
}


/*! 


Locates the fit creator associated with a specific type.


\param sType (string)
   Type of fit creator  to locate.

\return FitCreatorIterator
\retval endCreators()    - That type of fit does not exist.
\retval !endCreators()   - Pointer to the fit .


*/
CFitFactory::FitCreatorIterator
CFitFactory::FindFitCreator(string sType)  
{
  // This assumes the fit creators are in a map.

  FitCreatorIterator pFitCreator = m_mapCreators.find(sType);
  return pFitCreator;
}  



