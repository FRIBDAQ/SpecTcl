//  CPipeFile.cpp
// CPipeFile provides a mechanism for SpecTcl
// to accept data from non File sources.  In order to 
// insulate the program from differences in non file sources
// (e.g. such as data acquisition systems of different types,
// network data sources such as ftp, or http etc. etc),
// non-file data sources are modelled as a child process
// which accepts data from some source and writes it to
// stdout.  This program is started up as a child process
// using the system(3) function, with stdout redirected
// to the input end of a pipe.  Read's from CPipeFile
// are directed at the output end of the pipe.
// All functions require overriding as follows:
//
//  Open - Starts the child process.
//  Read - reads from m_nReadFd (which is the same as m_nFd).
//             also EPIPE errors are treated as an end of file.
//  Close - If m_nPid is still alive, then the pipe is not only closed, but the
//             SIGQUIT is sent and waitpid is done on m_nPid.
//  IsReadable - 
//             Does a select on the pipe, looking for read or exceptional 
//             condition.
//             time is determined by the nMs parameter.  The exceptional
//             condition also triggers a read in order to let that determine if
//             the child has exited.
//
//
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#ifdef HPUX
#ifndef _REENTRANT
#define _REENTRANT		// This is needed to get strtok_r defined.
#endif
#endif

#include "PipeFile.h" 
#include "ErrnoException.h"
#include <histotypes.h>
                              
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>

#ifdef OSF1			// Coulnd't find pipe for some reason!!
extern "C" {
  int pipe(int*);
};
#endif

static const char* Copyright = 
"CPipeFile.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Static functions

#ifdef __NEED_STRTOKR		// Need to implement strtok_r
// This is posix.1C so it seems the best of the bunch of strtok-ish ones
// to standardize on.
//
static inline char* strtok_r(char *s1, const char* s2, char** savept)
{
  if(s1) {
    *savept = s1;
  }
  return strsep(savept, s2);

}
#endif
#ifdef Darwin			// Need to implement strtok_r
// This is posix.1C so it seems the best of the bunch of strtok-ish ones
// to standardize on.
//
static inline char* strtok_r(char *s1, const char* s2, char** savept)
{
  if(s1) {
    *savept = s1;
  }
  return strsep(savept, s2);

}
#endif
#ifdef OSF1
extern "C" {			// Missing from string.h
  char* strtok_r(char* s1, const char* s2, char** savept);
}
#endif

// Functions for class CPipeFile
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//     ~ CPipeFile ( );
//  Operation Type:
//      Destructor.
//
CPipeFile::~CPipeFile()
{
  if(getState() == kfsOpen) {
    Close();			// Close pipe if open.
  }
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CPipeFile(const CPipeFile& aCPipeFile)
// Operation Type:
//    Copy Constructor.
//
CPipeFile::CPipeFile(const CPipeFile& aPipeFile)
{
  DoAssign(aPipeFile);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CPipeFile& operator= (const CPipeFile& aCPipeFile)
// Operation Type:
//    Assignment.
//
//
CPipeFile&
CPipeFile::operator=(const CPipeFile& aCPipeFile)
{
  if(this != &aCPipeFile) {
    DoAssign(aCPipeFile);
  }
  return *this;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    int operator== (const CPipeFile& aCPipeFile)
//  Operation Type:
//    Equality compare.
//
int
CPipeFile::operator==(const CPipeFile& aCPipeFile)
{
  //  Assume that there won't be any funny stuff with
  //  pipes that might have two people pointing to the same
  //  pipe.. then the base class compare is good enough:

  return CFile::operator==(aCPipeFile);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Open ( const std::string& rsConnection, UInt_t nAccess )
//  Operation Type:
//     Connector.
//
void 
CPipeFile::Open(const std::string& rsConnection, UInt_t nAccess) 
{
// Opens a pipe event source file:
// Create a new pipe.
// fork
// marshall the rsConnection string into an argument list and
// exec the program.
//
// 
// Formal Paramters:
//    const std::string& rConnection:
//      This is a valid program file and its
//       parameters.  Note that the program file
//       may be a shell script.
//   UInt_t nAccess:
//       The access which must be kacRead
//  
// Exceptions:  
//  CErrnoException

  if(getState() == kfsOpen) 
    Close(); // Close any prior pipe.

  // Figure out the access bits:

  if(nAccess != kacRead) {
    errno =EINVAL;
    throw CErrnoException("CPipeFile::Open() - Access must be kacRead");
  }

  // Open the pipe:

  int fds[2];
  if(pipe(fds)) {
    throw CErrnoException("CPipeFile::Open() - Unable to create pipe");
  }
  // Fork off the subprocess.
  //
  m_nPid = fork(); 
  if(m_nPid < 0) {
    m_nPid = 0;
    throw CErrnoException("CPipeFile::Open() - Unable to fork()");
  }

  char** argv = MakeArgv(rsConnection); // Create program and param list.

  if(m_nPid != 0) {		// Parent process.
    setFd(fds[0]);
    close(fds[1]);		// Close write side of pipe.
    setState(kfsOpen);
  }
  // Child process must redirect the pipe output to stdout and exec the child.
  //
  else {			// Child process.

    // Child process will only have writefd and it will be dup'd over
    // to stdout.
  
    close(fds[0]);
    int out = fileno(stdout);
    close(out);
    dup2(fds[1], out);
    close(fds[1]);		// Now stdout is redirected to the pipe.
    //
    //  The parameters of the connection string must be broken into
    //  'words' which are separated by whitespace, and stored in an
    //  argument list.

    execvp(argv[0], argv);

    // Control can only pass here if the execv failed:

    perror("Failed to exec child process");
    while(*argv) {
      fprintf(stderr, "%s ", *argv);
      argv++;
    }
    fputc('\n', stderr);
    exit(errno);
    
  }
  for(char** p = argv; *p != (char*)NULL; p++) {
    delete []*p;		// Delete an argument.
  }
  delete []argv;		// Delete the pointer list.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t& Read ( Address_t pBuffer, UInt_t nBytes )
//  Operation Type:
//     I/O
//
Int_t 
CPipeFile::Read(Address_t pBuffer, UInt_t nBytes)
{
// Reads data from the pipe blocking if need
// be to satisfy the I/O request count.
//
// Formal Parameters:
//    Address_t pBuffer:
//        Pointer to the buffer to receive data on.
//    UInt_t nBytes:
//         Number of bytes to read.
// Returns:
//     Number of bytes actually read or:
//     0 - if end file or EPIPE error.
// Signals CErrnoException on other problems with the read.
//
// Exceptions:  

  // The pipe buffering etc. may not let us do the read in one full gulp:

  UInt_t nTotalRead(0);

  while(nTotalRead != nBytes) {
    UInt_t nRead = CFile::Read(pBuffer, (nBytes - nTotalRead));
    if(nRead > 0) {
      nTotalRead += nRead;
      char* p     = (char*)pBuffer;
      p          += nRead;
      pBuffer     = (char*)p;
    }
    else if (nRead == 0) {
      break;			// Next read gives the end file cond.
    }
    else {			// Note in theory this is not possible.
      throw CErrnoException("CPipeFile::Read - failed CFile::Read");
    }
  }
  return nTotalRead;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Close (  )
//  Operation Type:
//     Connection.
//
void 
CPipeFile::Close() 
{
// Closes a pipe data source.  This is done by:
//  1. Closing the Pipe fd.s
//  2. If the m_nPid represents a valid process,
//     a SIGQUIT signal to the child.  A waitpid
//     is then done to ensure the process exits without
//     becomming zombie.
//   
// Exceptions:  

  CFile::Close();		// Closes read end of pipe and ensures open.
  if(m_nPid > 0) {
    int status;
    kill(m_nPid, SIGTERM);
    sleep(1);			// Give the process a second to run down.
    waitpid(m_nPid,  &status, WNOHANG);	// But no more than that.
				// Before just allowing it to be zombified.
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t IsReadable ( UInt_t nMs )
//  Operation Type:
//     I/O test
//
Bool_t 
CPipeFile::IsReadable(UInt_t nMs) const 
{
// Returns kfTRUE if the pipe is readable or
// has an exception pending or kfFalse if not.
// 
// Formal Parameters:
//      UInt_t nMs:
//       The parameter nMs determines how long
//        this function will wait for readability in ms.
//        while it is possible to specify an extremely
//        long wait, it is not possible to specify an infinite
//        wait.
// Returns:
//      kfTRUE if readable or exception pending.
//      kfFALSE if not.
//

  fd_set readfds;
  fd_set exceptfds;
  struct timeval timeout;
  int    fd(getFd());

  // Compute the timeout struct from the nMs value:
  //

  timeout.tv_sec = nMs/1000;	   // # seconds.
  nMs           -= timeout.tv_sec; // residual ms. 
  timeout.tv_usec= nMs * 1000;	   // saved in microsecond units.

  // Build up the read and except fd lists:

  FD_ZERO(&readfds);
  FD_ZERO(&exceptfds);
  FD_SET(fd, &readfds);
  FD_SET(fd, &exceptfds);

  if(select(fd+5, &readfds, (fd_set*)kpNULL, &exceptfds, &timeout) < 0) {
    throw CErrnoException("CPipeFile::IsReadable - select() failed");
  }
  return (FD_ISSET(fd, &readfds) || FD_ISSET(fd, &exceptfds));

}
/////////////////////////////////////////////////////////////////////
//
// Function:
//   void DoAssign(const CPipeFile& rRhs)
// Operation Type:
//   Protected utility.
//
void
CPipeFile::DoAssign(const CPipeFile& rRhs)
{
  CFile::DoAssign(rRhs);
  m_nPid = rRhs.m_nPid;
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//    char** MakeArgv(const std::string& rConnection)
// Operation type:
//    Protected utility.
//
char**
CPipeFile::MakeArgv(const std::string& rConnection)
{
  // Create an argument list which consists of a set of pointers
  // to 'words' in the rConnection string.  This argument list is
  // suitable for use with execv* functions.  The last element is
  // a null pointer.
  // Words are considered to be items separaeted by spaces or tabs.
  //
  
  char* pContext;

  // First count the number of items:

  char* pString = new char[strlen(rConnection.c_str())+ 1];
  strcpy(pString, rConnection.c_str());

  const char* pDelims  = " \t";
  UInt_t nItems = CountItems(pString, pDelims);
  char** pItems = new char*[nItems+1];

  // Now Pull the items out of the string.

  UInt_t nToken = 0;
  char*  pToken;
  while( pToken = strtok_r(pString, pDelims, &pContext)) {
    pItems[nToken] = new char[strlen(pToken)+1];
    strcpy(pItems[nToken], pToken);
    pString = (char*)kpNULL; // Use context for future strtok_r's.
    nToken++;
  }
  pItems[nToken] = (char*)kpNULL;
  delete []pString;
  return pItems;
}
/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    char CountItems(const char* pString, const char* pDelims)
//  Operation Type:
//    Private utility.
//
UInt_t
CPipeFile::CountItems(const char* pString, const char* pDelims)
{
  // Returns a count of the number of tokens in pString.
  // Formal Parameters:
  //    const char* pString:
  //       Pointer to the string to check.
  //    const char* pDelims:
  //       Set of token delimeters.
  // Returns:
  //   Number of tokens in the string.
  //
  // NOTE:
  //   strtok_r operates on a string which is not const, so we
  //   must copy the string locally.

  char*     p = new char[strlen(pString) +1];
  UInt_t    nItems(0);
  strcpy(p, pString);

  // Now run the token finding loop:

  char* pContext;
  char* pS = p;
  while(strtok_r(pS, pDelims, &pContext)) {
    pS = (char*)kpNULL;
    nItems++;
  }
  // Have what we need, delete the dynamically allocated storage and
  // return the item count.

  delete []p;
  return nItems;
}

