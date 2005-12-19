

/*
	Implementation file for CModule for a description of the
	class see CModule.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>
#include "CModule.h"    				
#include <Histogrammer.h>
#include <Parameter.h>
#include <RangeError.h>
#include <Globals.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



// Static attribute storage and initialization for CModule

/*!
    Create an object of type CModule
*/
CModule::CModule (const string& rName, CTCLInterpreter& rInterp) :
  CSegmentUnpacker(rName, rInterp)
{ 
} 

/*!
    Called to destroy an instance of CModule
*/
 CModule::~CModule ( )
{
}

// Functions for class CModule

/*! 

Description:

Creates a new parameter map.  
- Any existing parameter map is discarded.
- A new parameter map of the specified size is
  created and initialized such that attempts to
  do channel->id translations for all channels
  will produce -1 (undefined).


\pre none

\post m_ParameterMap.size() == nMapSize

Parameters:

\param nMapSize   (int [in]):
   Number of elements in the map.

\return void
NONE

\throw  string
  -  "Map sizes cannot be negative"

Pseudo code:

\verbatim
Clear m_ParameterMap
for i =0; i < nMapSize; i++
   push_back -1 to m_ParameterMap

\endverbatim

*/
void 
CModule::CreateMap(int nMapSize)  
{

  if(nMapSize < 0) {
    throw string("Map sizes cannot be negative");
  }
  // Clear Parameter map:

  if(!m_ParameterMap.empty()) {
    m_ParameterMap.erase(m_ParameterMap.begin(),
			 m_ParameterMap.end());
  }
  // Create a map full of undefineds:

  for(int i=0; i < nMapSize; i++) {
    m_ParameterMap.push_back(-1);
  }
}  

/*! 

Description:

Define a mapping between a channel and a SpecTcl
parameter name.  If the channel number is larger than the
map size a CRangeError Exception is thrown.
If the parameter name does not correspond to an
existing SpecTcl parameter, the map entry remains
undefined.

\pre none

\post none

Parameters:

\param nChannel (int [in]):
    Number of the channel to map.
\param sName (const string& [in]):
    Name of the spectcl parameter that should
    be bound to this entry.

\return void

\throw  CRangeError

Pseudo code:

\verbatim
if nChannel < m_ParameterMap size
   pParameter = LocateParameter(sName)
   if(pParameter) 
       m_ParameterMap[nChannel] = pParameter.id
   else 
       m_ParameterMap[nChanne] = undefined 
else
   throw CRangeError
\endverbatim

*/
void 
CModule::MapElement(int nChannel, const string& sName)  
{ 

  // Ensure nChannel is in the range of the map:

  if((nChannel < m_ParameterMap.size()) && (nChannel >= 0)) {

    // Find the parameter:

    CHistogrammer* pHistogrammer = (CHistogrammer*)gpEventSink;
    CParameter*    pParameter    = pHistogrammer->FindParameter(sName);

    // Set map entry if not null else set map to undefined (-1)

    if(pParameter) {
      m_ParameterMap[nChannel] = pParameter->getNumber();
    }
    else {
      m_ParameterMap[nChannel] = -1;
    }
    
  }
  else {			// Out of range!!
    throw CRangeError(0, m_ParameterMap.size(), nChannel, "CModule::MapElement");
  }
}  

/*! 

Description:

Returns the SpecTcl parameter id to
which a channel maps.
- If the channel is out of range a 
   CRangeError is thrown
- It is possible to return -1 which means there
  is no SpecTcl parameter that corresponds to
  this channel.

\pre none

\post none

Parameters:

\param nChannel (int [in]):
    Number of the channel to translate.

\return int
\retval     -1   The channel has no SpecTcl parameter.
\retval  != 0   The parameter id (index into rEvent) that
                      corresponds to nChannel.

\throw  CRangeError

Pseudo code:

\verbatim
if nChannel < size of m_ParameterMap 
    return m_ParameterMap[i]
else
    throw CRangeError
\endverbatim

*/
int 
CModule::Id(int nChannel)  
{
  // Range check the lookup:

  if((nChannel < m_ParameterMap.size()) && (nChannel >= 0)) {
    return m_ParameterMap[nChannel];
  } 
  else {
    throw CRangeError(0, m_ParameterMap.size(), nChannel, "CModule::Id()");
  }
}
