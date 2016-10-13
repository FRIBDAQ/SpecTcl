/*
	Implementation file for CCalibratedParameterManager for a description of the
	class see CCalibratedParameterManager.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>
#include "CCalibratedParameterManager.h"    				
#include "CCalibratedParameter.h"
#include <DesignByContract.h>
#include <Exception.h>
#include <string>
#include <iostream>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

using namespace DesignByContract;

// Utility classes:

/*!
    This is a functional object that is used in a for_each to visit
    each element of the calibrated paraemeter map and delete the parameters
    attached to the map
    
    WARNING: The parameters are removed, but the map elements themselves are
    still in place, holding pointers to invalid parameters.  The for_each
    that uses this must be immediately followed by a full map erasure.
 */
class Destroyer {					//!< function to destroy map  calibrated param
public:
	void operator()(CCalibratedParameterManager::MapEntry i) {
		delete i.second;
	}
};


// Static storage for CCalibratedParameterManager:

CCalibratedParameterManager::CalibratedParameterDictionary
                             CCalibratedParameterManager::m_CalibratedParameters;

/*!
    Create an object of type CCalibratedParameterManager
*/
CCalibratedParameterManager::CCalibratedParameterManager () 
{ 
} 

/*!
    Called to destroy an instance of CCalibratedParameterManager
*/
 CCalibratedParameterManager::~CCalibratedParameterManager ( )
{

}
/*!
   Called to create an instance of CCalibratedParameterManager that is a
   functional duplicate of another instance.
   \param rSource (const CCalibratedParameterManager& ):
      The object that we will dupliate.
*/
CCalibratedParameterManager::CCalibratedParameterManager (const CCalibratedParameterManager& rhs )  :
	CEventProcessor(rhs)
{
	
} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.
  \param rhs (const CCalibratedParameterManager& rhs ):
     The object that will be functionally copied to *this.
 */
CCalibratedParameterManager& 
CCalibratedParameterManager::operator= (const CCalibratedParameterManager& rhs)
{ 
	if(this != &rhs) {
		CEventProcessor::operator=(rhs);

	}
	return *this;

}
/*!
  Compare *this for functional equality with another object of
  type CCalibratedParameterManager.
  \param rhs (const CCalibratedParameterManager& rhs ):
     The object to be compared with *this.

 */
int 
CCalibratedParameterManager::operator== (const CCalibratedParameterManager& rhs) const
{
	return CEventProcessor::operator==(rhs);
}
/*!
   Compare *this for functional inequality with another object
   of type CCalibratedParameterManager.  Functional inequality is defined as
   the boolean inverse of functional equality.
   \param rhs (const CCalibratedParameterManager& rhs ):
      The object to compare with *this.
*/
int
CCalibratedParameterManager::operator!= (const CCalibratedParameterManager& rhs) const
{
  return !(*this == rhs);
}

// Functions for class CCalibratedParameterManager

/*! 

Description:

Calculates all of the calibrated parameters 
by iterating through the m_CalibratedParameters 
container.


Parameters:

\param pEvent (Address_t)
    Pointer to the raw event.
\param rEvent   (CEvent&)
    Reference to the unpacked event.
\param rAnalyzer (CAnalyzer&)
    Reference to the calling analyze (unused)
\param rDecoder (CBufferDecoder&)
    Reference to the decoder that's working on the buffer. 
   (unused).

   

\return Bool_t
  \retval kfTRUE - if everything worked (and it must).
  \retval kfFALSE - if there were any failures

Pseudo code:

\verbatim
Foreach fit in m_CalibratedParameters
   Parameter(rEvent)
end foreach

\endverbatim

Note, since this is in the innermost loop, we don't do the
for_each iteration to save the over head of the function
object dispatch.

*/
Bool_t 
CCalibratedParameterManager::operator()(const Address_t pEvent, 
										CEvent& rEvent, 
										CAnalyzer& rAnalyzer, 
										CBufferDecoder& rDecoder)  
{
	CalibratedParameterIterator i = begin();
	
	while (i != end()) {
		CCalibratedParameter* p= i->second;
		if(p) {
			try {
				(*p)(rEvent);
			}
			catch (DesignByContractException& contract) { // pre/post condition
				OutputEvaluationException(ReportEvaluationException(i),
										  contract);
			}
			catch (CException& rExcept) {                 // SpecTcl exceptions
				cerr << ReportEvaluationException(i) << ":\n";
				cerr << rExcept.ReasonText() << " Parameter not evaluated\n";
			}
			catch (string& msg) {                         // quicky 1.
				OutputEvaluationException(ReportEvaluationException(i),
				                          msg);
			}
			catch (char* msg)   {                         // quicky 2.
				OutputEvaluationException(ReportEvaluationException(i),
										  msg);
			}
			catch (...) {                                 // anything else.
				OutputEvaluationException(ReportEvaluationException(i),
										  "Unanticpated exception type caught");
			}
		}
		i++;
	}
	return kfTRUE;
}  

/*! 

Description:

Adds a new calibrated parameter to the m_CalibratedParameters container
Once added operator() will immedately start evaluating this parameter.
A calibrated parameter of this name must not yet exist.  The assumption
is that the parameter is a dynamically allocated element.

\pre FindParameter(rName) == end()
\pre pParam != NULL

\post FindParameter(rName) != end()

Parameters:

\param rName (string)
   Name of the calibrated parameter.  This name can be 
used to look up the paramter later (FindParameter) and
must be unique.  Note as well that by convention the name
corresponds to the actual SpecTcl parameter that is being computed
by this calibration.
\param pParam (CCalibratedParameter*)
  A pointer to the parameter to add.  The caller
is assumed to own this parameter.

\return void


Pseudo code:

\verbatim
If rName in map {
   delete parameter and map entry.
}]
m_CalibratedParameters[rName] =  pParam)

\endverbatim

*/
void 
CCalibratedParameterManager::AddParameter(string rName, 
										  CCalibratedParameter* pParam)  
{ 
	REQUIRE(FindParameter(rName) == end(), "Duplicate");  // precondition
	REQUIRE(pParam, "Null parameter");

	m_CalibratedParameters[rName] = pParam;
	
	ENSURE(FindParameter(rName) != end(), " Insertion failure"); // postcondition
}  

/*! 

Description:

Removes the calibrated parameter from the
m_CalibratedParameters container.  

\pre FindParameter(rName) != end()

\post FindParameter(rName) == end()
\post The pointer to the parameter found is not null.

Parameters:

\param rName (string)
   Name of the parameter to remove.

\return CCalibratedParameter*
   \retval a pointer to the calibrated parameter that was
    removed.  The caller will normally need this to delete it.
   \retval NULL - the parameter was not found .. null is only
      possible with debugging disabled.



Pseudo code:

\verbatim
iterator <- FindParameter(rName)
if(iterator != end()) THEN
  pParameter = *iterator
  m_CalibratedParameters.erase(iterator)
  return pParameter
ELSE
  return NIL
ENDIF
\endverbatim

*/
CCalibratedParameter* 
CCalibratedParameterManager::DeleteParameter(string rName)  
{ 
	CalibratedParameterIterator i = FindParameter(rName);
	REQUIRE(i != end(), " No such parameter");  // precondition.
	
	CCalibratedParameter* pParam = i->second;
	m_CalibratedParameters.erase(i);
	
	// post condition checking.
	
	ENSURE(FindParameter(rName) == end(), "Erase failed"); 
	ENSURE(pParam, "Parameter pointer deleted from map");
	
	return pParam;
}  

/*! 

Description:

Returns a start of iteration iterator to the m_CalibratedParameters
container.

\return CalibratedParameterIterator



Pseudo code:

\verbatim
return m_CalibratedParameters.begin()

\endverbatim

*/
CCalibratedParameterManager::CalibratedParameterIterator 
CCalibratedParameterManager::begin()  
{
	return m_CalibratedParameters.begin(); 
}  

/*! 

Description:

Return an end of iteration iterator to the
m_CalibratedParameters container.
\return CalibratedParameterIterator

Pseudo code:

\verbatim
return m_CalibratedParameters.end()
\endverbatim

*/
CCalibratedParameterManager::CalibratedParameterIterator 
CCalibratedParameterManager::end()  
{ 
  return m_CalibratedParameters.end();
}  

/*! 

Description:

Return the number of calibrated parameters
currently in m_CalibratedParameters

Parameters:

\return size_t
\retval number of elements in the list.


Pseudo code:

\verbatim
return m_CalibratedParameters.size()

\endverbatim

*/
size_t 
CCalibratedParameterManager::size()  
{ 
	return m_CalibratedParameters.size();
}  

/*! 

Description:

Locate the parameter requested.


Parameters:

\param Name (string)
   Name of the parameter to find.


\return CalibratedParameterIterator
 \retval end() - not found
 \retval anything else: An iterator that 'points' to the 
  parameter

Pseudo code:

\verbatim
return std::find(begin, end, Matchname)
\endverbatim

*/
CCalibratedParameterManager::CalibratedParameterIterator 
CCalibratedParameterManager::FindParameter(string rName)  
{
	return m_CalibratedParameters.find(rName); 
}




/*!
   Utility function that reports a parameter exception.  This factors
   out common code from the exception handling code making stuff simpler.
   \param i (CalibratedParameterIterator)
      iterator to a calibrated parameter whose evaluation caused an
      exceptoin
    \return string
    \retval a message to prepend to the exception message.
            
*/
string 
CCalibratedParameterManager::ReportEvaluationException(CCalibratedParameterManager::CalibratedParameterIterator i)
{
	string name                      = i->first;
	CCalibratedParameter* pParameter = i->second;
	
	string message("An exception was thrown evaluating parameter: ");
	message      += name;
	message      += " which is a calibrated parameter with fit: ";
	message      += pParameter->getFitName();
	
	return message;
}
/*!
   Utility function that reports 'writable' exceptions to stderr.
   A writable exception is any exception that can be written to an
   ostream via the << operator.
   \param prefix (string)
      A prefix string that is prepended to the exception message.
   \param rExcept (T)
      a 'writable' exception that is used as the message body.
      
*/
template <class T> void
CCalibratedParameterManager::OutputEvaluationException(string prefix,
                                                       T&     rExcept) 
{
	cerr << prefix << ";\n";
	cerr << rExcept << " Parameter not evaluated\n";
}
