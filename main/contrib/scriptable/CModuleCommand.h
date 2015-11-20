#ifndef __CMODULECOMMAND_H  //Required for current class
#define __CMODULECOMMAND_H

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
  \class CModuleCommand
  \file  CModuleCommand.cpp
Implements commands to create instances of 
module unpackers.  These instances are put into 
a module dictionary from which they can be added
to the experiment's unpacker.  The module command
uses the matcher/creator pattern to allow the set
of modules that can be created to be extended without
adding code to the module command class itself.
The command has the following switches
- -create - creates a new module.
- -list     - lists the modules that have been created.
- -types  - lists the module types that can be created.
- -delete - Destroys a module that has been created (removing
	 it from the unpacker if it has been installed.
*/

//
// Include files:
//
#ifndef __CTCLPROCESSOR_H
#include <TCLProcessor.h>
#endif

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// Forward classes:

class CModuleDictionary;
class CUnpacker;
class CModuleCreator;
class CTCLInterpreter;
class CTCLResult;
 

class CModuleCommand : public CTCLProcessor     
{
public:
  typedef std::map<std::string,CModuleCreator*> CreatorCollection;
  typedef CreatorCollection::iterator CreatorIterator;
private:
    CreatorCollection 	m_Creators;     //!< Creator objects
    CModuleDictionary&  m_rModules;   //!< Pointer to module dictionary.


public:
    // Constructor and other canonical functions:
    
    CModuleCommand (CTCLInterpreter&   rInterp, 
		    const std::string&      rCommand,
		    CModuleDictionary& rDictionary);
    virtual  ~ CModuleCommand ( ); 

	// Commands don't copy, assign or compare.
private:
   CModuleCommand (const CModuleCommand& aCModuleCommand );
   CModuleCommand& operator= (const CModuleCommand& aCModuleCommand);
   int operator== (const CModuleCommand& aCModuleCommand) const;
   int operator!= (const CModuleCommand& rhs) const;
public:

    // Selectors:

public:
    CreatorCollection getCreator() const 
    {
	return m_Creators;
    }

    CModuleDictionary* getDictionary() 
    {
	return &m_rModules;
    }
    
     // Class operations:

public:

    virtual   int 
        operator() (CTCLInterpreter&  rInterp, 
		    CTCLResult& rResult, 
		    int argc, char** pArgv); 
    int Create (CTCLInterpreter& rInterp, 
		CTCLResult& rResult, 
		int argc, char** pArgv)   ; 
    int Destroy (CTCLInterpreter& rInterp, 
		 CTCLResult& rResult, 
		 int argc, char** pArgv)   ; 
    int List (CTCLInterpreter& rInterp, 
	      CTCLResult& rResult, 
	      int argc, char** pArgv)   ; 
    std::string Usage ()   ; 
    virtual   int
           ListTypes (CTCLInterpreter& rInterp, 
		      CTCLResult& rResult, 
		      int argc, char** argv)   ; 
    void RegisterCreator(const std::string& rType, 
			 CModuleCreator& rCreator);

};

#endif
