   
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

#ifndef __CPACKETCREATOR_H  //Required for current class
#define __CPACKETCREATOR_H

//
// Include files:
//

                               //Required for base classes
#ifndef __CMODULECREATOR_H     //CModuleCreator
#include "CModuleCreator.h"
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

 
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

  CPacketCreator (const string& rName,
		  CModuleCommand* pModuleCreator); //!< Constructor.
  virtual  ~ CPacketCreator ( ); //!< Destructor.
  CPacketCreator (const CPacketCreator& rSource ); //!< Copy construction.
  CPacketCreator& operator= (const CPacketCreator& rhs); //!< Assignment.
  int operator== (const CPacketCreator& rhs) const; //!< == comparison.
  int operator!= (const CPacketCreator& rhs) const; //!< != comparison.
  


public:

  virtual   CSegmentUnpacker* Create (CTCLInterpreter& rInterp,
				      const string& rName)   ; 
  virtual   string TypeInfo ()  const ; 
  
  friend class CreatorTest;	/* for test purposes only. */
};

#endif
