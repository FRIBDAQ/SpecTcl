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


static const char* Copyright = "(C) Copyright Michigan State University 2005, All rights reserved";
//  CTestFile.cpp
// Encapsulates a source of test data.
// Test data consists of data buffers which
// are blocked up to contain fixed sized events
// containing a set of gaussian distributed parameters.
//  The parameter distributions and sizes are set
//  by Adding "CDistribution objects.

//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

// Header Files:
#include <config.h>
#include "TestFile.h"                               
#include "ErrnoException.h"
#include "RangeError.h"
#include <errno.h>
#include <buffer.h>
#include <buftypes.h>
#include <MultiTestSource.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



// Functions for class CTestFile

/*!
  Destroying a test source means also asking the multitestsource
  to remove it from the list of test sources (if it's there). .. this
  all still looks rather hokey...why do we need a MultiTestSource in the
  first place eh?
*/
CTestFile::~CTestFile()
{
  CMultiTestSource::GetInstance()->destroyingTestSource(this);
}


//////////////////////////////////////////////////////////////////////////
//
// Function:
//   int operator==(const CTestFile& aCTestFile) 
// 
// Operation Type:
//    equality comparison.
Int_t CTestFile::operator==(const CTestFile& aCTestFile) {
  // Compares *this to aCTestFile.  Equality is defined as:
  // vectors are equal, and base class elements are also equal
  //
  return ((CFile::operator==(aCTestFile))   &&
	  (m_vDistributions == aCTestFile.m_vDistributions)
	  );
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t& Read ( Address_t pBuffer, UInt_t nSize )
//  Operation Type:
//     "I/O" transfer
//
Int_t CTestFile::Read(Address_t pBuffer, UInt_t nSize) {
  // Produces an NSCL buffer header and all
  // containing test data.  The test data consists
  //  of fixed length events which are randomly
  // distributed according to parameter distributions
  // which are attached to this "file".
  //
  // Formal Parameters:
  //       Address_t pBuffer:
  //             Pointer to the buffer to fill.
  //       UInt_t nBytes:
  //             Number of bytes to put in the buffer.
  // Returns:
  //   Number of bytes of data read into the buffer. 
  // NOTE:
  //     The buffer created is of NSCL DAQ format with a header
  //     and all that.  The run number is always 1, the sequence
  //     number is always zero however. 
  //     If no distributions are defined, then an 'empty' buffer
  //     is returned.
  // Exceptions:
  //    CErrnoException  - If file was not open.
  //    CRangeError  - If buffer was too small to hold the header
  //                       and at least one event.
  if(getState() != kfsOpen) {
    errno = EBADF;
    CErrnoException e("CTestFile::Read() - File is not in open state");
    throw e;
  }
  if(nSize < (NSCLHeaderSize() + EventSize())) {
    CRangeError re(0, nSize, NSCLHeaderSize() + EventSize(),
		   "CTestFile::Read() - buffer too small for minimal data");
    throw re;
  }
  // The buffer is formatted as follows;
  // first the header is put in, then as many events as will fit.
  // finally, the buffersize, and event count are set back into the header.
  //
  UChar_t* p = (UChar_t*)pBuffer;
  UInt_t  nEvents = 0;
  UInt_t  nBytes  = 0;
  UInt_t  n;

  // Put the header in:
  n       = FormatNSCLHeader(p);
  p      += n; 
  nBytes += n;  

  // Fill the buffer with events (generic).
  while((nBytes + EventSize()) < nSize) { // Another event fits:
    n       = FormatEvent(p);
    p      += n;
    nBytes += n;  
    nEvents++;
  }

  SetEventCount(pBuffer, nEvents);
  SetBufferSize(pBuffer, nBytes);
  return nBytes;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t Write ( const Address_t pBuffer, UInt_t nBytes )
//  Operation Type:
//     "I/O" Transfer.
//
Int_t CTestFile::Write(const Address_t pBuffer, UInt_t nBytes) {
  // Emulates a write to the file.  In this case, the
  // operation is a no-op.
  //
  //  Formal Parameters:
  //       const Address_t pBuffer:
  //             points to the data to write.
  //       UInt_t  nBytes:
  //             Number of data bytes to write.
  //  Returns:
  //      nBytes.
  // Exceptions:
  //     If the file is not open, then an exception is thrown.
  //
  if(getState() != kfsOpen) {
    errno = EBADF;
    CErrnoException e("CTestFile::Write - Write attempted on unopened file");
    throw e;
  }

  return nBytes;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Open ( const std::string& rsFilename, UInt_t nAccess )
//  Operation Type:
//     I/O connection
//
void CTestFile::Open(const std::string& rsFilename, UInt_t nAccess, bool unused) {
  // opens . so that the file has an fd associated with it.
  //
  //  Formal Parameters:
  //      std::string& rsFilename:
  //           Name of the file to open (ignored).
  //      UInt_t nAccess:
  //            Access requested (ignored).
  // 
  CFile::Open(std::string("."), nAccess);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Close (  )
//  Operation Type:
//     I/O disconnect.
//
void CTestFile::Close() {
  // Sets m_eState to kfsClosed
  //
  // Exceptions:  

  CFile::Close();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t GetDistributionCount (  )
//  Operation Type:
//     Selector.
//
UInt_t CTestFile::GetDistributionCount() const {
  // Returns the number of distributions which are
  // currently in the simulator.
  // Exceptions:  

  return m_vDistributions.size();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void AddDistribution ( const CDistribution& rDistribution )
//  Operation Type:
//     Mutator
//
void CTestFile::AddDistribution(CDistribution& rDistribution) {
  // Adds a distribution to the event source.
  // 
  // Formal Parameters:
  //     const CDistribution& rDistribution:
  //          Reference to the distribution.  Note that
  //          the distribution must have a copy constructor
  //          defined.
  //
  m_vDistributions.push_back(&rDistribution);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void ClearDistribution ( UInt_t nDistribution )
//  Operation Type:
//     mutator.
//
void CTestFile::ClearDistribution(UInt_t nDistribution) {
  // Eliminates the selected distribution from the list.  If the distribution
  //  is not the last one, all the elements following the eliminated one
  // are moved forward.
  //
  // Formal Parameters:
  //      UInt_t nDist:
  //          Index of the distribution to eliminate.
  // Exceptions:
  //          CRangeError  if nDist is an invalid
  //          index.
  if(nDistribution >= m_vDistributions.size()) { // Throw range error.
    CRangeError re(0, m_vDistributions.size()-1, nDistribution,
		   "CTestFile::ClearDistribution - Distribution # too big");
    throw re;
  }
  CDistributionIterator p = begin();
  for(UInt_t i = 0; i <= nDistribution; i++) 
    p++;
  m_vDistributions.erase(p);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void ClearDistributions()
//  Operation Type:
//     mutator
//
void CTestFile::ClearDistributions() {
  // Clears all of the distributions from the 
  // event distribution list.
  // Exceptions:  

  while(m_vDistributions.size()) 
    m_vDistributions.pop_back();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CDistributionIterator begin (  )
//  Operation Type:
//     selector
//
CDistributionIterator CTestFile::begin() {
  // Returns an in iterator to the beginning of
  // the event distribution list.  Iterators are
  // pointer like objects with the following operations
  // defined:
  //    operator*    - Dereference
  //    operator++ - 'Point' to next object.
  //    operator--   - 'Point' to prior object.
  //    operator== - Test for iterator equality.
  //    operator!=  - Test for iterator inequality.
  //    operator=   - Assignment.
  //
  // Exceptions:  

  return m_vDistributions.begin();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CDistributionIterator end (  )
//  Operation Type:
//     selector.
//
CDistributionIterator CTestFile::end() {
  // Returns an iterator which can be used to determine
  // if the end of the distribution list has been reached yet.
  // used in contexts like:
  // 
  //  for(CDistributionListIterator i = testfile.begin(); i != testfile.end(); i++) 
  //    {   ... }

  // Exceptions:  

  return m_vDistributions.end();
}

////////////////////////////////////////////////////////////////////////
//
// FUnction:
//    void DoAssign(const CTestFile& rhs)
// Operation type:
//   Assignment support.
//
void CTestFile::DoAssign(const CTestFile& arhs) {
  CTestFile& rhs((CTestFile&)arhs);
  CFile::DoAssign(rhs);
  ClearDistributions();
  for(CDistributionIterator p = rhs.begin(); p != rhs.end(); p++) {
    CDistribution* pDist(*p);
    AddDistribution(*pDist);
  }
}

/////////////////////////////////////////////////////////////////////////
//
// Function:
//    UInt_t FormatNSCLHeader(Address_t pBuffer)
//  Operation Type:
//    Utility.
UInt_t CTestFile::FormatNSCLHeader(Address_t pBuffer) {
  // Formats an NSCL buffer header into the buffer passed in.
  //  the buffer must be big enough to handle the header.
  // 
  // returns the number of bytes put in the buffer.

  BHEADER* pHeader = (BHEADER*)pBuffer;
  
  pHeader->nwds = 0;		// Filled in later.
  pHeader->type = DATABF;	// Data buffer.
  pHeader->cks  = 0;		// Don't emulate checksum.
  pHeader->run  = 1;		// Always run # 1.
  pHeader->seq  = 0;		// Always buffer number 0.
  pHeader->nevt = 0;		// Filled in later.
  pHeader->nlam = 0;		// No lam masks.
  pHeader->cpu  = 1;		// CPU # 1.
  pHeader->nbit = 0;		// No bit registers.
  pHeader->buffmt=4;		// Buffer revision level.
  pHeader->ssignature = 0x0102; // Word byte order signature.
  pHeader->lsignature = 0x01020304l; // Longword order signature.

  return sizeof(BHEADER);
}

////////////////////////////////////////////////////////////////////////
//
// Function:
//    UInt_t FormatEvent(Address_t pBuffer)
// Operation Type:
///   Utility
UInt_t CTestFile::FormatEvent(Address_t pBuffer) {
  //  Formats an event into an NSCL buffer starting at the
  //  spot which is passed in.
  //  The buffer must be big enough to hold the event.
  // 
  // returns:
  //    The number of bytes put in the buffer.
  //
  Short_t* pW = (Short_t*)pBuffer;
  *pW++ = EventSize()/sizeof(UShort_t);
  for(CDistributionIterator p = begin(); p != end(); p++) {
    *pW++ = (Short_t)((**p)());
  }
  return EventSize();
}

/////////////////////////////////////////////////////////////////////
//
// Function:
//   UInt_t EventSize() const
// Operation Type:
//   Utility
//
UInt_t CTestFile::EventSize() const {
  // An event consists of a word count followed by a word for each
  // distribution:
  //
  return ((1 + GetDistributionCount()) * sizeof(UShort_t));
}

//////////////////////////////////////////////////////////////////////
//
// Function:
//    SetEventCount(Address_t pBuffer, UInt_t nEvents)
// Operation Type:
//    Utility:
//
void CTestFile::SetEventCount(Address_t pBuffer, UInt_t nEvents) {
  // Assumes that pBuffer points to an NSCL buffer header and fills
  // in the event count field of that buffer.
  //
  // Formal Parameters:
  //   Address_t pBuffer:
  //     Pointer to a buffer header.
  //  UInt_t nEvents:
  //     Number of events to put in the event count field.

  BHEADER* ph = (BHEADER*)pBuffer;

  ph->nevt = nEvents;
}

/////////////////////////////////////////////////////////////////////
//
// Function:
//    void SetBufferSize(Address_t pBuffer, UInt_t nBytes)
// Operation Type:
//    Utility function.
void CTestFile::SetBufferSize(Address_t pBuffer, UInt_t nBytes) {
  // Assumes that pBuffer points to an NSCL buffer header and
  // fills in the word count field of that header.
  UShort_t* p = (UShort_t*)pBuffer;
  *p = nBytes/sizeof(UShort_t);
}
