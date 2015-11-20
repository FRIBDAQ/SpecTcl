/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include "CFileDataSource.h"
#include <errno.h>

#include <TCLInterpreter.h>
#include <TCLObject.h>

#include <iostream>

/**
 * static unbound functions.
 *  - could become static private methods?
 */

/**
 * throwResult
 * 
 *  Throw a data_source_exception that requires the Tcl result string.
 *
 * @param interp  - Interpreter object reference.
 * @param base_msg - message prefix.
 */
static
void throwResult(CTCLInterpreter& interp, const char* base_msg)
{
  std::string result = interp.GetResultString();
  throw data_source_exception(base_msg, result.c_str());
}

/*------------------------------------------------------------------------------------*/
/**
 * constructor
 *   Construction is a 2 phase construction.  First the object is constructed then
 *   SpecTcl attaches the object to the front of the pipeline.  The constructor
 *   is just supposed to initialize local data, onAttach is called to actually
 *   connect the data source to a source.
 */
CFileDataSource::CFileDataSource() :
  m_fd(0),
  m_eof(false),
  m_pInterp(0)
{}

/**
 * destructor
 *
 *  Essentially a close() call.
 */
CFileDataSource::~CFileDataSource()
{
  if (m_pInterp && m_fd) {	// Don't want to deal with the exception here.
    close();
  }
}


/**
 * onAttach
 *   Attach the data source to the file.   This comes down to an open of the file.
 *   A std::error_condition of type std::generic_category is throw if the openfails.
 *   errno is used as the error code.
 *
 *  @param interrp -references the main SpecTcl Tcl interpreter.
 *  @param pSource -the remainder of the URI after the file://.  This is just a file-
 *                  system path.
 *
 * @throw std::error_condition if the file could note be opened.
 *
 * @note - The file is open for readonly.
 */
void
CFileDataSource::onAttach(CTCLInterpreter& interp, const char* pSource)
{

  /*
    Make pathPtr a Tcl object whose string representation is the path we're opening.
    Note that all pSource's will lead with "//". Absolute paths lead with "///"
    relative paths with "//" Therefore we need to strip the first two chars from
    the source.
  */

  
  CTCLObject pathPtr;
  pathPtr.Bind(interp);
  pathPtr = &(pSource[2]);


  m_pInterp  = &interp;
  Tcl_Interp* pI = interp.getInterpreter();

  Tcl_Channel fd = Tcl_FSOpenFileChannel(pI, pathPtr.getObject(), "r", 0);
  if (fd == NULL) {
    std::string msg("CFileDataSource::onAttach open failed for: ");
    msg += pSource;
    throw data_source_exception( Tcl_GetErrno(), msg.c_str());
  } else {
    m_fd = fd;
  }
  Tcl_RegisterChannel(pI, fd);

  // Set the channel options for binary I/O.

  int stat = Tcl_SetChannelOption(pI, fd, "-translation", "binary");
  if (stat != TCL_OK) {
    throwResult(interp, "CFileDataSource::onAttach fconfigure -translation binary failed");

  }
  stat = Tcl_SetChannelOption(pI, fd, "-encoding",    "binary");
  if (stat != TCL_OK) {
    throwResult(interp, "CFileDataSource::onAttach fconfigure -encoding binary failed");
  }
}

/**
 * createEvent
 *
 *   Attach readability handler to the data source.
 *   The idea  is that when the file is readable, we'll fire the event handler which, in turn.
 *   will read our data and shoot it down the pipeline.
 *
 * @param interpreter - Interpreter on which to register the handler.
 * @param handler     - Pointer to the handler
 * @param pClientData - Data passed without interpretation to the handler.
 *
 */
void
CFileDataSource::createEvent(CTCLInterpreter& interp, ChannelHandler* pHandler, void* pClientData)
{
  m_pInterp = &interp;

  setChannelHandler(pHandler, pClientData);
  Tcl_CreateChannelHandler(m_fd, TCL_READABLE | TCL_EXCEPTION, channelRelay, this);

}

/**
 * read
 *   get data from the source
 *
 * @param pBuffer - where to put the data.
 * @param nBytes  - number of bytes of data to read
 *
 * @return size_t - Number of bytes actually read... zero indicates an end of file and
 *                  subsequent calls to isEof return true until the source is close(d) and
 *                  reattached.
 *
 *
 * @throw data_source_exception if the read failed
 */
size_t 
CFileDataSource::read(void* pBuffer, size_t nBytes)
{

  if (!m_pInterp || !m_fd) {
    throw data_source_exception("CFileDataSource::read failed", "Data source is not attasched");
  }

  int nRead = Tcl_Read(m_fd, reinterpret_cast<char*>(pBuffer), nBytes);
  if (nRead == -1) {
    throw data_source_exception(Tcl_GetErrno(), "CFileDataSource::read  Tcl_Read failed");
  } 
  if (nRead == 0) {
    m_eof = true;
  }
  return nRead;
}


/**
  * close
  *   Close the file.  This means:
  *   - Unregistering the readability event.
  *   - Closing the channel
  *   - setting m_fd, m_pInterp -> 0
  *   - Setting m_eof -> false.
  */
void
CFileDataSource::close()
{
  if (m_pInterp && m_fd) {
    Tcl_DeleteChannelHandler(m_fd, channelRelay, this );
    Tcl_UnregisterChannel(m_pInterp->getInterpreter(), m_fd);
    m_fd      = 0;
    m_pInterp = 0;
    m_eof     = false;
  } else {
    throw data_source_exception("CFileDataSource::close  failed", "Source not open");
  }
}
/**
 * isEof
 *    @return bool
 *    @retval true - the file is open and at the EOF.
 *    @retval false - the  file is open and not at the EOF.
 *    @note If the file is not open the meaning of the result is not well defined.
 */
bool
CFileDataSource::isEof() 
{
  return m_eof;
}

/*---------------------------------------------------------------------------------------
 *
 * Static members.
 */

/**
 * channelRelay
 *   Channel ready handler. Establish object context and invoke the handler.
 *
 * @param pData - actually a pointer to the file handler that fired.
 * @param mask  - Mask of events that occured.
 */
void
CFileDataSource::channelRelay(ClientData pData, int mask)
{
  CFileDataSource* pThis = reinterpret_cast<CFileDataSource*>(pData);
  pThis->invokeHandler();
}
