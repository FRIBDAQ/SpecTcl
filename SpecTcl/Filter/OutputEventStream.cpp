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
  m_fXDRError(kfFALSE),
  //m_sFileName(ParseFileName(string(""))), // Use default file name.
  m_nValidParameters(0),
  m_pFile((FILE *)kpNULL),
  m_nBLOCKSIZE(8192),
  m_nMAXBUFFERSIZE(16) // Arbitrarily chosen.
{
  string sEmpty = "";
  m_sFileName = ParseFileName(sEmpty);

  m_vParameterNames.clear(); // I just hope that this is not a necessary action in C++.
  m_vBuffer.clear(); // I just hope that this is not a necessary action in C++.
  Open(); // Calls Close() first.
}

COutputEventStream::COutputEventStream(string& rFileName, vector<string>& rvParameterNames) :
  m_fActive(kfFALSE),
  m_fXDRError(kfFALSE),
  m_sFileName(ParseFileName(rFileName)),
  m_vParameterNames(rvParameterNames),
  m_nValidParameters(0),
  m_pFile((FILE *)kpNULL),
  m_nBLOCKSIZE(8192),
  m_nMAXBUFFERSIZE(16) // Arbitrarily chosen.
{
  m_vBuffer.clear(); // I just hope that this is not a necessary action in C++.
  Open(); // Calls Close() first.
}

COutputEventStream::~COutputEventStream() {
  Close();
}

// Operators.
Bool_t COutputEventStream::operator()(const string& rFileName) {
  m_sFileName = rFileName;
  return kfTRUE;
};

Bool_t COutputEventStream::operator()(CEvent& rEvent) {
  return ReceiveEvent(rEvent);
};

COutputEventStream& COutputEventStream::operator=(const COutputEventStream& rRhs) {
  return *this;
};

// Additional functions.
Bool_t COutputEventStream::isActive() {
  return m_fActive;
}

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
}

string COutputEventStream::getFileName() {
  return m_sFileName;
}

Bool_t COutputEventStream::Open() { // Always means that the file has been opened and the stream is active.
  Close(); // Clear everything.

  if(!m_fActive) {
    // Determine header information.
    // Ensure that parameters are present in dictionary before writing them.
    CParameter* pParameter = (CParameter*)kpNULL;
    //for(UInt_t i=0; i<m_vParameterNames.size(); i++) {
    for(vector<string>::iterator iParameterName = m_vParameterNames.begin(); iParameterName != m_vParameterNames.end(); iParameterName++) {
      pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(*iParameterName); // Parameter should be valid, but ...
      if(pParameter == (CParameter*)kpNULL) { // If parameter is not present in dictionary.
	cerr << "Error: Invalid parameter (" + *iParameterName + ").\n";
	m_vParameterNames.erase(iParameterName); // Remove the invalid parameter name. //**************************** Is iterator still valid???
      }
    }
    m_nValidParameters = m_vParameterNames.size();

    // Send header information. (Only valid parameters.)
    m_pFile = fopen(m_sFileName.c_str(), "w"); // Over-write any previously existing file.
    xdrstdio_create(&m_xdrs, m_pFile, XDR_ENCODE); // Create the XDR stream for encoding and WRITING TO the file stream.
    if(!xdr_u_int(&m_xdrs, &m_nValidParameters)) { // Send the number of valid parameters. (Should equal m_vParameterNames.size().)
      cerr << "Error: xdr_u_int unsuccessful.\n";
    }
    char *pParameterName;
    UInt_t nParameterId = 0;
    for(UInt_t i=0; i<m_vParameterNames.size(); i++) {
      if(!xdr_u_int(&m_xdrs, &i)) { // Send the parameter index.
	cerr << "Error: xdr_u_int unsuccessful.\n";
      }
      pParameterName = (char *)((m_vParameterNames[i]).c_str()); // Transform the parameter name string to a character string.
      if(!xdr_string(&m_xdrs, &pParameterName, 1024)) { // Send the parameter name.
	cerr << "Error: xdr_string unsuccessful.\n";
      }
      pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(m_vParameterNames[i]); // Find the parameter.
      nParameterId = pParameter->getNumber(); // Obtain the parameter ID.
      if(!xdr_u_int(&m_xdrs, &nParameterId)) { // Send the parameter ID.
	cerr << "Error: xdr_u_int unsuccessful.\n";
      }
    }

    m_fActive = kfTRUE;
  }
  return m_fActive;
}

Bool_t COutputEventStream::Close() { // Must be consecutively callable.
  if(m_pFile != (FILE *)kpNULL) {
    SendBuffer();
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
}

Bool_t COutputEventStream::ReceiveEvent(CEvent& rEvent) {
  if(m_fActive) {
    if(m_vBuffer.size() >= m_nMAXBUFFERSIZE) {
      if(!SendBuffer()) { // Send buffer. If code is an error, return false.
	cerr << "Error sending buffer.\n";
	return kfFALSE; // Failure sending buffer.
      }
    }
    m_vBuffer.push_back(&rEvent);
    return kfTRUE;
  } else {
    // ERROR! Trying to output an event to an inactive stream.
    cerr << "Error: Trying to output to inactive stream.\n";
    return kfFALSE;
  }
}

Bool_t COutputEventStream::SendBuffer() {
  /*
  if(m_ofs.is_open() && m_ofs.good() && m_vBuffer.size()>0) {
    // Send buffer.
    //for(int i=0; i<Buffer.size(); i++) { // For each and every CEvent in Buffer,
    for(vector<CEvent>::iterator i=m_vBuffer.begin(); i!=m_vBuffer.end(); i++) { // Just trying.
      //for(int j=0; j<((CEvent)Buffer[i]).size(); j++) { // For each and every Parameter in CEvent,
      for(CEventIterator j=i->begin(); j!=i->end(); j++) { // Just tryin'
	// Output the parameter in the event in the buffer.
	//m_ofs << ((CEvent)(Buffer[i]))[j] << " "; // Done with parameter.
	//m_ofs << (*j) << " "; // Done with parameter.
      }
      // Done with event.
      m_ofs << "EOE" << endl;
    }
    // Done with buffer.
    m_ofs << "EOB" << endl;
    m_ofs << flush;
    cout << flush;
    m_vBuffer.clear();
    return kfTRUE;
  } else {
    return kfFALSE;
  }
  */
  UInt_t True(1), False(0);
  string DOCUMENTATION = "DOC", EVENT = "EVT", ENDOFRECORD = "EOR"; // Tags.
  if(m_fActive) {
    // Send buffer.
    CParameter* pParameter = (CParameter*)kpNULL;
    UInt_t i = 0, nParameterId = 0;
    Float_t nParameterValue = 0;
    UInt_t *(ValidParameterArray[m_nValidParameters]); // Create this just once, and repeatedly assign to it.

    for(vector<CEvent*>::iterator ipEvent = m_vBuffer.begin(); ipEvent != m_vBuffer.end(); ipEvent++) { // For each event in buffer,
      // Find out the TRULY valid (based on isValid()) parameters and send the corresponding "bit-mask" array.
      i = 0;
      for(vector<string>::iterator iParameterName = m_vParameterNames.begin(); iParameterName != m_vParameterNames.end(); iParameterName++) { // For each parameter,
	pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(*iParameterName); // Find the parameter.
	nParameterId = pParameter->getNumber(); // Obtain the parameter ID.
	if(((**ipEvent)[nParameterId]).isValid()) {
	  ValidParameterArray[i] = &True;
	} else {
	  ValidParameterArray[i] = &False;
	}
	i++;
      }

      // Calculate event size.
      UInt_t nEventSize = 0;
      nEventSize = sizeof(ValidParameterArray) + (i * 4); // i is the number of parameters. Each parameter is a 32-bit (4-byte) float.

      if(m_nOffset >= m_nBLOCKSIZE) {
	m_nOffset %= m_nBLOCKSIZE;
	XDRfill(m_nBLOCKSIZE - m_nOffset);
      } else if((m_nOffset + nEventSize) > m_nBLOCKSIZE) {
	XDRfill(m_nBLOCKSIZE - m_nOffset); // Will exceed block size limits. Fill with null data instead and continue to next block.
      } else {
	if(!xdr_array(&m_xdrs, (char**)(&ValidParameterArray), &m_nValidParameters, 100, (UInt_t)sizeof(UInt_t), (xdrproc_t)xdr_u_int)) { // Send the bit-mask array.
	  cerr << "Error: xdr_array unsuccessful.\n";
	}

	// Send (only) the TRULY valid parameters.
	i = 0;
	for(vector<string>::iterator iParameterName = m_vParameterNames.begin(); iParameterName != m_vParameterNames.end(); iParameterName++) { // For each parameter,
	  if(*(ValidParameterArray[i]) == True) {
	    pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(*iParameterName); // Find the parameter.
	    nParameterId = pParameter->getNumber(); // Obtain the parameter ID.
	    nParameterValue = (**ipEvent)[nParameterId];
	    if(!xdr_float(&m_xdrs, &nParameterValue)) { // Send the parameter.
	      cerr << "Error: xdr_float unsuccessful.\n";
	    }
	  }
	  i++;
	} // Done with event.
	m_nOffset += nEventSize; // Increment the offset by the event size.
      }
    } // Done with buffer.

    m_vBuffer.clear();
  }

  return kfTRUE;
}

// The following XDR functions are intended to eliminate the need to repeat semantics.
// They can be used for either output (XDR_ENCODE) and input (XDR_DECODE) depending on the specified create operation.
Bool_t COutputEventStream::XDRstring(string& rString) {
  char* pString = (char*)(rString.c_str());
  if(xdr_string(&m_xdrs, &pString, (sizeof(pString)))) {
    m_nOffset += sizeof(rString);
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_string unsuccessful.\n";
    return kfFALSE;
  }
}

Bool_t COutputEventStream::XDRuint(UInt_t& rUInt) {
  if(xdr_u_int(&m_xdrs, &rUInt)) {
    m_nOffset += sizeof(rUInt);
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_u_int unsuccessfull.\n";
    return kfFALSE;
  }
}

Bool_t COutputEventStream::XDRarray(char** pValidParameterArray) {
  if(xdr_array(&m_xdrs, pValidParameterArray, &m_nValidParameters, 100, (UInt_t)sizeof(UInt_t), (xdrproc_t)xdr_u_int)) {
    m_nEvents++;
    m_nOffset += sizeof(*pValidParameterArray);
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_array unsuccessful.\n";
    return kfFALSE;
  }
}

Bool_t COutputEventStream::XDRfloat(Float_t& rFloat) {
  if(xdr_float(&m_xdrs, &rFloat)) {
    m_nOffset += sizeof(rFloat);
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_float unsuccessful.\n";
    return kfFALSE;
  }
}

Bool_t COutputEventStream::XDRfill(UInt_t nBytes) { // Pad with nBytes zeroes.
  // This function should only be used for WRITING out enough characters to pad the buffer.
  string String = "";
  for(UInt_t i = 0; i < nBytes; i++) {
    String += "0"; // Append nBytes zeroes.
  }
  char* pString;
  pString = (char*)(String.c_str());

  if(xdr_string(&m_xdrs, &pString, (sizeof(pString)))) {
    m_nOffset = 0;
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_string unsuccessful.\n";
    return kfFALSE;
  }

}
