

/*
	Implementation file for CSegmentUnpacker for a description of the
	class see CSegmentUnpacker.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include "CSegmentUnpacker.h"    				
#include "CPacket.h"
#include <string>


// Static attribute storage and initialization for CSegmentUnpacker

/*!
    Create an object of type CSegmentUnpacker
    - Initially we are not owned.
    \param rName (const string& [in]):
        The name to give to the object (passed to the base class constructor).
    \param rInterp (CTCLInterpreter& [in]):
        The interpreter on which the object will be registered as a command.
	(passed to the base class constructor).
*/
CSegmentUnpacker::CSegmentUnpacker (const string&    rName,
				    CTCLInterpreter& rInterp) :
  CConfigurableObject(rName, rInterp),
  m_pOwner(0) 
{ 
} 

/*!
    Called to destroy an instance of CSegmentUnpacker
*/
 CSegmentUnpacker::~CSegmentUnpacker ( )
{
  OnDelete();			// Just to be sure...
}


// Functions for class CSegmentUnpacker

/*! 
\fn TranslatorPointer<UShort_t> CSegmentUnpacker::Unpack(TranslatorPointer<UShort_t> pEvent, CEvent& rEvent, CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)  


Description:

Unpack an event segment.  This is an interface
(pure virtual function) specification.  In a concrete
class the implementer must Recognize and unpack:
- Recognize: Make a positive identification that this
   object is supposed to be unpacking the data that
    is pointed to by the buffer pointer.
- Unpack: If recognition is successful, the
  recognized data must be decoded and placed
  in an appropriate set of SpecTcl parameters in the
  rEvent array.   How the appropriate set is determiend
  is up to the individual unpacker, however the
  CModule base class supports an interface that
   allows a set of parameter names for each channel
   to be mapped to individual SpecTcl parameter names.

\pre fully constructed && configured

\post none

Parameters:

\param pEvent (TranslatorPointer<UShort_t> [in]):
      pointer like object that 'points' to the raw event.  
      TranslatorPointers will automaticallly byte swap as needed to
      deal with endien-ness mismatches.
\param rEvent (CEvent& [out]):
     Reference to the event 'array'  rEvent is an array like
     object into which raw parameters are placed. 
\param rAnalyzer (CAnalyzer& [in]):
    Reference to the analyzer object (in case any analyzer object
    services are required by the unpacker).
\param rDecoder (CBufferDecoder& [in]):
    Reference to the buffer decoder object (in case any buffer decoder
    services are required).

\return TranslatorPointer<UShort_t>
\retval  If recognized:  Points to the next event segment to decode;
    The returned translator pointer points to the first word in the event
     following the last word that this unpacker recognized.
\retval If not recognized: pEvent

\throw  CEventFormatError


*/


/*! 

\fn void CSegmentUnpacker::Setup(CAnalyzer& rAnalyzer, CHistogrammer& rHistogrammer) 

Description:

This is an interface specification (pure virtual).  
A concrete class that derives from us must
fetch and program the configuration.
- Whatever pieces of the unpacker's 
  configuration are required for unpacking must
   be fetched.
- The relevant configuration parameters must be
   saved, manipulated or whatever else is needed so 
   that subsequent unpack calls can operate correctly.


\pre fully constructed && configured

\post fully constructed && configured

Parameters:

\param rAnalyzer (CAnalyzer& [in]):
   A reference to the analyzer object.  This is
   provided in case any analyzer services are required.
\param rHistogrammer (CHistogrammer& [in]):
    A reference to the histogramming sink.  This is provided
   in case it is necessary to look-up any parameters, spectra
   or whatever else the histogramer can provide.

*/
 


/*! 

Description:

Sets the ownership of *this to a specific CPacket object. 
 All segment unpackers except the
top level unpacker are owned by a packet (the top l
evel unpacker is just a packet).  The m_pOwner
field tracks ownership.


\pre m_pOwner  == nil

\post m_pOwner != nil

Parameters:

\param pOwner (CPacket* [in]):
    Pointer to the new packet that owns us.

\return void

\throw "Attempt to set ownership of a previously owned unpacker" string

Pseudo code:

\verbatim
m_pOwner != nil::throw string "Already owned object..."
m_pOwner == nil::m_pOwner <-- pNewOwner
\endverbatim

*/
void 
CSegmentUnpacker::Own(CPacket* pNewOwner)  
{
  if(m_pOwner) {
    throw string("CSegmentUnpacker::Own Unpacker module previously owned ");
  }
  else {
    m_pOwner = pNewOwner;
  }
}  

/*! 

Description:

Called by the owning packet to disown this 
module.  This might be done, for example because
the module is being removed from the packet, or
becaue the packet itself is being deleted.
See the discussion in CSegmentUnpacker::Own
for more information about ownership.

\pre m_pOwner != nil

\post m_pOwner == nil

Parameters:

<none>

\return void
<none>

\throw "Not owned but attempting to disown." string

Pseudo code:

\verbatim
m_pOwner == nil:: throw string("not owned attempting to disown"
m_pOwner != nil:: m_pOwner <-- nil
\endverbatim

*/
void 
CSegmentUnpacker::Disown()  
{
  if(m_pOwner) {
    m_pOwner = (CPacket*)NULL; 
  } 
  else {
    throw string("CSegmentUnpacker::Disown - not owned");
  }
}  

/*! 

Description:

Returns true if the segement is currently owned by
a packet.

\pre none

\post none

Parameters:

\return bool
\retval true::m_pOwner != nil
\retval false::m_pOwner == nil.


Pseudo code:

\verbatim
m_pOwner==nil :: return false
m_pOwner!=nil :: return true
\endverbatim

*/
bool 
CSegmentUnpacker::isOwned()   const
{ 
  return (m_pOwner != (CPacket*)NULL);
}  

/*! 

Description:

Should be called just prior to deleting a Segment unpacker.  
This virtual allows segments to take per segment
cleanup functions.  The default operation removes
Disowns us if we are owned.

\pre none

\post none

Parameters:

\return void
C

Pseudo code:

\verbatim
m_pOwner != nil::Disown().
\endverbatim

*/
void 
CSegmentUnpacker::OnDelete()  
{
  if (isOwned()) {
    m_pOwner->RemoveModule(this);	// Remove self from packet.
    m_pOwner = 0;		        //!< In case not really del. 
  }
}  

/*! 

Description:

Returns the current value of the m_pOwnerField.

\pre none

\post none

Parameters:

none

\return CPacket*
none

Pseudo code:

\verbatim
return m_pOwner;
\endverbatim

*/
CPacket* 
CSegmentUnpacker::getOwner()  
{ 
  return m_pOwner;
}
