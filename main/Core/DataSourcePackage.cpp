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

//  CDataSourcePackage.cpp:
//    Encapsulates the commands and services required to 
//    attach SpecTcl to external data sources.
//    Currently this package supports File and
//    pipe data sources.
// 
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
// Change log:
//     July 14, 1999 Ron Fox
//         Remove online data source stuff, replacing it with Pipe data sources
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

// Header Files:
#include <config.h>
#include "DataSourcePackage.h"                               
#include "AttachCommand.h"
#include "RingFormatCommand.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"
#include "TKRunControl.h"
#include "DiskFile.h"
#include "PipeFile.h"
#include "MultiTestSource.h"
#include "Globals.h"
#include "Exception.h"


#include <SpecTcl.h>

#include <NSCLBufferDecoder.h>
#include <NSCLJumboBufferDecoder.h>
#include <CRingBufferDecoder.h>
#include <FilterBufferDecoder.h>


#include <string>
#include <tcl.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/////////////////////////////////////////////////////////////////////////
//
// Code in this section  define  buffer decoders creators
// used to register the default set of buffer decoders
// for the attach -format switch.
//

class CNSCLDecoderCreator : public CAttachCommand::CDecoderCreator
{
public:
  virtual CBufferDecoder* operator()() {
    return new CNSCLBufferDecoder();
  }
  virtual string          describe() const {
    return string("nscl - NSCL 'standard' buffer format decoder'");
  }
};

class CJumboDecoderCreator : public CAttachCommand::CDecoderCreator
{
public:
  virtual CBufferDecoder* operator()() {
    return new CNSCLJumboBufferDecoder();
  }
  virtual string          describe() const {
    return string("jumbo - NSCL 'standard' buffer format with jumbo sized buffers");
  }
};


class CFilterDecoderCreator : public CAttachCommand::CDecoderCreator
{
public:
  virtual CBufferDecoder* operator()() {
    return new CFilterBufferDecoder();
  }
  virtual string          describe() const {
    return string("filter - SpecTcl filter format files.");
  }
};


class CRingDecoderCreator : public CAttachCommand::CDecoderCreator
{
public:
  virtual CBufferDecoder* operator()() {
    return new CRingBufferDecoder();
  }
  virtual string    describe() const {
    return string("ring  - NSCL DAQ Ring buffer data acquisition system");
  }
};

///////////////////////////////////////////////////////////////////////////

// Functions for class CDataSourcePackage

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     CDataSourcePackage (CTCLInterpreter* pInterp)
//  Operation Type:
//     Constructor
//
CDataSourcePackage::CDataSourcePackage(CTCLInterpreter* pInterp) :
  CTCLCommandPackage(pInterp, Copyright),
  m_eSourceType(CDataSourcePackage::kTestSource),

  m_pAttach(new CAttachCommand(pInterp, *this)),
  m_pRingFormat(new CRingFormatCommand(pInterp, *this))
  
{

  AddProcessor(m_pAttach);
  AddProcessor(m_pRingFormat);
  

  // install the buffer decoder creators for the default set
  // of buffer format types:

  SpecTcl* pApi = SpecTcl::getInstance();
  pApi->addBufferDecoder("nscl", new CNSCLDecoderCreator());
  pApi->addBufferDecoder("jumbo", new CJumboDecoderCreator());
  pApi->addBufferDecoder("filter",new CFilterDecoderCreator());
  pApi->addBufferDecoder("ring",  new CRingDecoderCreator());
  
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int AttachFileSource ( CTCLResult& rResult )
//  Operation Type:
//     Connection
//
int CDataSourcePackage::AttachFileSource(CTCLResult& rResult) {
  // Attaches SpecTcl to a file data source.
  // File data sources are diskfiles which contain
  // event data.
  //
  // Prior to calling this, analysis must be halted.
  //
  // Formal Parameters:
  //       CTCLResult& rResult:
  //           Refers to the result string which should be returned.
  //           If successful, this contains file
  //           If not, contains the failure reason.
  // 
  // Returns:
  //     TCL_OK         - Success
  //     TCL_ERROR - Failure.
  if(gpRunControl->getRunning()) { // Analysis is active...
    rResult = "Analysis is active and must first be stopped";
    return TCL_ERROR;
  }

  // Disconnect the current event source and connect the new one.

  
  CFile* pNew   = new CDiskFile();
  gpEventSource = pNew;
  CFile* pOld   = gpRunControl->Attach(pNew);
  delete pOld;

  // The new data source type is a file, the result string and
  // return code should now show success:
  m_eSourceType = CDataSourcePackage::kFileSource;
  rResult = "file:";
  return TCL_OK;
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int AttachPipeSource ( CTCLResult& rResult )
//  Operation Type:
//     attachment
//
int CDataSourcePackage::AttachPipeSource(CTCLResult& rResult) {
  // Connects the SpecTcl analysis engine to 
  // an pipe data source.
  //     
  // Formal Parameters:
  //     CTCLResult& rResult:
  //             The TCL result string.  Returns either:
  //                   pipe on success or the reason for
  //                   a failure.
  // Returns:
  //      TCL_OK         - Success.
  //      TCL_ERROR - Failure.
  // Exceptions:
  if(gpRunControl->getRunning()) { // Analysis is active...
    rResult = "Analysis is active and must first be stopped";
    return TCL_ERROR;
  }

  // Disconnect the current event source and connect the new one.
  CFile* pNew   = new CPipeFile();
  gpEventSource = pNew;
  CFile* pOld   = gpRunControl->Attach(pNew);
  delete pOld;

  // The new data source type is a file, the result string and
  // return code should now show success:
  m_eSourceType = CDataSourcePackage::kPipeSource;
  rResult = "pipe:";
  return TCL_OK;
}

int CDataSourcePackage::AttachTestSource(CTCLResult& rResult) {
  if(gpRunControl->getRunning()) { // Analysis is active...
    rResult = "Analysis is active and must first be stopped";
    return TCL_ERROR;
  }

  // Disconnect the current event source and connect the new one.
  CMultiTestSource* pMultiTestSource = CMultiTestSource::GetInstance();
  CFile* pNewEventSource = pMultiTestSource->getDefaultTestSource();
  gpEventSource = pNewEventSource;
  CFile* pOldEventSource = gpRunControl->Attach(pNewEventSource);
  if (pOldEventSource != pNewEventSource) { // Re-attaching test source.
    delete pOldEventSource;
  }

  // The new data source type is a file, the result string and
  // return code should now show success:
  m_eSourceType = CDataSourcePackage::kTestSource;
  rResult = "test:";
  return TCL_OK;
}

int CDataSourcePackage::AttachNullSource(CTCLResult& rResult) {
  if(gpRunControl->getRunning()) { // Analysis is active...
    rResult = "Analysis is active and must first be stopped";
    return TCL_ERROR;
  }

  // Disconnect the current event source and connect the new one.
  /*
  CFile* pNewEventSource = new CTestData;
  gpEventSource = pNewEventSource;
  CFile* pOldEventSource = gpRunControl->Attach(pNewEventSource);
  delete pOldEventSource;
  */
  if(gpEventSource != (CFile*)kpNULL) {
    delete gpEventSource;
    gpEventSource = (CFile*)kpNULL;
  }

  // The new data source type is a file, the result string and
  // return code should now show success:
  m_eSourceType = CDataSourcePackage::kNullSource;
  rResult = "null:";
  return TCL_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int OpenSource ( CTCLResult& rResult, const char*  pConnectionString, 
//                     UInt_t nBufferSize )
//  Operation Type:
//     Source connection
//
int CDataSourcePackage::OpenSource(CTCLResult& rResult, 
			       const char*  pConnectionString, 
			       UInt_t nBufferSize) 
{
  // Connects a data source to a concrete object.
  // What this means depends on the actual data source type:
  //
  //  file   - Connects the data source to a file in the filesystem.
  //  pipe   - Creates a child data source process with stdout pointing
  //           at the pipe file and connects the data source to that pipe.
  //
  // Formal Parameters:
  //     CTCLResult&  rResult:
  //             Result string which will either contain:
  //                  sourcetype:connectionstring   - success.
  //     const char* pConnectionString:
  //             Specifies the connection string.  For file
  //              this is the name of an event file.
  //              For pipe data sources, this is the program which is run.
  //     UInt_t   nBufferSize:
  //              Specifies the size of the data buffer to use.
  //              
  // Returns:
  //     TCL_OK        - Success
  //     TCL_ERROR- Failure.

  // Data Analysis must already be halted:
  if(gpRunControl->getRunning()) { // Analysis is active...
    rResult = "Analysis is active and must first be stopped";
    return TCL_ERROR;
  }

  // If necessary, close the event source prior to opening as specified.
  try {
    if(gpEventSource->getState() == kfsOpen) 
      gpEventSource->Close();
    gpEventSource->Open(pConnectionString, kacRead,
			gpBufferDecoder->blockMode());
    
    // Now the event record size is set from the parameter.
    // Two assumptions are made:
    //   1. The Event source type matches that of our m_eSourceType
    //   2. The run control is derived from CTKRunControl
    CTKRunControl* pSource = (CTKRunControl*)gpRunControl;
    
  }
  catch (CException& rExcept) {
    rResult = rExcept.ReasonText();
    return TCL_ERROR;
  }

  // If control passes here, the open is successful, construct the
  // result string and return TCL_OK:
  switch(m_eSourceType) {
  case kFileSource:
    rResult = "file:";
    break;
  case kPipeSource:
    rResult = "pipe:";
    break;
  case kTestSource:
    rResult = "test:";
    break;
  case kNullSource:
    rResult = "null:";
    break;
  default:
    rResult = "-Unknown-:";
    break;
  }
  rResult += pConnectionString;
  return TCL_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int CloseSource ( CTCLResult& rResult )
//  Operation Type:
//     source connection
//
int CDataSourcePackage::CloseSource(CTCLResult& rResult) {
  // Closes the current data source.  The type
  // remains the same, but the source is not associated
  // with anything.  It is illegal to start/stop runs.
  //
  // Formal Parameters:
  //     CTCLResult& rResult:
  //           Either empty or a failure reason.
  // Returns:
  //          TCL_OK         - Success.
  //          TCL_ERROR - Failure

  // The data analysis must be halted:
  if(gpRunControl->getRunning()) { // Analysis is active...
    rResult = "Analysis is active and must first be stopped";
    return TCL_ERROR;
  }

  // Only necessary to close if the data source is in fact open:
  if(gpEventSource->getState() == kfsOpen) 
    gpEventSource->Close();

  return TCL_OK;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t isOpen (  )
//  Operation Type:
//     Selector.
//
Bool_t CDataSourcePackage::isOpen() {
  // kTRUE if the event source is currently open.
  return gpEventSource->getState() == kfsOpen;
}
