#include "CUnpacker.h"    				
#include "CModule.h"
#include <Event.h>
#include <Analyzer.h>
#include <TCLAnalyzer.h>
#include <BufferDecoder.h>
#include <Histogrammer.h>
#include <Globals.h>
#include <iostream.h>
#include <algorithm>
// internal classes:  NameMatch is a predicate that
// matches names of modules.
//
class NameMatch {
    string m_sName;
public:
    NameMatch(const string& rName) :
	m_sName(rName) {}
    bool operator()(CModule* pModule) {
	return (m_sName == pModule->getName());
    }
};


// implemenation of CUnpacker.

/*!
    Constructs an unpacker.   No action is required.
*/
CUnpacker::CUnpacker (int id) :
  m_nPacketId(id)
{   
} 
/*!
    Destroys the unpacker. No action is required.
*/
 CUnpacker::~CUnpacker ( )  //Destructor - Delete dynamic objects
{
}

/*!
    Copy constructor:
    \param rhs (const CUnpacker& [in]) Reference to the item that we are constructing
	a copy of.
*/
CUnpacker::CUnpacker (const CUnpacker& aCUnpacker ) :
    CEventProcessor(aCUnpacker),
    m_Unpackers(aCUnpacker.m_Unpackers),
    m_nPacketId(aCUnpacker.m_nPacketId)
{
} 

/*!
    Assignment operator.
    \param aCUnpacker (const CUnpacker& [in]) Right hand side of the assignment
	operator
    \return reference to this to support operator chaining.
*/
CUnpacker& CUnpacker::operator= (const CUnpacker& aCUnpacker)
{ 
    if(&aCUnpacker != this) {
	CEventProcessor::operator=(aCUnpacker);
	m_Unpackers = aCUnpacker.m_Unpackers;
	m_nPacketId = aCUnpacker.m_nPacketId;
    }
    return *this;
}

/*!
    Equality comparison:
*/
int CUnpacker::operator== (const CUnpacker& aCUnpacker) const
{ 
    return (CEventProcessor::operator==(aCUnpacker) &&
		(m_Unpackers ==aCUnpacker.m_Unpackers)  &&
	    (m_nPacketId == aCUnpacker.m_nPacketId) );
}

// Functions for class CUnpacker

/*!
    Handle begin run buffers.  At this time, all the unpackers are
    re-initialized so that they pick up any configuration changes.
    \parameter all parameters are ignored.

    \return kfTRUE indicating everything worked correctly.
*/
Bool_t
CUnpacker::OnBegin(CAnalyzer& rAnalyzer,
		   CBufferDecoder& rDecoder)
{
  UnpackerIterator i = begin();
  while(i != end()) {
    
    (*i)->Setup(rAnalyzer,
		*((CHistogrammer*)gpEventSink));
    i++;
  }
  return kfTRUE;
}
/*!  Function: 	
  
  
Called to process an event.  In sequence,
each of the modules is called to process its
part of the event (if present).
    \param pEvent (Address_t [in]): Pointer to the event.
    \param rEvent  (CEvent& [out]): Reference to the parameter array for this event.
    \param rAnalyzer (CAna,yzer& [io]): reference to the analyzer attached to this run of
	SpecTcl.
    \param rDecoder (CBufferDecoder& [in]): refrence to the overall buffer decoder.
    
\note  This function assumes that the first word of an event is the word count
and lets the analyzer know that.  The first word is then skipped prior to 
calling the decoders.
*/
Bool_t
CUnpacker::operator()(Address_t pEvent, CEvent& rEvent, 
				CAnalyzer& rAnalyzer, 
				CBufferDecoder& rDecoder)  
{
     // Extract the word count, and inform the analyzer:

  // UShort_t* psEvent((UShort_t*) pEvent);    // Event pointer as showrt pointer.
    TranslatorPointer<UShort_t> p(*(rDecoder.getBufferTranslator()), 
						pEvent);
    CTclAnalyzer&      rAna((CTclAnalyzer&)rAnalyzer);
    UShort_t  nWords = *p++; 
    rAna.SetEventSize(nWords*sizeof(UShort_t)); // Set event size.
    


    // Hunt for our packet.  If we find it we unpack
    // as per our configuration, otherwise we pass on this assuming
    // that we may be part of a larger experiment that
    // will unpack the id's that we don't recognize.

    nWords--;			// We've already passed the initial word count.
    while(nWords) {
      UShort_t nPacketSize = p[0];
      UShort_t nPacketId   = p[1];
      if(nPacketId == m_nPacketId) {
	TranslatorPointer<UShort_t> psEvent(p + 2); // Packet body.
	TranslatorPointer<UShort_t> pEndOfEvent(p+nPacketSize);

	// Now iterate through the modules.  Procesing stops when either we run out of data
	// or we run out of unpackers.  In this implementation, there's no assumption of
	// a packet structure.  It's therefore up to the user to ensure that all module
	// unpackers have actually been registered... or else the entire event may not get 
	// unpacked.
	// If we run out of unpackers, before we run out of events, an error
	// message is emitted, but processing continues.
	
	UnpackerIterator i  = begin();
	while ((i != end()) && (psEvent != pEndOfEvent)) {
	  psEvent = ((*i)->Unpack(psEvent, rEvent, rAnalyzer,
					     rDecoder));
	  i++;
	}
	
	if (psEvent != pEndOfEvent) {
	  cerr << "Unpacker ran out of event processors before "
	       << "the end of the event\n";
	  return kfFALSE;	// Event format error!!
	}	
	break;			// There's no reason to look for more matching
				// packets.
	
      }
      nWords  -= nPacketSize;
      p       += nPacketSize;
      
    }
    


 

    return kfTRUE;
    
}  

/*!  Function: 	

Adds a module to the end of the unpackers.
\param rModule (CModule& [in]): The module unpacker to add.

*/
void 
CUnpacker::Add(CModule& rModule)  
{ 
    rModule.Setup(getAnalyzer(), getHistogrammer());
    m_Unpackers.push_back(&rModule);
}  

/*!  

Removes a module from the module list given an iterator.
\param p (UpackerIterator [in]): Pointer to the event to delete
     from the list.
     
    \note The module is not deleted.  It's up to the creator
              of the module to destroy it.
*/
void 
CUnpacker::Delete(UnpackerIterator p)  
{ 
    m_Unpackers.erase(p);
}  

/*!  

Attempts to locate the module named
as per the enclosed string. This is done
using the generic algorithm find_if
and a predicate that does string matching.

\param rName (const string & [in]): Name to match.
*/
CUnpacker::UnpackerIterator 
CUnpacker::Find(const string& rName)  
{ 
    return find_if(begin(), end(),
			NameMatch(rName));
}  

/*!  
Returns a beginning of iteration iterator


*/
CUnpacker::UnpackerIterator 
CUnpacker::begin()  
{ 
    return m_Unpackers.begin();
}  

/*!  

Returns an end of iteration iterator.

*/
CUnpacker::UnpackerIterator 
CUnpacker::end()  
{ 
    return m_Unpackers.end();
}  

/*!  : 	

Returns the number of modules in the unpacker.

*/
int 
CUnpacker::size()  
{ 
    return m_Unpackers.size();
}
/*!
   get the current analyzer.  This is done by consulting the global
   variables.
*/
CAnalyzer&
CUnpacker::getAnalyzer()
{
  return *gpAnalyzer;
}
/*!
  Get the current event sink cast as a histogrammer.
*/
CHistogrammer&
CUnpacker::getHistogrammer()
{
  return *((CHistogrammer*)gpEventSink);
}
