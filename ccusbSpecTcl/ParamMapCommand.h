#ifndef __PARAMMAPCOMMAND_H
#define __PARAMMAPCOMMAND_H

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

#ifndef __TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif




/*!
   The parammap command provides a mechanism to specify a mapping
   between the paramters from a hardware module and a SpecTcl named parameter.
   For the CC-USB, each module has a position in the readout data and
   a vector of parameters associated with it (a vector whose elements
   are themselves vectors of parameter ids).

   In order to be sure that we know the parameter ids we will be creating
   the parameters ourselves as tree parameters.

   Command syntax is:
\verbatim
   parammap -add    module_number  module-type module-id [list name1 ...]
   parammap -delete module_number
   parammap -list   ?module_number?

\endverbatim

   Note this is a modified singleton class, however there's an explicit creator
   in addition to the getInstance().   This is because creation requires
   a TCL interpreter, which most getInstance() callers won't know.
   the create() member will throw an exception if invoked with the
   object already created (could be the constructor, but this is more
   explicit, and prevents accidental destruction).

*/
class CParamMapCommand : public CTCLObjectProcessor
{
  // exported types:
public:


  typedef struct _ParameterMap {
    int                  s_moduleType;
    int                  s_id;
    std::vector<int>     s_parameterIds;
  } ParameterMap;

  typedef std::vector<ParameterMap>  ModuleMap;

  // Internal data:
private:
  static CParamMapCommand*    m_pTheInstance;
  ModuleMap                   m_modules;

  // Constructors are private to enforce the singleton-ness
  // of the class and remove copy/assignment/comparison operators.

private:
  CParamMapCommand(CTCLInterpreter& interp, std::string command);
  CParamMapCommand(const CParamMapCommand& rhs);
  CParamMapCommand& operator=(const CParamMapCommand& rhs);
  int operator==(const CParamMapCommand& rhs);
  int operator!=(const CParamMapCommand& rhs);

public:
  // Destruction is ok, but  will require recreation.
  virtual ~CParamMapCommand();	// Probably required to prevent undefined globals.

  // singleton methods; create throws if the object already exists.

  static void create(CTCLInterpreter& interp, std::string command=std::string("parammap"));
  static CParamMapCommand* getInstance();

  //  Command processors.
public:
  virtual int operator()(CTCLInterpreter& interp,
			 std::vector<CTCLObject>& objv);
  
private:
  int add(CTCLInterpreter& interp,
	  std::vector<CTCLObject>& objv);
  int remove(CTCLInterpreter& interp,
	     std::vector<CTCLObject>& objv);
  int list(CTCLInterpreter& interp,
	   std::vector<CTCLObject>& objv);

  // Data accessors.
public:

//  std::vector<int>* getModuleMap(unsigned int moduleNumber);
//  std::vector<ParameterMap>* getModuleMap(unsigned int moduleNumber);
  ParameterMap* getModuleMap(unsigned int moduleNumber);
  size_t            getMapSize() const;

  // Utilities:
private:
  CTCLObject listModule(unsigned int moduleNumber);
  std::string Usage() const;

};

#endif
