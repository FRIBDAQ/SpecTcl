#ifndef __CUNPACKER_H  //Required for current class
#define __CUNPACKER_H

//
// Include files:
//
#ifndef __EVENTPROCESSOR_h
#include <EventProcessor.h>
#endif

#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

// Forward class definitions:

class CModule;
class CEvent;
class CAnalyzer; 
class CHistogrammer;
class CBufferDecoder;
/*!
    The unpacker is responsible for breaking an event up into 
    elements of the parameter array.  This is done by 
    passing the event through the a series of Modules
    that have been registered in a list we maintain.
*/
class CUnpacker : public CEventProcessor     
{
public:
    typedef list<CModule*>          UnpackerList;
    typedef UnpackerList::iterator  UnpackerIterator;
private:
    UnpackerList m_Unpackers;
    int          m_nPacketId;	  // Packet id we unpack.

public:
    // Constructors and other cannonical functions.
    CUnpacker (int id );
    virtual ~CUnpacker ( );  
    CUnpacker (const CUnpacker& aCUnpacker );
    CUnpacker& operator= (const CUnpacker& aCUnpacker);
    int operator== (const CUnpacker& aCUnpacker) const;
    int operator!=  (const CUnpacker& rhs) const {
	return !(operator==(rhs));
    }

public:
    virtual Bool_t OnBegin(CAnalyzer& rAnalyzer, 
			   CBufferDecoder& rDecoder);
    virtual Bool_t operator() (Address_t pEvent, CEvent& rEvent, 
				       CAnalyzer& rAnalyzer, 
				       CBufferDecoder& rDecoder)   ; 
    void Add (CModule& rModule)   ; 
    void Delete (UnpackerIterator p)   ; 
    UnpackerIterator Find (const string& rName)   ; 
    UnpackerIterator begin ()   ; 
    UnpackerIterator end ()   ; 
    int size ()   ; 
protected:
  CAnalyzer& getAnalyzer();
  CHistogrammer& getHistogrammer();


};

#endif
