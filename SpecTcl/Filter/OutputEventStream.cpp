/*
  OutputEventStream.cpp
*/

// Copyright Notice.
static const char* Copyright =
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header Files.
#include <vector>
#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <rpc/xdr.h> // rpc.h includes xdr.h

#include "Globals.h"
#include "Histogrammer.h"
#include "Event.h"
#include "EventList.h"

#include "OutputEventStream.h"

// Constructors.
COutputEventStream::COutputEventStream() : // Should no longer be called due to the implementation and use of EventFilter::ParseFileName which sets the default file name, as well as the lack of a vector of parameter names.
  m_fActive(kfFALSE),
  //m_sFileName(ParseFileName(string(""))), // Use default file name.
  m_pFile((FILE*)kpNULL),
  //m_xdrs
  m_fXDRError(kfFALSE),

  //m_vParameterNames
  m_nValidParameters(0),
  m_pBitMask((char*)kpNULL),

  m_nBUFFERSIZE(8192),
  m_pBuffer((char*)kpNULL),
  m_nOffset(0),

  m_nEvents(0),
  m_nEventSize(0),
  m_nBytesSent(0)
{
  m_pBuffer = (char*) new char[m_nBUFFERSIZE];

  string sEmpty = "";
  m_sFileName = ParseFileName(sEmpty);

  m_vParameterNames.clear(); // I just hope that this is not a necessary action in C++.
  Open(); // Calls Close() first.
};

COutputEventStream::COutputEventStream(string& rFileName, vector<string>& rvParameterNames) :
  m_fActive(kfFALSE),
  m_sFileName(ParseFileName(rFileName)), // Use default file name.
  m_pFile((FILE*)kpNULL),
  //m_xdrs
  m_fXDRError(kfFALSE),

  m_vParameterNames(rvParameterNames),
  m_nValidParameters(0),
  m_pBitMask((char*)kpNULL),

  m_nBUFFERSIZE(8192),
  m_pBuffer((char*)kpNULL),
  m_nOffset(0),

  m_nEvents(0),
  m_nEventSize(0),
  m_nBytesSent(0)
{
  Open(); // Calls Close() first.
};

COutputEventStream::~COutputEventStream() {
  Close();
};

// Operators.
Bool_t COutputEventStream::operator()(const string& rFileName) {
  m_sFileName = rFileName;
  return kfTRUE;
};

Bool_t COutputEventStream::operator()(CEvent& rEvent) {
  // Send the event to the buffer.
  CParameter* pParameter = (CParameter*)kpNULL;
  UInt_t nXDRPosition = 0, nParameterId = 0;
  string sTag = "", sBitMask = "";
  Float_t nParameterValue = 0;

  if(m_fActive) {
    // First ensure that there is enough space for the event.
    nXDRPosition = xdr_getpos(&m_xdrs);
    if(nXDRPosition > m_nBUFFERSIZE) {
      cerr << "An overflow error MIGHT have occurred. Check this. nXDRPosition = " << nXDRPosition << "." << endl;
      return kfFALSE;
    }
    m_nOffset = nXDRPosition % m_nBUFFERSIZE; // Calculate offset from the beginning of this buffer.
    if((m_nBUFFERSIZE - (m_nOffset + m_nEventSize)) < 200) { // The 200 is just an educated guess to give a nice padding and help ensure all goes well.
      if(m_nEvents < 1) { // We couldn't even send a single one??? DAMN!!!
	cerr << "Error: Buffer size (" << m_nBUFFERSIZE << ") too small for event size (" << m_nEventSize << "). FATAL!" << endl;
	return kfFALSE;
      }
      // Unsatisfactory amount of space left for what you will use for event. Send an endofrecord immediately and write the buffer out.
      sTag = "endofrecord";
      XDRstring(sTag);
      if(!WriteBuffer()) {
	return kfFALSE;
      }
    }

    // Start sending event:
    sTag = "event";
    XDRstring(sTag);
    cerr << "Event beginning..." << endl;
    m_nEvents++;
    cerr << " Event Number: " << m_nEvents << endl;
    cerr << " Offset: " << m_nOffset << ", XDR Position: " << nXDRPosition << "." << endl;

    // Find out the TRULY valid (based on isValid()) parameters.
    // Set and send the corresponding "bit-mask" array.
    for(UInt_t i = 0; i < m_nValidParameters; i++) {
      pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(m_vParameterNames[i]); // Find the parameter.
      nParameterId = pParameter->getNumber(); // Obtain the parameter ID.
      if((rEvent[nParameterId]).isValid()) {
	m_pBitMask[i] = (char)'1';
      } else {
	m_pBitMask[i] = (char)'0';
      }
    }
    sBitMask = string(m_pBitMask);
    XDRstring(sBitMask); // Send the bit mask.

    // Send (only) the TRULY valid parameters.
    for(UInt_t i = 0; i < m_nValidParameters; i++) { // For each parameter,
      if(m_pBitMask[i] == '1') {
	pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(m_vParameterNames[i]); // Find the parameter.
	nParameterId = pParameter->getNumber(); // Obtain the parameter ID.
	nParameterValue = (rEvent)[nParameterId];
	XDRfloat(nParameterValue); // Send the parameter.
      }
    }
    cerr << "... End of event." << endl << endl;
    return (!m_fXDRError);
  } else {
    // ERROR! Trying to output an event to an inactive stream.
    cerr << "Error: Trying to output to inactive stream.\n";
    return kfFALSE;
  }
  return kfFALSE;
};

COutputEventStream& COutputEventStream::operator=(const COutputEventStream& rRhs) {
  return *this;
};

// Additional functions.
Bool_t COutputEventStream::isActive() {
  return m_fActive;
};

string COutputEventStream::ParseFileName(string& rFileName) {
  // Implemented here, as opposed to EventFilter, so as to use a uniform default setting.
  /*
    Not currently implemented in OutputEventStream due to potential conflict with respect to setting the default file name when none is specified.
    This should not pose any problems as the user only sets file names with respect to the filter (which then calls the output event stream), and this is taken care of right here.
  */
  char* pHomeDir = getenv("HOME");
  string sHomeDir = "", sFileName = "";

  if(pHomeDir) {
    sHomeDir = string(pHomeDir);
  }

  if(rFileName == "") { // If empty,
    sFileName = sHomeDir + "/filteroutput.ftr"; // use this as the default (for both EventFilter and OutputEventStream).
  } else if(rFileName[0] == '~') { // Else if it begins with a ~,
    sFileName = sHomeDir + rFileName.substr(1, (rFileName.length()-1)); // interpret the ~ as the home directory.
  } else { // Else,
    sFileName = rFileName; // Use what was given originally.
  }

  return sFileName;
};

string COutputEventStream::getFileName() {
  return m_sFileName;
};

Bool_t COutputEventStream::Open() { // Always means that the file has been opened and the stream is active.
  string sTag = "header";
  string sParameterName = "";
  UInt_t nParameterId = 0;

  Close(); // Clear everything.

  if(!m_fActive) {
    // Determine header information.
    // Ensure that parameters are present in dictionary before writing them.
    CParameter* pParameter = (CParameter*)kpNULL;
    for(vector<string>::iterator iParameterName = m_vParameterNames.begin(); iParameterName != m_vParameterNames.end(); iParameterName++) {
      pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(*iParameterName); // Parameter should be valid, but ...
      if(pParameter == (CParameter*)kpNULL) { // If parameter is not present in dictionary.
	cerr << "Error: Invalid parameter (" + *iParameterName + "). Erasing from parameter vector.\n";
	m_vParameterNames.erase(iParameterName); // Remove the invalid parameter name. //**************************** Is iterator still valid???
      }
    }
    m_nValidParameters = m_vParameterNames.size();
    //UInt_t *(ValidParameterArray[m_nValidParameters]);
    if(m_pBitMask != (char*)kpNULL) {
      delete[] m_pBitMask; // AVOID MEMORY LEAKS! Use delete[].
    }
    m_pBitMask = (char*) new char[m_nValidParameters + 1]; // Add 1 for the null terminator.
    m_pBitMask[m_nValidParameters] = (char)kpNULL; // Set null terminator right now.

    // Create the XDR file stream.
    m_pFile = fopen(m_sFileName.c_str(), "wb"); // Over-write any previously existing file.
    if(m_pFile == (FILE*)kpNULL) {
      cerr << "Error: Could not open output file (" << m_sFileName << ")." << endl;
      return kfFALSE;
    }
    m_pBuffer = (char*) new char[m_nBUFFERSIZE];
    //xdrstdio_create(&m_xdrs, m_pFile, XDR_ENCODE); // Create the XDR stream for encoding and WRITING TO the file stream.
    xdrmem_create(&m_xdrs, (char*)m_pBuffer, m_nBUFFERSIZE, XDR_ENCODE); // READ from memory buffer.

    // Send header information. (Only valid parameters.)
    XDRstring(sTag); // Send the tag.
    cerr << "Header beginning..." << endl;
    XDRuint(m_nValidParameters); // Send the number of valid parameters. (Should equal m_vParameterNames.size().)
    cerr << " Number of valid parameters: " << m_nValidParameters << endl;
    // Calculate event size.
    m_nEventSize = strlen(sTag.c_str()) + 1 + // Null terminator added.
      strlen(m_pBitMask) + 1 + // Null terminator added.
      (m_nValidParameters * 4); // Each parameter is (at least) a 32-bit float.
    cerr << " Event size: " << m_nEventSize << endl;
    for(UInt_t i = 0; i < m_nValidParameters; i++) {
      cerr << " Parameter:" << endl;
      XDRuint(i); // Send the parameter index.
      cerr << "  Index: " << i << endl;
      sParameterName = m_vParameterNames[i];
      XDRstring(sParameterName); // Send the parameter name.
      cerr << "  Name: " << sParameterName << endl;
      pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(m_vParameterNames[i]); // Find the parameter.
      nParameterId = pParameter->getNumber(); // Obtain the parameter ID.
      XDRuint(nParameterId); // Send the parameter ID.
      cerr << "  Id: " << nParameterId << endl;
    }
    cerr << "... End of header." << endl << endl;
    m_fActive = kfTRUE;
  }
  return m_fActive;
};

Bool_t COutputEventStream::Close() { // Must be consecutively callable.
  if(m_pBuffer != (char*)kpNULL) {
    delete[] m_pBuffer;
  }
  m_pBuffer = (char*)kpNULL;

  if(m_pFile != (FILE *)kpNULL) {
    fflush(m_pFile);
    xdr_destroy(&m_xdrs);
    clearerr(m_pFile);
    if(fclose(m_pFile) != 0) {
      cerr << "Error: Could not close file.\n";
      clearerr(m_pFile);
      fclose(m_pFile); // Desperation!
    }
  }
  m_pFile = (FILE *)kpNULL;
  m_fActive = kfFALSE;
  return !m_fActive; // Always true which always means success with me, but this is arbitrary.
};

// The following XDR functions are intended to eliminate the need to repeat semantics.
// They can be used for either output (XDR_ENCODE) and input (XDR_DECODE) depending on the specified create operation.
Bool_t COutputEventStream::XDRstring(string& rString) {
  char* pString = (char*)(rString.c_str());
  if(xdr_string(&m_xdrs, &pString, (strlen(pString) + 1))) { // Add 1 to include the null terminator.
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: COutputEventStream::XDRstring unsuccessful.\n";
    return kfFALSE;
  }
};

Bool_t COutputEventStream::XDRuint(UInt_t& rUInt) {
  if(xdr_u_int(&m_xdrs, &rUInt)) {
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: COutputEventStream::XDRuint unsuccessfull.\n";
    return kfFALSE;
  }
};

Bool_t COutputEventStream::XDRarray(char** pValidParameterArray) {
  if(xdr_array(&m_xdrs, pValidParameterArray, &m_nValidParameters, 100, (UInt_t)sizeof(UInt_t), (xdrproc_t)xdr_u_int)) {
    m_nEvents++;
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: COutputEventStream::XDRarray unsuccessful.\n";
    return kfFALSE;
  }
};

Bool_t COutputEventStream::XDRfloat(Float_t& rFloat) {
  if(xdr_float(&m_xdrs, &rFloat)) {
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: COutputEventStream::XDRfloat unsuccessful.\n";
    return kfFALSE;
  }
};

Bool_t COutputEventStream::WriteBuffer() {
  m_nBytesSent = fwrite(m_pBuffer, sizeof(char), m_nBUFFERSIZE, m_pFile);
  cerr << "Number of bytes written: " << m_nBytesSent << "." << endl;
  m_nOffset = 0;
  if(!xdr_setpos(&m_xdrs, 0)) {
    cerr << "Error: XDR stream could not be repositioned. FATAL!" << endl;
    m_fXDRError = kfTRUE;
    return kfFALSE;
  }
  return kfTRUE;
}
