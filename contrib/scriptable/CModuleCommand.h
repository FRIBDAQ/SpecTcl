#ifndef __CMODULECOMMAND_H  //Required for current class
#define __CMODULECOMMAND_H


//
// Include files:
//
#ifndef __CTCLPROCESSOR_H
#include <TCLProcessor.h>
#endif

#ifndef __STL_MAP
#include <map>
#define __STL_MAP
#endif

#ifndef __STL_STRING
#include <string>
#endif

// Forward classes:

class CModuleDictionary;
class CUnpacker;
class CModuleCreator;
class CTCLInterpreter;
class CTCLResult;
 
/*!
Implements commands to create instances of 
module unpackers.  These instances are put into 
a module dictionary from which they can be added
to the experiment's unpacker.  The module command
uses the matcher/creator pattern to allow the set
of modules that can be created to be extended without
adding code to the module command class itself.
The command has the following subcommands:
- create - creates a new module.
- list     - lists the modules that have been created.
- types  - lists the module types that can be created.
- delete - Destroys a module that has been created (removing
	 it from the unpacker if it has been installed.
*/
class CModuleCommand : public CTCLProcessor     
{
public:
  typedef map<string,CModuleCreator*> CreatorCollection;
  typedef CreatorCollection::iterator        CreatorIterator;
private:
    CreatorCollection 	m_Creators;     //!< Creator objects
    CUnpacker&	   	m_rUnpacker;  //!< Pointer to unpacker object.
    CModuleDictionary&  m_rModules;   //!< Pointer to module dictionary.


public:
    // Constructor and other canonical functions:
    
    CModuleCommand (CTCLInterpreter& rInterp, 
			      const string&        rCommand,
			      CUnpacker&           rUnpacker,
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
    CUnpacker& getUnpacker() 
    {
	return m_rUnpacker;
    }
    CModuleDictionary& getDictionary() 
    {
	return m_rModules;
    }
    
    // Mutators: Note that references can't be modified so once construted, we're
    // stuck with  the unpacker and module dictionary we started with.
    
protected:
    void setCreators(const CreatorCollection& rCollection) 
    {
	m_Creators = rCollection;
    }

     // Class operations:

public:

    virtual   int operator() (CTCLInterpreter&  rInterp, 
					CTCLResult& rResult, 
					int argc, char** pArgv)   ; 
    int Create (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		    int argc, char** pArgv)   ; 
    int Destroy (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		      int argc, char** pArgv)   ; 
    int List (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		int argc, char** pArgv)   ; 
    string Usage ()   ; 
    virtual   int ListTypes (CTCLInterpreter& rInterp, 
				    CTCLResult& rResult, 
				    int argc, char** argv)   ; 
    void RegisterCreator(const string& rType, CModuleCreator& rCreator);

};

#endif
