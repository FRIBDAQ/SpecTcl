// Typically this class should be overridden, but not necessarily.
//  CTCLCommandPackage.h:
//
//    This file defines the CTCLCommandPackage class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __TCLCOMMANDPACKAGE_H  //Required for current class
#define __TCLCOMMANDPACKAGE_H
                               //Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif                               
                               
#ifndef __TCLINTERPRETER_H
#include "TCLInterpreter.h"
#endif
                               //Required for 1:M associated classes
#ifndef __TCLPROCESSOR_H
#include "TCLProcessor.h"
#endif                                                               

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif


#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif


typedef std::list<CTCLProcessor*>   CommandList;
typedef CommandList::iterator  CommandListIterator;

class CTCLCommandPackage  : public CTCLInterpreterObject        
{
  std::string m_sSignon;    // Package signon message.  
  CommandList m_lCommands;  // List of references to command objects
                            // which implement package.
  
public:
  // Constructors with parameters:
  //
  CTCLCommandPackage (CTCLInterpreter* pInterp, 
		      const std::string& rSignon=std::string("Unnamed pkg")) :
    CTCLInterpreterObject(pInterp),
    m_sSignon(rSignon)
  {}
  CTCLCommandPackage(CTCLInterpreter* pInterp,
		     const char* pSignon = "Unnamed pkg") :
    CTCLInterpreterObject(pInterp),
    m_sSignon(pSignon)
  {}
  virtual ~ CTCLCommandPackage ( ) { /* Unregster(); */ }       //Destructor

			//Copy constructor

  CTCLCommandPackage (const CTCLCommandPackage& aCTCLCommandPackage )   : 
    CTCLInterpreterObject (aCTCLCommandPackage),
    m_sSignon(aCTCLCommandPackage.m_sSignon),
    m_lCommands(aCTCLCommandPackage.m_lCommands)
  {}                                     

			//Operator= Assignment Operator

  CTCLCommandPackage& operator= (const CTCLCommandPackage& aCTCLCommandPackage)
  { 
    if (this == &aCTCLCommandPackage) return *this;          
    CTCLInterpreterObject::operator= (aCTCLCommandPackage);
    m_sSignon   = aCTCLCommandPackage.m_sSignon;
    m_lCommands = aCTCLCommandPackage.m_lCommands;
        
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLCommandPackage& aCTCLCommandPackage)
  { return (
	    (CTCLInterpreterObject::operator== (aCTCLCommandPackage)) &&
	    (m_sSignon   == aCTCLCommandPackage.m_sSignon)              &&
	    (m_lCommands == aCTCLCommandPackage.m_lCommands)
	    );
  }                             
  // Selectors:

public:
  std::string getSignon() const
  {
    return m_sSignon;
  }
  CommandList getCommandList() const
  {
    return m_lCommands;
  }
  // Mutators:

protected:                    
  void setSignon (std::string am_sSignon)
  { 
    m_sSignon = am_sSignon;
  }
  void setCommandList(CommandList& rList)
  {
    m_lCommands = rList;
  }
  // Operations:

public:                       
  void Register ()  ; 
  void Unregister ()  ;

  // inline functions:

  void AddProcessor (CTCLProcessor* pProcessor) {
    m_lCommands.push_back(pProcessor);
  }
  void AddProcessors(CommandList& rList) 
  {
    m_lCommands.insert(end(), rList.begin(), rList.end());
  }
  CommandListIterator begin () {
    return m_lCommands.begin();
  }
  CommandListIterator end () {
    return m_lCommands.end();
  }

};

#endif
