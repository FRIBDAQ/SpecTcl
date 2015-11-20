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

#include <config.h>
#include "CTCLTcpServerInstance.h"
#include "CTCLServer.h"
#include "TCLInterpreter.h"
#include <string>

using namespace std;


/////////////////////////////////////////////////////////////////////////

/*!
  Construction just invokes base class construction and saves the
  listener pointer so that onEndFile can call it.
*/

CTCLTcpServerInstance::CTCLTcpServerInstance(CTCLInterpreter*   pInterp,
					     Tcl_Channel       connection,
					     CTCLServer*       pServer) :
  CTCLChannelCommander(pInterp, connection),
  m_pListener(pServer)
{}

/*!
  Destruction gets and closes the channel.
  Typically destruction happens when onEndFile calls
  the instanceExite member to indicate it's exiting.
  
*/
CTCLTcpServerInstance::~CTCLTcpServerInstance()
{
  Tcl_Close(m_pInterp->getInterpreter(), getChannel());
}
////////////////////////////////////////////////////////////////////////

/*!
   Called when an end file is detected on the socket.  This means
   the client has disconnected.
*/
void
CTCLTcpServerInstance::onEndFile()
{
  CTCLChannelCommander::onEndFile();
  m_pListener->instanceExit(this); // kill ourselves off.

  // Must not be any code below this point as the object has been
  // deleted!!!!!!!
}

/*!
  The resultis returned by writing it to the 
  channel.
*/
void
CTCLTcpServerInstance::returnResult()
{
  Tcl_Interp*  pInterp = m_pInterp->getInterpreter();
  const char*  pResult = Tcl_GetStringResult(pInterp);
  string result(pResult);
  result += "\n";		// So we can ensure this is one message to the socket.
  Tcl_WriteChars(getChannel(), result.c_str(), -1);
  Tcl_Flush(getChannel());
}
