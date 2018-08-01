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
  \class CPacketCreator
  \file  CPacketCreator.h

  Creator for  a packet module this module must:
  - Recognize the module type requested (by providing a module type
    name to the base class.
  - Create a new instance of a CPacket for the module command.
    (via the Create member)
  - Provide information for documentation purposes about the
    (Via tye TypeInfo function).
    type of module it creates.

*/

// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef CPACKETCREATOR_H  //Required for current class
#define CPACKETCREATOR_H

//
// Include files:
//

                               //Required for base classes
#include "CModuleCreator.h"
#include <string>

 
// Forward class definitions (convert to includes if required):

class CModuleDictionary;
class CModuleCommand;
class CTCLInterpreter;
class CSegmentUnpacker;

//////

class CPacketCreator  : public CModuleCreator        
{
private:
  
  // Private Member data:
  
  CModuleDictionary* m_pModules; //!< The available modules.


public:
    //  Constructors and other canonical operations.

  CPacketCreator (const std::string& rName,
		  CModuleCommand* pModuleCreator); //!< Constructor.
  virtual  ~ CPacketCreator ( ); //!< Destructor.
  CPacketCreator (const CPacketCreator& rSource ); //!< Copy construction.
  CPacketCreator& operator= (const CPacketCreator& rhs); //!< Assignment.
  int operator== (const CPacketCreator& rhs) const; //!< == comparison.
  int operator!= (const CPacketCreator& rhs) const; //!< != comparison.
  


public:

  virtual   CSegmentUnpacker* Create (CTCLInterpreter& rInterp,
				      const std::string& rName)   ; 
  virtual   std::string TypeInfo ()  const ; 
  
  friend class CreatorTest;	/* for test purposes only. */
};

#endif
