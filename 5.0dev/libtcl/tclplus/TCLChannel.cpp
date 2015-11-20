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


#include <config.h>
#include <TCLVersionHacks.h>
#include <TCLInterpreter.h>
#include <TCLChannel.h>
#include <tcl.h>
#include <stdio.h>
#include <string.h>

using namespace std;

/*!
  Construct a File channel.  A file channel is a TCL abstraction on top of the
  underlying operating system's mechanisms for reading/writing/accessing files.
  
  \param pInterp (CTCLInterpreter* [m]):  
     The Tcl interpreter this channel should be opened on.
  \param Filename (string)
      The name of the file to open.  This is a filename in the
      naming convention of the underlying system.  For platform independent
      programs you may be well advised to construct this using Tcl_JoinPath.
  \param pMode (const char* [in]):
      The mode of the open.  This is the same as the mode string in the
      TCL [open] command.
  \param permissinos (int):
      A posix compatibile permissions mask this is used only if the file is being
      created.
     

  \throw  string: an error message if the operation failed.  The reason for the
          failure is returned.

*/
CTCLChannel::CTCLChannel(CTCLInterpreter* pInterp, 
	      string      Filename,
	      const char*      pMode,
	      int              permissions) throw (string):
  CTCLInterpreterObject(pInterp),
  m_fCloseOnDestroy(true),
  m_fRegistered(false)
{
  m_Channel = Tcl_OpenFileChannel(getInterpreter()->getInterpreter(), 
				  (tclConstCharPtr)Filename.c_str(), 
				  (tclConstCharPtr)pMode,
				  permissions);
  if(!m_Channel) {
    int e = Tcl_GetErrno();	// Below may modify it.
    string failure("Failed to open ");
    failure += Filename;
    failure += " ( ";
    failure += pMode;
    failure += " ) ";
    Tcl_SetErrno(e);		// Restore in case string ops change it.
    failure += Tcl_ErrnoMsg(e);
    throw(failure);
  }
}
/*!
   Construct a command pipeline channel.  A command pipeline is a channel abstraction
   to a pipeline of executing programs.  Using command pipelines, the client
   can feed input to stdin of the pipeline as well as gather the stdout and stderr
   The pipeline is specified using the argc/argv style where I believe a multistage
   pipe can be constructed via  elements of argv that contain '|' and I/O redirection
   can be done similarly as described in the documentation of the Tcl
   [exec] and [open] commands.

   \param pInterp (CTCLInterpreter* [m]):
      The interpreter on which this channel will be registered/executed.
   \param argc (int):
       Number of words describing the command pipeline.
   \param pargv (char** [in]):
       The command pipeline description words.
   \param flags (int)
       Bitwise or of Flags that describe how the pipeline is connected to the channel:
       - TCL_STDIN - the first element of the pipeline takes stdin from writes
                     to the channel.
       - TCL_STDOUT - The last element's stdout is connected to reads from the
                      channel.
       - TCL_STDERR - The last element's stderr is connected to reads from the channel
       - TCL_ENFORCE_MODE - Causes the open to return an error to be returned if
                      the pipeline then further redirects file descriptors
                      that should go to or come from the channel.
    \throw string - contaning an error if there's a problem opening the pipe.
*/
CTCLChannel::CTCLChannel(CTCLInterpreter* pInterp,
	      int              argc,
	      const char**           pargv,
	      int              flags)  throw (string)	  :
  CTCLInterpreterObject(pInterp),
  m_fCloseOnDestroy(true),
  m_fRegistered(false)
{
  m_Channel = Tcl_OpenCommandChannel(getInterpreter()->getInterpreter(),
				     argc, 
				     (tclConstCharPtr*)pargv, 
				     flags);
  if(!m_Channel) {
    int e =Tcl_GetErrno();
    string error("Unable to open pipeline: ");
    for (int i =0; i < argc; i++) {
      error += (pargv[i]);
      error += " ";
    }
    error += " : ";
    Tcl_SetErrno(e);
    error += Tcl_ErrnoMsg(e);
    throw(error);
  }

}
/*!
   Construct a Tcp/Ip client channel.  The channel attempts to form
   a connection to a TCP/Ip server.
   
   \param pInterp (CTCLInterpreter* [m]):
      The interpreter on which this channel will be opened.
   \param port (int):
      The port on which the server we connect to will be running.
   \param host (string host):
      Name or IP of the host on which the server we are connecting to is running.

    \throw string If the the conenction cannot be made a string is thrown indicating
        the reason for the failure.

*/
CTCLChannel::CTCLChannel(CTCLInterpreter* pInterp,
			 int              port,
			 string      host)  throw (string) :
  CTCLInterpreterObject(pInterp),
  m_fCloseOnDestroy(true),
  m_fRegistered(false)
{
  m_Channel = Tcl_OpenTcpClient(getInterpreter()->getInterpreter(), 
				port, 
				(tclConstCharPtr)host.c_str(),
				NULL, 0, 0);

  if(!m_Channel) {
    int e = Tcl_GetErrno();

    string err("Could not open Tcp Client to ");
    char buffer[200];
    sprintf(buffer, " %d@%s : ", port, host.c_str());
    err += buffer;

    Tcl_SetErrno(e);
    err += Tcl_ErrnoMsg(e);
    throw err;
  }
}
/*!
   Construct a Tcp/ip server channel.  The server channel is listening on 
   the designated port for connection requests.  When a client connects, the
   user's handler is called.  The handler is passed three parameters in order:
   - ClientData  AppData - the AppData parameter passed to us without interpretation.
   - Tcl_Channel cChan   - Channel open to communicate with the client.
   - char*       pHost   - Name of the host connecting.
   - int         port    - Port assigned for communication.

   Note that the seerver has already done an accept(2), the only way to reject
   a client is to close the corresponding channel.

   \param pInterp (CTCLInterpreter* [m]):
      The interpreter on which this channel will be opened.
   \param port (int):
      The port on which the server we connect to will be running.
   \param proc (Tcl_TcpAcceptProc [callback]):
       This function is called when a client connects.  Parameters are described
       above.
   \param AppData (ClientData [?]):
       Data that is passed to the client without any interpretation.  In the OO
       world, this is recommended to be a pointer to the object that will
       service the client.

    \throw string If the channel cannot be opened, a string exception is thrown.
        The string dscribes the reason the channel could not be opened.

*/
CTCLChannel::CTCLChannel(CTCLInterpreter* pInterp,
	      int              port,
	      Tcl_TcpAcceptProc* proc,
			 ClientData       AppData)   throw(std::string) :
  CTCLInterpreterObject(pInterp),
  m_fCloseOnDestroy(true),
  m_fRegistered(false)
{
  m_Channel = Tcl_OpenTcpServer(getInterpreter()->getInterpreter(), 
				port, NULL, proc, AppData);
  if(!m_Channel) {
    int e = Tcl_GetErrno();
    string err("Unable to establish a server on port: ");
    char buffer[100];
    sprintf(buffer, "%d : " , port);
    err += buffer;
    Tcl_SetErrno(e);
    err += Tcl_ErrnoMsg(e);
    throw err;
    
  }

}
/*!
   Create a channel from an existing open channel.
   In this case, we are not allowed to close or register
   the channel... only the original creator can do that.
   
   \param pInterp (CTCLInterpreter* [m]):
       The interpreter on which the channel is open
   \param Channel (Tcl_Channel [m]):
       The channel to bless into this object.

*/
CTCLChannel::CTCLChannel(CTCLInterpreter* pInterp,
			 Tcl_Channel      Channel) :
  CTCLInterpreterObject(pInterp),
  m_Channel(Channel),
  m_fCloseOnDestroy(false),
  m_fRegistered(false)
{}

/*!
   Create a channel via copy construction.  The
   channel created will have a copy of m_Channel, and
   the flags all set to false to prevent close/unregister on destroy.
*/
CTCLChannel::CTCLChannel(const CTCLChannel& rhs) :
  CTCLInterpreterObject(rhs.getInterpreter()),
  m_Channel(rhs.m_Channel),
  m_fCloseOnDestroy(false),
  m_fRegistered(false)
{}

/*!
   Destroy a channel, nothing happens unless m_fCloseOnDestroy is set then:
   - if m_fRegistered is set, Tcl_UnregisterChannel is called
   - if m_fRegistered is not set, Tcl_Close is called.
*/
CTCLChannel::~CTCLChannel()
{
  Close();
}
/*!
    Read data from the channel.  The data are read using Tcl_ReadChars which
    implies that transformations consistent with the current encoding on the
    channel are applied to convert the data to UTF-8 unless the channel encoding
    has been set to binary.. in which case no transformations are performed.
    See SetEncoding and GetEncoding for more about this... as well as the 
    TCL documentation.
    \param pData (void** [in]):
       Points to a pointer.  The pointer will be filled in with a dynamically
       allocated buffer in which the data have been transferred.  The
       buffer must be deleted by the caller.  It was allocated as 
       new char[nnn].  No effot is made to null terminate etc. since
       in binary mode, this may be just bytes.

       This is done because in general there's no way for the caller to know
       a-priori how large a buffer is required to hold nchars o UTF-8 encoded
       textual data.
    \param nChars (int):
       The number of bytes to read.

   \return int
   \retval >= 0: The number of \em characters transfered. See the special cases below.
   \retval < 0:  An error occured that is stored in errno.

   Special cases:
   - If the return value is 0, this means either that the end file was reached
     or that the channel is in nonblocking mode and had nothing to read.  A call
     to atEof() will distinguish between these two cases.
   - The return value can be less than the requested value either because the end
     file was encountered, or because the encoding of the file required more than
     1 byte per character.
*/
int
CTCLChannel::Read(void** pData, int nChars)
{
  Tcl_Obj* pObject = Tcl_NewObj();
  int nRead = Tcl_ReadChars(m_Channel, pObject, nChars, 0);

  // Only need to fill the buffer if nRead > 0:

  if(nRead >0) {
    int nBytesRead;
    unsigned char* pDataRead = Tcl_GetByteArrayFromObj(pObject, &nBytesRead);
    char* pUserBuffer = new char[nBytesRead];
    memcpy(pUserBuffer, pDataRead, nBytesRead);
    *pData = pUserBuffer;
  }
  else {
    *pData = reinterpret_cast<void*>(NULL);
  }
  Tcl_DecrRefCount(pObject);
  return nRead;
}
/*!
   Writes a set of bytes to the channel.  Note that unless the encoding has been set
   to binary, the buffer is assumed to contain nbytes of utf-8 data that
   will be encoded as described by the encoding. Note that data written
   with Write may not appear in the underlying operating system channel until
   Flush is called. 

   \param pData (const void* [in]):
       Pointer to the data to be written.
   \param nBytes (int)
       Number of bytes to write.

   \return int
      \retval >= 0   Number of bytes asctually written.  0 may indicate that
                     the channel is in nonblocking mode and cannot be written yet.
      \reetval <=0   Indicates an error described by errno.

  Notes:
    - The returned value may be larger than nBytes because the encoding may
      expand the data.

*/
int
CTCLChannel::Write(const void* pData, int nBytes)
{
  int nWritten = Tcl_WriteChars(m_Channel, (const char*)pData, nBytes);
  return nWritten;
}
/*!
    Returns true if the channel is at eof.  
*/
bool
CTCLChannel::atEof()
{
  return (Tcl_Eof(m_Channel) != 0);
}

/*!
   Flushes the output data buffer to the underlying stream.
*/
void
CTCLChannel::Flush()
{
  Tcl_Flush(m_Channel);
}
/*!
   Closes the channel.  This is a bit more complex than it appears:
   - This is a no-op if CloseOnDestroy is not true.
   - If m_fRegistered is true, then this just unregisters.
*/
void
CTCLChannel::Close()
{
  if(m_fCloseOnDestroy) {
    if(m_fRegistered) {
      Tcl_UnregisterChannel(getInterpreter()->getInterpreter(), m_Channel);
    } 
    else {
      Tcl_Close(getInterpreter()->getInterpreter(), m_Channel);
    }
    m_fCloseOnDestroy = false;	// Prevent double close on destruction.
  }
  
}
/*!
  Registers a channel to make it visible to the TCL script world.
  \param Name (string [in]):
      Name under which the channel should be registered.
 */
void
CTCLChannel::Register()
{
  Tcl_RegisterChannel(getInterpreter()->getInterpreter(), m_Channel);
}
/*!
    Set the input/output encoding of the channel (binary means no encoding).
    \param name (string):
        The name of the encoding.
*/
void
CTCLChannel::SetEncoding(string name)
{
  Tcl_SetChannelOption(getInterpreter()->getInterpreter(),
		       m_Channel, "-encoding", 
		       (tclConstCharPtr)name.c_str());
}
/*!
   Return the current encoding.
*/
string
CTCLChannel::GetEncoding()
{
  Tcl_DString String;
  Tcl_DStringInit(&String);

  Tcl_GetChannelOption(getInterpreter()->getInterpreter(),
		       m_Channel, "-encoding", &String);
  
  string value(Tcl_DStringValue(&String));
  Tcl_DStringFree(&String);

  return value;
}
