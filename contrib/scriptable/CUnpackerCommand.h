
#ifndef __CUNPACKERCOMMAND_H  //Required for current class
#define __CUNPACKERCOMMAND_H

#ifndef __TCLPROCESSOR_H
#include <TCLProcessor.h>
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

// Forward class definitions:

class CModuleDictionary;	//Set known modules.
class CUnpacker;			// Unpackers.
 
/*!
This class implements the unpacker command.  The unpacker command
configures the unpacker object by adding and removing modules to it.
The available modules are held in a module dictionary.  The unpacker can
also list the set of modules that are currently inserted in the unpacker in
unpacker order.

*/
class CUnpackerCommand : public CTCLProcessor     
{
private:
  CModuleDictionary& m_rModules;
  CUnpacker&           m_rUnpacker;
  
public:
	// Constructors and other canonical functions.
	
  CUnpackerCommand (const string&          rCommand, 
				CTCLInterpreter&    rInterp,
				CModuleDictionary& rDictionary,
				CUnpacker&           rUnpacker);
  virtual ~CUnpackerCommand ( );
private:
  CUnpackerCommand (const CUnpackerCommand& aCUnpackerCommand );
  CUnpackerCommand& operator= (const CUnpackerCommand& aCUnpackerCommand);
  int operator== (const CUnpackerCommand& aCUnpackerCommand) const;
  int operator!= (const CUnpackerCommand& rhs) const;
public:

	// Selectors:
public:
    CModuleDictionary& getModules()  {
	   return m_rModules;
    }
    CUnpacker& getUnpacker()  {
	return m_rUnpacker;
    }

  // Class operations:

public:

    virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
				       int  argc, char** argv)   ; // 
    int AddUnpacker (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			    int argc, char** argv)   ; // 
    int RemoveUnpacker (CTCLInterpreter& rInterp, CTCLResult& rResult, 
				  int argc, char** argv)   ; // 
    int ListUnpackers (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			     int argc, char** argv)   ; // 
    string Usage();

};

#endif
