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

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

//  CAttachCommand.cpp
// Implements the 'attach' TCL command.  This command 
// Connects SpecTcl to various data sources.
// It has the following formats:
//
//   attach  -file   filename  [size]
//   attach  -tape   devicename
//   attach  -pipe   [-size nwords]  command_string    
//   attach  -list
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//  Change Log:
//      July 14, 1999  Ron Fox
//           Replace online support with pipe support.
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

// Header Files:

#include <config.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <algorithm> // For case-insensitive string comparison.
#include <ctype.h>

#include <histotypes.h>
#include <Globals.h>
#include <File.h>

#include "AttachCommand.h"                               
#include "TCLInterpreter.h"
#include "TCLCommandPackage.h"
#include "TCLResult.h"
#include "DataSourcePackage.h"
#include "TCLAnalyzer.h"
#include "FilterBufferDecoder.h"
#include "NSCLBufferDecoder.h"
#include "FilterEventProcessor.h"
#include "SpecTcl.h"
#include "EventProcessor.h"

// For test data:
#include "GaussianDistribution.h"
#include "TestFile.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Class level members:

CAttachCommand::CDecoderFactory CAttachCommand::m_decoderFactory;

// Static data declarations:
struct SwitchDef {
  const char*     pName;
  CAttachCommand::Switch_t  Value;
};

static const SwitchDef SwitchTable[] = {
  {"-file", CAttachCommand::keFile},
  {"-tape", CAttachCommand::keTape},
  {"-pipe", CAttachCommand::kePipe},
  {"-size", CAttachCommand::keBufferSize},
  {"-format", CAttachCommand::keFormat},
  {"-test", CAttachCommand::keTest},
  {"-list", CAttachCommand::keList},
  {"-null", CAttachCommand::keNull}
};

typedef enum {
  eUnspecified,
  eFile,
  eTape,
  ePipe,
  eTest,
  eNull} SourceType;		// Possible source types:

struct OptionInfo {
  SourceType eSource;		// Source type.
  string     Format;		// Format selector string.
  long        nBytes;		// Blocksize.
  string     Connection;	// Connection string.
  OptionInfo() {		// Construct with default parameters.
    eSource = eUnspecified;
    Format  = "unchanged";
    nBytes  = knDefaultBufferSize;
    Connection = "";
  }
};

static const UInt_t nSwitches = sizeof(SwitchTable)/sizeof(SwitchDef);

// Functions for class CAttachCommand

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult,
//                     int nArgs, char* pArgs[] )
//  Operation Type:
//     Function operator.
//
/*!
   Parses and implements the attach TCL Command.
   
   Formal Parameters:
      \param  rInterp (in)
          Refers to the interpreter which is running this command.
      \param rResult (out)
          Refers to the result string associated with the interpreter.
      \param nArgs (in)
          Number of parameters on the command line.
      \param pArgs[] (in)
          Argument list for the command parameters.
       
   \return int one of:
   - TCL_OK  - If the function worked correctly.
   - TCL_ERROR - If not.
*/
int CAttachCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			       int nArgs, char* pArgs[])
{

  nArgs--;
  pArgs++;
  OptionInfo options;
  int        numSourceTypes(0);

  while(nArgs > 0) {
    Switch_t nextSw = ParseSwitch(*pArgs);
    switch (nextSw) {

      // Types of data sources... there must only be one so we
      // count and check this later:

    case keFile:
      numSourceTypes++;
      options.eSource = eFile;
      break;
    case keTape:
      numSourceTypes++;
      options.eSource = eTape;
      break;
    case kePipe:
      numSourceTypes++;
      options.eSource = ePipe;
      break;
    case keTest:
      numSourceTypes++;
      options.eSource = eTest;
      break;
    case keList:
      rResult = m_AttachedTo;
      return TCL_OK;
      break;
    case keNull:
      numSourceTypes++;
      options.eSource = eNull;
      break;

      // -size nBytes.

    case keBufferSize:
      nArgs--; pArgs++;
      if(nArgs <= 0) {
	rResult = "Not enough parameters: need a size for -size\n";
	Usage(rResult);
	return TCL_ERROR;
      }
      try {
	options.nBytes = rInterp.ExprLong(*pArgs);
	if(options.nBytes <= 0) throw string("nBytes < 0");
      }
      catch (...) {		// Invalid long expr.
	rResult = "Block size must be an unsigned nonzero integer\n";
	Usage(rResult);
	return TCL_ERROR;
      }
      break;

      // -format {formatname}

    case keFormat:
      nArgs--; pArgs++;
      if(nArgs <= 0) {
	rResult = "Not enough parameters: need a format type\n";
	Usage(rResult);
	return TCL_ERROR;
      }
      options.Format = *pArgs;
      break;

      // This must be the connection string.  Eat up all the
      // remaining parameters in to the connection string which is
      // blank separated.
      // 
    case keNotSwitch:
      options.Connection = ConcatenateParameters(nArgs, pArgs);
      nArgs = 0;
      break;
    default:			// Should not go here.
      assert(0);
      
    }
    nArgs--; pArgs++;
  }


  // Now the parameters are parsed correctly and the options struct,
  // together with numSourceTypes summarizes what we found.

  if(numSourceTypes != 1) {	// Require exactly one src type:
    rResult = "You must have exactly one data source type\n";
    Usage(rResult);
    return TCL_ERROR;
  }


  // Note well, since this mistake was already made once:
  // We can't delete the gpBufferDecoder prior to setting
  // a new one because setting the buffer decoder
  // calls the OnDetach method of the decoder.
  //
  //



  CBufferDecoder* oldDecoder = gpBufferDecoder;
  CBufferDecoder* pNewDecoder= createDecoder(options.Format);
  if (!pNewDecoder) 
  {
    rResult  = "Unrecognized format type: ";
    rResult += options.Format;
    rResult += "\n";
    Usage(rResult);
    return TCL_ERROR;

  }

  gpBufferDecoder = pNewDecoder;

  // Can only set a decoder if there's an analyzer
  // and we don't want to thrash decoders if it
  // turned out the logic above did not replace
  // the old one.
  //
  if(gpAnalyzer && (oldDecoder != gpBufferDecoder)) {
    gpAnalyzer->AttachDecoder(*gpBufferDecoder);
    delete oldDecoder; 
  }

  // It's now up to the individual attachers to figure out what's
  // going on.  I don't want to expose the structure of the
  // OptionInfo struct so I'm only passing pieces of it into the
  // various attachers:


  int status;
  switch(options.eSource) {
  case eFile:
    options.Connection = rInterp.TildeSubst(options.Connection);
    status =  AttachFile(rResult, options.Connection,
		      options.nBytes);
    break;
  case eTape:
    status = AttachTape(rResult, options.Connection,
		      options.nBytes);
    break;
  case ePipe:
    status = AttachPipe(rResult, options.Connection,
		      options.nBytes);
    break;
  case eTest:
    status = AttachTest(rResult, options.Connection,
		      options.nBytes);
    break;
  case eNull:
    status = AttachNull(rResult, options.Connection,
		      options.nBytes);
    break;
  case eUnspecified:		// Prior tests should have
  default:			// prevented these cases...
    assert(0);			// Let me know noisily if I'm wrong.
  }

  // Set the block/nonblock mode on the event source depending on what the
  // decoder needs:

  if(gpBufferDecoder->blockMode()) {
    gpEventSource->setFixedRecord();
  }
  else {
    gpEventSource->setVariableRecord();
  }


  // Iterate through the event processing pipeline, invoking the
  //  each event processor's OnEventSourceOpen function:


  SpecTcl* api = SpecTcl::getInstance();
  CTclAnalyzer::EventProcessorIterator p = api->ProcessingPipelineBegin();

  while (p != api->ProcessingPipelineEnd()) {
    CEventProcessor *pProcessor = p->second;
    if (!pProcessor->OnEventSourceOpen(m_AttachedTo)) break;
    p++;
  }
  return status;
}


/*!
   Attaches a file as SpecTcl's data source.
  
  
  
      \param rResult (out):
          Refers to the command interpreter result string.
      \param name (in):
          The filename after it has undergone a tilde expansion.
      \param nSize (in):
          Number of bytes per block in the file.
   \return int status (see operator()).
*/
int CAttachCommand::AttachFile(CTCLResult& rResult, 
			       const string& rName,
			       long  nBytes)
{


  // Now pFilename and nBlockSize are all set up for the open.
  // Try the attach, and if it works, then do the open:
  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  Int_t stat                = rPack.AttachFileSource(rResult);
  if(stat != TCL_OK)
    return stat;

  stat = rPack.OpenSource(rResult, rName.c_str(), nBytes);
  if(stat == TCL_OK) {
    m_AttachedTo  = "File: ";
    m_AttachedTo += rName;
  } 
  else {
    m_AttachedTo = "";
  }
  return stat;
}




/*!
    Attaches a tape data source to SpecTcl.
    Note that the attachment of tape data sources
    enables the tape commands.
      \param rResult (out):
          Refers to the command interpreter result string.
      \param name (in):
          The name of the device special file.
      \param nSize (in):
          Number of bytes per block in the file.
   \return int status (see operator()). 


*/
int CAttachCommand::AttachTape(CTCLResult& rResult, 
			       const string& rName,
			       long nSize)
{
  int stat;

  // This one is just an attach.  Opens are done using the tape -open 
  // command.

  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  stat =  rPack.AttachTapeSource(rResult, rName.c_str());

  if (stat == TCL_OK) {
    m_AttachedTo = "Tape: ";
    m_AttachedTo += rName;
  }
  else {
    m_AttachedTo = "";
  }

  return stat;
}
/*!
    Attaches a data source which comes through a pipe file.
    These are programs which generate data on the fly and pipe
    it to us.  For example, an online data source.
    SpecTcl histogrammer.  
      \param rResult (out):
          Refers to the command interpreter result string.
      \param name (in):
          The command to run on the other end of the pipe. This
	  command's stdout is used as the input to the pipe data
	  source.
      \param nSize (in):
          Number of bytes per block of data.
   \return int status (see operator()).   


*/
int CAttachCommand::AttachPipe(CTCLResult& rResult, 
			       const string& rName,
			       long          nBytes)
{
  std::string Command;

 
  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  int stat                  = rPack.AttachPipeSource(rResult);
  if(stat != TCL_OK) 
    return stat;

  stat = rPack.OpenSource(rResult, rName.c_str(), nBytes);
  if (stat == TCL_OK) {
    m_AttachedTo = "Pipe from: ";
    m_AttachedTo += rName;
  }
  else {
    m_AttachedTo = "";
  }
  return stat;
}

/*!
   Attaches a test data source.  The connection identifier is the
   name of a test data source that has been stored in the
   multi-event source.
      \param rResult (out):
          Refers to the command interpreter result string.
      \param name (in):
          The name of the test source to run.
      \param nSize (in):
          Number of bytes per block of data.
   \return int status (see operator()).   
*/
int CAttachCommand::AttachTest(CTCLResult& rResult,
			       const string& rName,
			       long  nBytes)
{
  UInt_t nBlockSize = nBytes;
  string sTestName = rName;
  
  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  int stat = rPack.AttachTestSource(rResult);
  if(stat != TCL_OK) {
    return stat;
  }

  stat = rPack.OpenSource(rResult, sTestName.c_str(), nBlockSize);
  if (stat == TCL_OK) {
    m_AttachedTo = "Test Source";
  }
  else {
    m_AttachedTo = "";
  }
  return stat;
}
/*!
   Attach the null data source.  The null data source is a data
   source that is always at end of file (no data source).
     \param rResult (out):
          Refers to the command interpreter result string.
      \param name (in):
          The name of the source (ignored).
      \param nSize (in):
          Number of bytes per block of data.
   \return int status (see operator()).      
*/
int CAttachCommand::AttachNull(CTCLResult& rResult,
			       const string& rName,
			       long nBytes)
{
  if(gpEventSource != (CFile*)kpNULL) {
    if(gpEventSource->getState() == kfsOpen) {
      gpEventSource->Close();
    }
    delete gpEventSource;
    gpEventSource = (CFile*)kpNULL;
  }
  m_AttachedTo = "Null";
  return TCL_OK;
}


/*!
   Register a new buffer decoder type to the attach command's decoder
   factory.
   \param type    - Name by which the type will be known.
                    If -format type is used, the registered creator
                    will be used to create the buffer decoder.
   \param creator - pointer to a creator that will be used to generate
                    buffer decoders for that type.

*/
void
CAttachCommand::addDecoderType(string type, CAttachCommand::CDecoderCreator* creator)
{
  m_decoderFactory.addCreator(type, creator);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void     Usage(CTCLResult& rResult)
//  Operation Type:
//     Protected utility.
//
void CAttachCommand::Usage(CTCLResult& rResult) {
  rResult += "Usage:\n";
  rResult += "  attach {switches...} connection\n";
  rResult += "  Where:\n";
  rResult += "     Switches are taken from the following:\n";
  rResult += "     -format {format type}\n";
  rResult += "     -size nBytes - Number of bytes per buffer.\n";
  rResult += "     {sourcetype} which can be only one of:\n";
  rResult += "        -file  when connection is the name of the file\n";
  rResult += "               from which data will be taken\n";
  rResult += "        -tape  When connection is the name of a tape device\n";
  rResult += "               special file and the tape command selects\n";
  rResult += "               files on the ansi labelled volume\n";
  rResult += "        -pipe  When connection is a command whose stdout is \n";
  rResult += "               taken as the event source.\n";
  rResult += "        -test  When connection selects one of the test data";
  rResult += "               sources\n";
  rResult += "        -null  No events will be made available.\n";

  vector<string> formats = m_decoderFactory.getDescriptions();

  rResult += "Available format types are:\n";
  for (int i =0; i < formats.size(); i++) {
    rResult += formats[i];
    rResult += "\n";
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    Switch_t ParseSwitch(char* pSwitch)
//  Operation Type:
//    Protected Utility.
//
CAttachCommand::Switch_t
CAttachCommand::ParseSwitch(char* pSwitch) {
  for(UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(SwitchTable[i].pName, pSwitch) == 0)
      return SwitchTable[i].Value;
  }
  return keNotSwitch;
}


/////////////////////////////////////////////////////////////////////////
//
// Function:
//    CBufferDecoder* createDecoder(std::string type)
// Operation Type:
//    private utility
// Description:
//    Given a format type string produces a pointer to the
//    decoder represented by that typ.
//    Returns NULL if type does not correspond to a 
//    known decoder type.
// 
//   Note that the special type "unchanged" will return the
//   value of gpBuferDecoder.
//
CBufferDecoder*
CAttachCommand::createDecoder(string type)
{
  if (type == string("unchanged")) {
    return gpBufferDecoder;
  }

  return m_decoderFactory.create(type);
}
