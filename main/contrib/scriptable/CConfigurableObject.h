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

Base class of all objects that have a TCL configurable
 configuration. The configuration object autonomously processes the
config an cget subcommands to maintain a configuration parameter 
database.  Configuration consists of a set of configuration parameter 
objects.

Each of these represents a keyword/value pair. 

*/

// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef CCONFIGURABLEOBJECT_H  //Required for current class
#define CCONFIGURABLEOBJECT_H

//
// Include files:
//

#include <TCLProcessor.h>
#include <TCLResult.h>        //Required for include files  
#include <list>
#include <string>


// forward definitions. 

class CConfigurationParameter;
class CTCLInterpreter;
class CTCLResult;
class CBoolConfigParam;
class CIntConfigParam;
class CIntArrayParam;
class CStringConfigParam;
class CStringArrayparam;

class CConfigurableObject : public  CTCLProcessor     
{
  // Public data types.
public:
  typedef std::list<CConfigurationParameter*> ConfigArray;
  typedef ConfigArray::iterator          ParameterIterator;
private:
  
  std::string          m_sName;	//!< Name of the command associated with the object.
  ConfigArray     m_Configuration; //!< The configuration.


  // Constructors and other canonical operations.
public:
  CConfigurableObject (const std::string& rName,
		       CTCLInterpreter& rInterp);
  virtual  ~ CConfigurableObject ( );  

  // The copy like operations are not supported on tcl command processing
  // objects:
private:
  CConfigurableObject (const CConfigurableObject& aCConfigurableObject );
  CConfigurableObject& operator= (const CConfigurableObject& aCConfigurableObject);
  int operator== (const CConfigurableObject& aCConfigurableObject) const;
public:

  // Selectors:

  //!  Retrieve a copy of the name:

  std::string getName() const
  { 
    return m_sName;
  }   



  // Member functions:

public:

  virtual  int      operator() (CTCLInterpreter& rInterp, 
				CTCLResult& rResult, 
				int nArgs, char** pArgs)   ; //!< Process commands.
  virtual  int      Configure (CTCLInterpreter& rInterp, 
			       CTCLResult& rResult, 
			       int nArgs, char** pArgs)   ; //!< config subcommand 
  virtual  int      ListConfiguration (CTCLInterpreter& rInterp, 
				       CTCLResult& rResult, 
				       int nArgs, char** pArgs); //!< list subcommand 
  ParameterIterator AddParameter(CConfigurationParameter*);
  ParameterIterator AddIntParam (const std::string& sParamName, 
				 int nDefault=0)   ; //!< Create an int.
  ParameterIterator AddBoolParam (const std::string& rName,
				  bool          fDefault)   ; //!< Create a boolean. 
  ParameterIterator AddStringParam (const std::string& rName)   ; //!< Create std::string param. 
  ParameterIterator AddIntArrayParam (const std::string&  rParameterName, 
				      int nArraySize, 
				      int nDefault=0)   ; //!< Create array of ints.
  ParameterIterator AddStringArrayParam (const std::string& rName, 
					 int nArraySize)   ; //!< Create array of std::strings.
  ParameterIterator Find (const std::string& rKeyword)   ; //!< Find a param 
  ParameterIterator begin ()   ; //!< Config param start iterator.
  ParameterIterator end ()   ;   //!< Config param end iterator.
  int size ()   ;                //!< Config param number of items.
  std::string ListParameters (const std::string& rPattern)   ; //!< List configuration 
  std::string ListKeywords ()   ;     //!< List keyword/type pairs.

protected:
  virtual std::string Usage();
private:
  void              DeleteParameters ()   ; //!< Delete all parameters. 
  
};

#endif
