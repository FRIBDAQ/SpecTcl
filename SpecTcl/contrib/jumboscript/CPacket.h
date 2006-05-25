
/*!
  \class CPacket
  \file  .h

  Decodes a packet.  A packet is an event segment 
  that contains other unpackers (modules and packets).
  The CPacket unpacker supports two fundamentally different
  types of unpacking:
  - Tagged:  The event segment must be an NSCL identified packet.
     The packet id must match m_nId. Otherwise no recognition
     occurs and the object passes on the opportunity to unpack
     any data.
  - Untagged - Arbitrary data.  The packet gives each of its
     members the opportunity to scoop some data out of the
     event in to parameters.  In this case, the packet object
     is just used as a convenient (to the programmer) organizing
     tool.

*/

// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef __CPACKET_H  //Required for current class
#define __CPACKET_H

//
// Include files:
//

                               //Required for base classes
#ifndef __CSEGMENTUNPACKER_H     //CSegmentUnpacker
#include "CSegmentUnpacker.h"
#endif



#ifndef __STL_STRING
#include <string>        //Required for include files  
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_LIST
#include <list>
#ifndef __STL_LSIT
#define __STL_LIST
#endif
#endif

// Forward class definitions.

class CModuleDictionary;
class CTCLInterpreter;
class CTCLResult;

///

class CPacket  : public CSegmentUnpacker        
{
  // Publicly available types:

public:
  typedef  STD(list)<CSegmentUnpacker*>    SegmentList;
  typedef  SegmentList::iterator      ModuleIterator;
private:
  
  // Private Member data:
  int                         m_nId;         //!<  m_fPacket::packet id.  
  bool                        m_fPacketize;  //!<  True if decoding an id'd packet.  
  
  SegmentList                 m_Decoders;    //!< Set of decoders added to us.
  CModuleDictionary*          m_pModules;    //!< Dictionary of defined modules. 

  int                         m_nPacketSize; //!<  Size of packet being decoded.  
  TranslatorPointer<UShort_t>* m_pPacketBase;  //!<  Base of current packet.  




public:
    //  Constructors and other canonical operations.
    //  You may need to adjust the parameters
    //  and the visibility esp. if you cannot
    // implement assignment/copy construction.
    // safely.
    CPacket (const STD(string)&      rName,
	     CTCLInterpreter&   rInterp,
	     CModuleDictionary* pDictionary);		//!< Constructor.
    virtual  ~ CPacket ( ); //!< Destructor.
private:
    CPacket (const CPacket& rSource ); //!< Copy construction.
    CPacket& operator= (const CPacket& rhs); //!< Assignment.
    int operator== (const CPacket& rhs) const; //!< == comparison.
    int operator!= (const CPacket& rhs) const; //!< != comparison.
public:

  // Selectors needed for testing:
public:
  bool Packetized() const 
  {
    return m_fPacketize;
  }
  CModuleDictionary* getDictionary() { // for testing.
    return m_pModules;
  }

// Class operations:

public:

  // Base class overrides:

  virtual   void OnDelete ()   ;                     //!< Called prior to deletion.
  virtual   int operator() (CTCLInterpreter& rInterp, 
			    CTCLResult& rResult, 
			    int nArgs, char** pArgs); //!< Command processor
  virtual void Setup (CAnalyzer&     rAnalyzer,
		      CHistogrammer& rHistogrammer)   ;	//!< Prepare to unpack.
  //!  Decode event segment
  virtual   TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> pBuffer, 
					       CEvent& rEvent, 
					       CAnalyzer& rAnalyzer, 
					       CBufferDecoder& rDecoder)   ; 
  virtual STD(string) getType() const; //!<    Return the 'unpacker type'.

  // Non-virtual functions:

  int AddModuleCommand (CTCLInterpreter& rInterp, 
			CTCLResult& rResult, 
			int nArgs, char** pArgs)   ; //!< 'add' subcommand.
  int RemoveModuleCommand (CTCLInterpreter& rInterp, 
			   CTCLResult& rResult, 
			   int  nArgs, char** pArgs)   ; //!< remove subcommand
  int ListModulesCommand (CTCLInterpreter& rInterp, 
			  CTCLResult& rResult, 
			  int nArgs, char** pArgs)   ; //!< list subcommand

  void AddModule (CSegmentUnpacker* pModule)   ; //!< Really add a module.
  void RemoveModule (CSegmentUnpacker* pModule)   ; //!< Remove module.
  void RemoveModule (ModuleIterator module)   ; //! Remove module @ iterator.
  
  int getPacketSize (TranslatorPointer<UShort_t> p)   ; //!< How big is segment?
  

  // Utility member functions.

protected:
  virtual   STD(string) Usage ()   ;                       //!< print usage. 

  ModuleIterator FindDecoder(const STD(string)& name); 
  int            NumDecoders() const;
  ModuleIterator BeginDecoders();
  ModuleIterator EndDecoders();

  TranslatorPointer<UShort_t> UnpackModules(TranslatorPointer<UShort_t> pBuffer, 
					    CEvent& rEvent, 
					    CAnalyzer& rAnalyzer, 
					    CBufferDecoder& rDecoder)   ; 
  
  friend class PacketTest;
};

#endif

