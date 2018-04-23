/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __PARAMETERCOMMAND_H  //Required for current class
#define __PARAMETERCOMMAND_H
                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif  

#ifndef __TCLCOMMANDPACKAGE_H
#include "TCLCommandPackage.h"
#endif

#include "Dictionary.h"
#include "Histogrammer.h"


// Forward class definitions.
//
class CTCLInterpreter;                             
class CTCLResult;
class CParameter;
                               
class CParameterCommand  : public CTCLPackagedCommand        
{
    
private:
    // Internal class/object that's used to dispatch -traces:
    
    class TraceDispatcher : public ParameterDictionaryObserver {
    private:
        CTCLInterpreter* m_pInterp;
        std::vector<std::string> m_scripts;
    
    public:
        TraceDispatcher(CTCLInterpreter* pInterp);
        void addTrace(const char* script);
        void removeTrace(const char* script);
        
        virtual void onAdd(std::string name, CParameter& p);
        virtual void onRemove(std::string name, CParameter& p);
    };
    
  // Datatypes:
public:
  enum SwitchValue_t {
    NewSw, 
    ListSw,
    DeleteSw,
    IdSw,
    ByIdSw,
    AddTrace,
    RmTrace,
    NotSwitch			// Should be last, ideally
  };
private:
    TraceDispatcher m_Observer;
public:

  CParameterCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage ); 
  virtual ~ CParameterCommand ( );
	
			//Copy constructor [illegal]
private:
  CParameterCommand (const CParameterCommand& aCParameterCommand );
public:

			//Operator= Assignment Operator [illegal]
private:
  CParameterCommand& operator= (const CParameterCommand& aCParameterCommand);
  int operator==(const CParameterCommand& rhs) const;
  int operator!=(const CParameterCommand& rhs) const;
public:

  //
  // Operations on the Object:
  //
public:
  virtual   int operator() (CTCLInterpreter& rInterp, 
			    CTCLResult& rResult, 
			    int nArguments, char* pArguments[])  ;

  // Subcommands:
  //
  UInt_t Create (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		 UInt_t nArg, char*  pArg[])  ;

  UInt_t List (CTCLInterpreter& rInterp, CTCLResult& rResult, 
	       UInt_t nPars, char* pPars[])  ;

  UInt_t Delete (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		 UInt_t nPars, Char_t* pPars[])  ;
  
  UInt_t addTrace(
        CTCLInterpreter& rInterp, CTCLResult& rResult, 
		 UInt_t nPars, Char_t* pPars[]
  );
  UInt_t removeTrace(
        CTCLInterpreter& rInterp, CTCLResult& rResult, 
		 UInt_t nPars, Char_t* pPars[]
  );

  //  Utility members:

protected:
  //
  // General parsing utilities:
  //
  void Usage (CTCLInterpreter& rInterp, CTCLResult& rResult)  ;
  SwitchValue_t ParseSwitch (const char* pSwitch)  ;

  // Utilities for producing Tcl lists for parameter definitions.

  UInt_t ListParametersById(CTCLResult& rResult, const char* pattern);
 
};

#endif
