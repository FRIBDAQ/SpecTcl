

//  CFile.cpp
// Encapsulates any entity which can
// be expressed as a file descriptor object.
//   
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
// BUGBUGBUG - 
//   Cygwin seems to have problems with network accessed files:
//   Every now and then we get a partial read and corrupted buffer.
//   The workaround for now is to throttle the read to something smaller
//   than a typical buffer.
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "File.h" 
#include "ErrnoException.h"
#include <errno.h>   
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>                           
#include <iostream.h>
#include <stdio.h>
#ifdef CYGWIN
#include <windows.h>
#endif

static const char* Copyright = 
"CFile.cpp: Copyright 1999 NSCL, All rights reserved\n";


static UInt_t nBuffers(0);
static UInt_t nReads(0);

// Functions for class CFile

//////////////////////////////////////////////////////////////////////////
//
// Function: 
//     CFile(const CFile& rFile)
// Operation Type:
//    Copy Constructor.
//    
CFile::CFile(const CFile& rFile)
{
	DoAssign(rFile);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//     CFile& operator=(const CFile& rFile)
// Operation Type:
//     Assignment Operator:
//
CFile&
CFile::operator=(const CFile& rFile)
{
	if(this != &rFile)
		DoAssign(rFile);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int Read ( Address_t pBuffer, UInt_t nBytes )
//  Operation Type:
//     I/O transfer
//
Int_t
CFile::Read(Address_t pBuffer, UInt_t nBytes) 
{
//  Reads from the file descriptor.
//  The default action is to use the read()
//  system function.  
//
// Formal Parameters:
//       Address_t pBuffer:
//            Points to the data buffer which is
//            open on the file.
//      UInt_t       nBytes:
//            Number of bytes to read.
// Returns:
//     0    - End of file hit on read.
//     n    - Number of bytes read.
// Throws:
//       CErrnoException, note that if file state
//       is not kfsOpen this is mapped into
//       CErrnoException with errno = EBADF.
//       (Bad file descriptor).


  // The I/O system buffering etc. may not let us do the read in one full gulp:

  UInt_t nTotalRead(0);
  errno = 0;
  while(nTotalRead != nBytes) {
    UInt_t n     = (nBytes - nTotalRead);
    UInt_t nRead = read(getFd(), pBuffer, n);
    nReads++;
    if(nRead > 0) {
      nTotalRead += nRead;
      char* p     = (char*)pBuffer;
      p          += nRead;
      pBuffer     = (Address_t)p;
    }
    else if (nRead == 0) {
      break;			// Next read gives the end file cond.
    }
    else {			// Note in theory this is not possible.
      throw CErrnoException("CPipeFile::Read - failed CFile::Read");
    }
  }
  nBuffers++;
  return nTotalRead;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t& Write ( const Address_t pBuffer, UInt_t nBytes )
//  Operation Type:
//     I/O transfer
//
Int_t 
CFile::Write(const Address_t pBuffer, UInt_t nBytes) 
{
// Writes a buffer of data to the File.
//  By default, this function executes the
//  write() system call.
// 
// Formal Parameters:
//      const Address_t pBuffer:
//         Pointer to the buffer to be written.
//      Uint_t nBytes:
//         Number of bytes to be written.
//
// Returns:
//    0   - End of file.
//    n   - Number of words which could be written.
//
// Throws:
//    CErrnoException if there are errors in errno.
//    Note that if the file state is not Open, this
//    is mapped into a CErrnoException with
//    value EBADF (Bad file descriptor).
//

	
  AssertOpen();

  // Now try the write:
  
  ssize_t nWritten =  write(m_nFd, pBuffer, nBytes);
  if(nWritten < 0) {
    CErrnoException except("CFile::Write - write() system call failed");
    throw except;
  }
  return nWritten;
}
//////////////////////////////////////////////////////////////////////////
// 
// Function:
//   void Open(UInt_t nFd)
// Operation Type:
//    I/O connection.
//
void
CFile::Open(UInt_t nFd)
{
	// Connects the file object to an already open file
	// descriptor.
	//
	// Formal Parameters:
	//    UInt_t nFd:
	//        File descriptor already open.
	//

	if(m_eState == kfsOpen) {           // Close this file if open.
		close(m_nFd);
	}
	m_nFd    = nFd;
	m_eState = kfsOpen;                 // nFd is presumably open.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Open ( std::string& rsFilename, UInt_t nAccess )
//  Operation Type:
//     I/O connection
//
void 
CFile::Open(const std::string& rsFilename, UInt_t nAccess) 
{
// Connects the file object to a file.
// If successful, the file state is set to 
// Open.  If not, an exception is thrown.
//
// By default, the open() system service is
// called.
//
// Formal Parameters:
//       std::string&    sFilename:
//           Name of file to open.
//       UInt_t    nAccess:
//          Bitwise or of the values:
//              kacRead          Read access
//              kacWrite         Write access
//              kacCreate        Create if needed.
//              kacAppend        Append before writes.
//
// Throws:
//         CErrnoException if the open fails.
//  NOTES:
//  1.
//        Files created are created with permissions
//        rw-r--r--
//   2.
//        If the file is already open, it is closed first.
//
  int nReadWrite = kacRead | kacWrite;
  
  if(m_eState == kfsOpen) {
    Close();
  }
  // Figure out the Access bits.
  
  int oflags = 0;
#ifdef CYGWIN
  oflags |= O_BINARY;
#endif
  oflags |= (nAccess & kacCreate) ? O_CREAT   : 0;
  oflags |= (nAccess & kacAppend) ? O_APPEND  : 0;
  if( (nAccess & nReadWrite) == nReadWrite) {
    oflags |= O_RDWR;
  }
  else {
    oflags |= (nAccess & kacRead) ? O_RDONLY : 0;
    oflags |= (nAccess & kacWrite)? O_WRONLY : 0;
  }
  // Now we should be able to open the file:
  
  int nFd= open(rsFilename.c_str() , oflags);
  if(nFd < 0) {
    CErrnoException exception("CFile::Open - open() system service failed");
    throw exception;
  }
  else {
    m_eState = kfsOpen;
    m_nFd = (UInt_t)nFd;
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Close (  )
//  Operation Type:
//     I/O disconnect.
//
void 
CFile::Close() 
{
// Closes the file and invalidates the file descriptor.
//  Subsequent reads and writes will fail.
// 
// Throws:
//    CErrnoException if the close fails, or
//    m_eState != kfsOpen on entry.
//       (in this case, the code is EBADF).
//

  AssertOpen();
  
  close(m_nFd);
  m_eState = kfsClosed;
  
	  
}
//////////////////////////////////////////////////////////////////////////////
//
// Function:
//    void DoAssign(const CFile& rFile)
// Operation Type:
//     Assignment utility
//
void
CFile::DoAssign(const CFile& rFile)
{
	m_eState = rFile.m_eState;
	if(m_eState == kfsOpen) {
		m_nFd = dup(rFile.m_nFd);
	}
}
///////////////////////////////////////////////////////////////////////////////
//
// Function:
//     Int_t operator==(const CFile& rFile)
//  Operation type:
//     Equality comparison.
// 
Int_t
CFile::operator==(const CFile& rFile)
{
	if(m_eState == rFile.m_eState) {
		if(m_eState == kfsOpen) {
			struct stat mystat;
			struct stat hisstat;

			fstat(m_nFd, &mystat);
			fstat(rFile.m_nFd, &hisstat);

			return((mystat.st_ino == hisstat.st_ino) &&
				   (mystat.st_dev == hisstat.st_dev));
		}
	}
	return kfFALSE;
}
////////////////////////////////////////////////////////////////
//
//  Function:
//    void AssertOpen();
//  Operation Type:
//    Utility for derived classes and us.
//
void
CFile::AssertOpen() const
{
  // Throws an errno exception if the file is not open.

  if(m_eState != kfsOpen) {
    errno = EBADF;
    CErrnoException except("CFile::Read - CFile object was not open");
    throw except;
  }
}
/////////////////////////////////////////////////////////////////////////
//  Function:
//      Bool_t IsReadable(UInt_t nMS)
//  Function Type:
//      State check
Bool_t
CFile::IsReadable(UInt_t nMS) const
{
  // Returns kfTRUE if the file is readable within nMS milliseconds.
  // Default action is to return TRUE as long as the file is open.
  // This is suitable for Disks and Tapes.  Note that online sources
  // may need to override this to allow for slower data sources.
  //
  return (m_eState == kfsOpen);
}
