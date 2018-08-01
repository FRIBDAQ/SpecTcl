/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

//  CAttachCommand.h:
//
//    This file defines the CAttachCommand class.
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
//  Change Log:
//      July 14, 1999 Ron Fox
//         Removed online connection stubs and replaced with 
//         pipe.
/////////////////////////////////////////////////////////////

#ifndef ATTACHCOMMAND_H //Required for current class
#define ATTACHCOMMAND_H

#include "TCLPackagedCommand.h"

// Forward class references:
class CTCLInterpreter;
class CTCLCommandPackage;
class CTCLResult;
class CTestData;

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __CCREATOR_H
#include <CCreator.h>
#endif

#ifndef __CEXTENSIBLEFACTORY_H
#include <CExtensibleFactory.h>
#endif


class CBufferDecoder;

// Class definition:
class CAttachCommand : public CTCLPackagedCommand {
  // Data types:
public:
  enum Switch_t { // The command switch set.
    keFile,
    keTape,
    kePipe,
    keBufferSize,
    keFormat,
    keTest,
    keList,
    keNull,
    keNotSwitch
  };
  typedef CCreator<CBufferDecoder>            CDecoderCreator;
  typedef CExtensibleFactory<CBufferDecoder>  CDecoderFactory;
private:
  std::string    m_AttachedTo;
  static CDecoderFactory m_decoderFactory;
  
public:
  //Constructor with arguments
  CAttachCommand(CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage) :
    CTCLPackagedCommand("attach", pInterp, rPackage),
    m_AttachedTo("Test Data Source")
  {}
  virtual ~CAttachCommand() {} //Destructor
	
  //Copy constructor [illegal]
private:
  CAttachCommand(const CAttachCommand& aCAttachCommand);
public:
  //Operator= Assignment Operator [illegal]
private:
  CAttachCommand& operator=(const CAttachCommand& aCAttachCommand);
  int operator==(const CAttachCommand& rhs) const;
  int operator!=(const CAttachCommand& rhs) const;
public:


  // Operations on the class:
public:
  virtual int operator()(CTCLInterpreter& rInterp,
			 CTCLResult& rResult,
			 int nArgs, char* pArgs[]);
  int AttachFile(CTCLResult& rResult, const std::string& Connection,
		 long nBytes);
  int AttachTape(CTCLResult& rResult,  const std::string& Connection,
		 long nBytes);
  int AttachPipe(CTCLResult& rResult,  const std::string& Connection,
		 long nBytes);
  int AttachTest(CTCLResult& rResult,  const std::string& Connection,
		 long nBytes);
  int AttachNull(CTCLResult& rResult,  const std::string& Connection,
		 long nBytes);

  static void addDecoderType(std::string type, CDecoderCreator* creator);
protected:
  static void Usage(CTCLResult& rResult);
  static Switch_t ParseSwitch(char* pSwitch);
private:
  static CBufferDecoder* createDecoder(std::string type);

};

#endif
