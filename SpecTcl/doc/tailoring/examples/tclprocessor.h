//  CTCLProcessor.h:
//
//    This file defines the CTCLProcessor class.
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

#ifndef __TCLPROCESSOR_H  //Required for current class
#define __TCLPROCESSOR_H
                               //Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif                               

#ifndef __TCLINTERPRETER_H
#include "TCLInterpreter.h"
#endif

#ifndef __TCLRESULT_H
#include "TCLResult.h"
#endif
  
#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif
                             
typedef vector<CTCLInterpreter*> TCLInterpreterList;
typedef TCLInterpreterList::iterator TCLInterpreterIterator;

class CTCLProcessor  : public CTCLInterpreterObject        
{

  std::string m_sCommandName;                     // Name of the command.
  TCLInterpreterList m_vRegisteredOn;  // Set of interpreters 
				                  // we've been registered to.
public:
  
  //Constructors with arguments

  CTCLProcessor(const std::string& sCommand, CTCLInterpreter* pInterp);
  CTCLProcessor(const char* pCommand, CTCLInterpreter* pInterp);

  ~ CTCLProcessor ( ) {
    UnregisterAll();
  }
  	
			//Copy constructor [ illegal ]
private:
  CTCLProcessor (const CTCLProcessor& aCTCLProcessor );
public:
			//Operator= Assignment Operator [ illegal ]
private:
  CTCLProcessor& operator= (const CTCLProcessor& aCTCLProcessor);
public:

			//Operator== Equality Operator
                        // Legal, but pretty useless

  int operator== (const CTCLProcessor& aCTCLProcessor)
  { 
    return (
	    (CTCLInterpreterObject::operator== (aCTCLProcessor)) &&
	    (m_sCommandName == aCTCLProcessor.m_sCommandName) &&
	    (m_vRegisteredOn == aCTCLProcessor.m_vRegisteredOn) 
	    );
  }                             
  // Selectors:

  std::string getCommandName() const
  {
    return m_sCommandName;
  }
  TCLInterpreterIterator begin() { 
    return m_vRegisteredOn.begin();
  }
  TCLInterpreterIterator end() {
    return m_vRegisteredOn.end();
  }
  // Mutators:
protected:

  void setCommandName (const std::string& am_sCommandName)
  { m_sCommandName = am_sCommandName;
  }
  void setRegisteredOn (const std::vector<CTCLInterpreter*>& am_vRegisteredOn)
  { m_vRegisteredOn = am_vRegisteredOn;
  }
  // Operations and overrides:

public:
  
  virtual   int operator() (CTCLInterpreter& rInterpreter, 
			    CTCLResult& rResult, 
			    int nArguments, 
			    char* pArguments[])   = 0;
  
  static  std::string ConcatenateParameters (int nArguments, 
					      char* pArguments[])  ;
  static  int EvalRelay (ClientData pData,
			 Tcl_Interp* pInterp, 
			 int Argc, char *Argv[])  ;
  virtual   void OnDelete ( )  ;
  static void DeleteRelay (ClientData pObject)   ;

  int ParseInt (const char* pString, int* pInteger)  ;
  int ParseInt (const std::string& rString, int* pInteger) {
    return ParseInt(rString.c_str(), pInteger);
  }

  int ParseDouble (const char* pString, double* pDouble)  ;
  int ParseDouble (const std::string& rString, double* pDouble) {
    return ParseDouble(rString.c_str(), pDouble);
  }

  int ParseBoolean (const char* pString, Bool_t* pBoolean)  ;
  int ParseBoolean (const std::string& rString, Bool_t* pBoolean) {
    return ParseBoolean(rString.c_str(), pBoolean);
  }

  void Register ()  ;
  int Unregister ()  ;
  void UnregisterAll ()  ;

};

#endif
