   

/*
	Implementation file for CCalibFitFactory for a description of the
	class see CCalibFitFactory.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>

#include "./CFitFactory.h"    				
#include "./CFit.h"
#include "./CFitCreator.h"

#include <DesignByContract.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

using namespace DesignByContract;

// Static attribute storage and initialization for CCalibFitFactory

// m_mapCreators is a <string, CCalibFitCreator*> map that
// contains creators for all the fit types we support.
// 
CCalibFitFactory::FitCreatorMap CCalibFitFactory::m_mapCreators;

// m_mapDefinedFits is a map of all the fits that
// have been created by this factory and are still
// in existence.

CCalibFitFactory::FitMap        CCalibFitFactory::m_mapDefinedFits;



/*! 

Description:

Adds a new fit creator to m_mapFitTypes.  
Fit creators associate a fit type (e.g. "linear") with
a creational object (CCalibLinearFit) 
for fits of that type (e.g. CCalibLinearFit).  This 
mechanism allows the set of fit types to be 
extensible by both the SpecTcl programming team
and user application programmers.



\pre   A fit creator by this name does not yet exist.
\post  The map shoud contain our new creator.

\param sType  (const string& [in])
    The name of the fit type (e.g. "linear").

\param pCreator (CCalibFitCreator* [in]):
    Pointer to the fit creational object.


\throw  DesignByContract::PreConditionException


*/
void 
CCalibFitFactory::AddFitType(const string & rType, CCalibFitCreator* pCreator)  
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
 
\pre  The fit does not yet exist.
\post The fit is enteredin the map
\post The fit in the map is not null.

Parameters:

\param sFitType (string)
   The type of fit to be created e.g. "linear"
\param sFitName (string)
   The name of the fit to be created e.g. "calibration1"


\return CCalibFit*
\return CCalibFit*
\retval NULL    - Indicates the fit could not be created.
                          this is usually because the fit type is illegal.
\retval !NULL  - Pointer to the created fit.

*/
CCalibFit* 
CCalibFitFactory::Create(string sFitType, string sFitName)  
{

  // Check precondition:

  REQUIRE(FindFit(sFitName) == end(),
	"Fit already exists");

  // Locate the creator..

  FitCreatorIterator i = FindFitCreator(sFitType);
  if(i == endCreators()) {	// no creator.
    return (CCalibFit*)NULL;
  }
  CCalibFitCreator* pCreator = i->second;
  CCalibFit* pFit = (*pCreator)();
  CHECK(pFit, "Null Fit created");

  m_mapDefinedFits[sFitName] = pFit;
  
  // Check postconditions.
  
  CHECK(FindFit(sFitName) != end(),
	"Fit is inserted but is not  be found");
  CHECK(m_mapDefinedFits[sFitName],
	"Fit is entered with a null pointer!");

  return pFit;
}  


/*! 

Description:

Deletes a specific fit from the list of existing fits.  


Parameters:

\param sName (string)
   Name of the fit to delete.

\return bool
\return bool - success info
\retval false - The fit does not exist and therefore could not be
    deleted.
\retval true - fit was found and deleted normally.


*/
bool 
CCalibFitFactory::Delete(string sName)  
{


  FitIterator i = FindFit(sName);
  if(i != end()) {
    delete i->second;
    m_mapDefinedFits.erase(sName);
    return true;
  }
  else {
    return false;
  }
}  

/*! 

Description:

Performs the requested fit.



Parameters:
\param name (string)
   Name of the fit to perform

\return bool
\retval true   - Fit succeeded.
\retval false - Fit failed.


*/
bool
CCalibFitFactory::Perform(string sName)  
{ 
  // The fit can only be performed if it can be found:

  FitIterator pFitPair = FindFit(sName);
  if(pFitPair != end()) {
    try {
      CCalibFit* p = pFitPair->second;
      CHECK(p, "Null fit pointer");
      p->Perform();
      return true;
    }
    catch (...) {
      return false;
    }
  }
  else {
    return false;
  }
}  

/*! 


Add some points to the fit data.


\param name (string)
   Name of fit to add points to.
\param points (vector<FPoint >)  
   The points to add (x,y) pairs.


\return bool
\retval true - Fit worked
\retval false- fit failed.
*/
bool
CCalibFitFactory::AddPoints(string sName,  vector<FPoint> vPoints)
{ 
  // First locate the fit:

  FitIterator pFit = FindFit(sName);

  if(pFit != end()) {
    CCalibFit* p    = pFit->second;	// Pointer to the fit.
    CHECK(p, "Null fit pointer");
    CCalibFit::Point Pt;		// CCalibFit::AddPoint requires one of these.
    for(int i =0; i < vPoints.size(); i++) {
      Pt.x = vPoints[i].X();
      Pt.y = vPoints[i].Y();

      p->AddPoint(Pt);		// Add points one at a time.
    }
    return true;
  }
  else {			// No such fit
    return false;
  }
}  

/*! 


Evaluate the fit at a given point, given that it was
performed in the first place.  If it was not an exception
is thrown.


\param sName (string)
   The name of the fit to evaluate at.
\param x  (double)
   The point to evaluate.

\pre The fit must have been performed.

\return double
\return double
\retval the fit function evaluated at the point passed in.

\throw PreconditionException the fit has not been successfully performed.
\throw PreconditionException the specified fit does not exist.


*/
double 
CCalibFitFactory::Evaluate(string sName, double x)  
{
  // Precondition: the fit must exist:

  FitIterator pFit = FindFit(sName);
  CHECK(pFit != end(),
	"Fit does not exist");
  
  // Fit state must be "Performed":

  CCalibFit* p = pFit->second;
  CHECK(p, "Null fit pointer");
  CHECK(p->GetState() == CCalibFit::Performed,
	"Fit not yet performed on this data set");

  // Now we can be assured of a good eval:

  return (*p)(x);
}  

/*!
  Return an iterator to the begining of the fit creator map.

  \return FitCreatorIterator

 */
CCalibFitFactory::FitCreatorIterator 
CCalibFitFactory::beginCreators()
{
  return m_mapCreators.begin();
}
/*!
  Return an end of iteration iterator to the fit creator map.
*/
CCalibFitFactory::FitCreatorIterator 
CCalibFitFactory::endCreators()
{
  return m_mapCreators.end();
}
/*!
  Return the number of fit creators that are defined.
*/
int    
CCalibFitFactory::sizeCreators()
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
CCalibFitFactory::FitCreatorIterator
CCalibFitFactory::FindFitCreator(string sType)  
{
  // This assumes the fit creators are in a map.

  FitCreatorIterator pFitCreator = m_mapCreators.find(sType);
  return pFitCreator;
}  



/*! 


Return the number of fits that are in the fit factory's
m_mapDefinedFits map.

\return int
\retval Number of fits in m_mapDefinedFits.


*/
int 
CCalibFitFactory::size()  
{
  return m_mapDefinedFits.size();
}  

/*! 


Returns a start of iteration iterator to the m_mapDefinedFits
map.


\return FitIterator
\retval m_mapDefinedFits.begin()

\endverbatim

*/
CCalibFitFactory::FitIterator 
CCalibFitFactory::begin()  
{ 
  return m_mapDefinedFits.begin();
}  

/*! 


Returns an end of fit iterator to m_mapDefinedFits


\return FitIterator
\retval m_mapDefinedFits.end()


*/
CCalibFitFactory::FitIterator 
CCalibFitFactory::end()  
{
  return m_mapDefinedFits.end();
}  

/*! 


Locate a specific, named fit.


\param sName (string)
   Name of the fit to search for.


\return FitIterator
\retval end() if the named fit does not exist.
\retval other If the named fit does exist, the iterator will point to the 
     pair<string, CCalibFit*> of the fit.


*/
CCalibFitFactory::FitIterator 
CCalibFitFactory::FindFit(string sName)  
{
  if(m_mapDefinedFits.empty()) {
    return m_mapDefinedFits.end();
  } 
  else {
    return m_mapDefinedFits.find(sName);
  }
}
