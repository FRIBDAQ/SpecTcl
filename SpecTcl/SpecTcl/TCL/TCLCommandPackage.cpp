//  CTCLCommandPackage.cpp
// Provides a set of related commands to a TCL interpreter.  
// References to the commands are stored in the m_lCommands list. 
// m_sSignon provides a signon message which can be retrieved and optionally 
// displayed as the package is installed.
//    Register         - Gang registers all commands in the m_lCommands list.
//    Unregister       - Gang unregisters all commands in the m_lCommands list.
//                       Note that this is done on destruction automatically
//                       for the currently bound interpreter.
//    AddProcessor     - Adds a command processor to the list
//                       ownership of the processor storage is not transferred.
//
//  Typically this class should be overridden, but not necessarily.
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "TCLCommandPackage.h"                               
#include <assert.h>


static const char* Copyright = 
"CTCLCommandPackage.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTCLCommandPackage

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Register (  )
//  Operation Type:
//     Connection.
//
void 
CTCLCommandPackage::Register() 
{
// Runs through the list of command processors in m_lCommands
// and:
//    1. Binds them to our current interpreter.
//    2. Registers them on the current interpreter.
//

  CommandListIterator i = begin();
  while(i != end()) {
    (*i)->Bind(getInterpreter());
    (*i)->Register();
    i++;
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Unregister (  )
//  Operation Type:
//     Disconnection
//
void 
CTCLCommandPackage::Unregister() 
{
// Runs through the list of commands in the
// the m_lCommands list and unregsters them from the
// interpreter to which they are bound.
// Exceptions:  

  CommandListIterator i = begin();
  while(i != end()) {
    (*i)->Unregister();
    i++;
  }

}

