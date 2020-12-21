/*
     This software is Copyright by the Board of Trustees of Michigan
     State University (c) Copyright 2014.
 
     You may use this software under the terms of the GNU public license
     (GPL).  The terms of this license are described at:
 
      http://www.gnu.org/licenses/gpl.txt
 
      Authors:
              Ron Fox
              Jeromy Tompkins
              NSCL
              Michigan State University
              East Lansing, MI 48824-1321
 
 */
/*!
  \class CScriptableUnpacker
  \file  .h

  This event processor wraps the entire scriptable event processing
  subsystem.  Construction is 2-phase to allow this objects to be
  constructed in the early stages of SpecTcl's startup (even at
  static init time).  The bulk of the real construction work is done
  
  
  at Attach time (OnAttach):
  - A module dictionary is created to hold the set of modules
    that can be added to the unpacker.
  - The module command is created and all the
     currently known modules are registered
    (we have members to delegate the 
     registration of additional modules).
  - A CPacket is created and registered with the
    name unpack  this will be the top level unpacker
    for the entire scheme.
  
  OnBegin causes all of the module Setup functions to be called.
  The m_fSetupDone flag is used to keep track of whether or not
  a special setup must be done on the first data buffer of a run.
  - It is initialized false.
  - Set true on Begin
  - Set false on End.
  If a buffer arrives that is not
  Begin or End, but the flag is false, Setup is done.

*/

// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef CSCRIPTABLEUNPACKER_H  //Required for current class
#define CSCRIPTABLEUNPACKER_H

//
// Include files:
//
#include <EventProcessor.h>
#include <string>

                               //Required for 1:1 association classes

// Forward class definitions (convert to includes if required):

class CPacket;
class CModuleCommand;
class CAnalyzer;
class CBufferDecoder;
class CModuleCreator;
//

class CScriptableUnpacker : public CEventProcessor     
{
private:
  

  bool            m_fSetupDone;     //!<  true if Setup done at run start.  
  std::string          m_sCreatorName;   //!< Name of the creator object.
  std::string          m_sTopLevelName;  //!< Name of the top level packet.
  CPacket*        m_pTopLevel;      //!< Top level event decoder.
  CModuleCommand* m_pModuleCommand; //!< Module command processor.

protected:
	// Protected member data.
 
public:
	// Public member data.

public:
    //  Constructors and other canonical operations.

    CScriptableUnpacker (std::string CreatorName = std::string("module"),
			 std::string TopLevelName= std::string("unpack")); //!< Constructor.
    virtual  ~CScriptableUnpacker ( ); //!< Destructor.

    virtual CScriptableUnpacker* clone() { return new CScriptableUnpacker(*this); }
    //DAQ::DDAS::CParameterMapper* m_mapper;
    DAQ::DDAS::FitParameterMapper* m_mapper;  
    //virtual void setParameterMapper(DAQ::DDAS::CParameterMapper& rParameterMapper) { m_mapper = &rParameterMapper; }
    virtual void setParameterMapper(DAQ::DDAS::FitParameterMapper& rParameterMapper) { m_mapper = &rParameterMapper; }  
  
private:
    //CScriptableUnpacker (const CScriptableUnpacker& rSource ); //!< Copy construction.
    CScriptableUnpacker& operator= (const CScriptableUnpacker& rhs); //!< Assignment.
    int operator== (const CScriptableUnpacker& rhs) const; //!< == comparison.
    int operator!= (const CScriptableUnpacker& rhs) const; //!< != comparison.
public:

    // Selector (for testing).

  bool isSetup() const {
    return m_fSetupDone;
  }
  CPacket* getTopLevel() {
    return m_pTopLevel;
  }
  CModuleCommand* getModuleCommand() {
    return m_pModuleCommand;
  }

// Class operations:

public:

  virtual   Bool_t   OnAttach (CAnalyzer& rAnalyzer)   ; //!< When attached to analyzer
  virtual   Bool_t OnBegin  (CAnalyzer& rAnalyzer, 
			     CBufferDecoder& rDecoder)   ; //!< At begin run
  virtual   Bool_t OnResume (CAnalyzer& rAnalyzer, 
			     CBufferDecoder& rDecoder)   ; //!< At resume run
  virtual   Bool_t OnEnd    (CAnalyzer& rAnalyzer, 
			     CBufferDecoder& rDecoder)   ; //!< At end run.
  virtual   Bool_t OnPause  (CAnalyzer& rAnalyzer, 
			     CBufferDecoder& rDecoder)   ; //!< At pause run.
  virtual   Bool_t OnOther  (CAnalyzer& rAnalyzer, 
			     CBufferDecoder& rDecoder)   ; //!< At any other buf.
  virtual   Bool_t operator() (const Address_t pBuffer, 
			       CEvent& rEvent, 
			       CAnalyzer& rAnalyzer, 
			       CBufferDecoder& rDecoder)   ; //!< For a physics event.
  
  void RegisterCreator(const std::string& rType, CModuleCreator* pCreator);
protected:
  virtual void RegisterCreators(); //!< Register module creators
private:

};

#endif

