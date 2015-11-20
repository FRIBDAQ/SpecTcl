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
#include "CTCLServer.h"
#include "TCLInterpreter.h"
#include "CTCLTcpServerInstance.h"

#include <ErrnoException.h>
#include <errno.h>

using namespace std;


/////////////////////////////////////////////////////////////////////////////////


/*!
   Construct the module and set up the listener.  The listener
   is set up using Tcl_OpenTcpServer  errors are reported as
   a CErrnoException using Tcl_GetErrno to get and set the errno variable.
   \param interp - Interpreter to which commands will be submitted.
   \param port   - Tcp/Ip port on which to listen. If this is not a privileged
                   program this should generally be larger than 1023.
*/
CTCLServer::CTCLServer(CTCLInterpreter* pInterp,
		       int              port) :
  m_pInterpreter(pInterp),
  m_nPort(port)
{
  m_listenChannel = Tcl_OpenTcpServer(pInterp->getInterpreter(),
				      port, 
				      NULL,
				      connectionRelay, this);

  if (m_listenChannel == NULL) {
    errno = Tcl_GetErrno();
    throw CErrnoException("TclServer starting listener");
  }
}
/*!
   Destruction does a shutdown().
*/
CTCLServer::~CTCLServer()
{
  shutdown();
}


////////////////////////////////////////////////////////////////////////////////

/*!
   This static function establishes object context by casting the client
   data to a CTCLServer pointer.  If allowConnection returns true, 
   we call onConnection, otherwise, the connecting channel is shut down.
   \param pData      - Really a pointer to the server/listener objecdt.
   \param connection - The Tcl_Channel that can transfer data between client/server.
   \param pHostname  - Name of the host connecting.
   \param port       - TCP port number for the data transfer socket.
*/
void
CTCLServer::connectionRelay(ClientData  pData,
			    Tcl_Channel connection,
			    char*       pHostname,
			    int         port)
{
  CTCLServer* pServer  = reinterpret_cast<CTCLServer*>(pData);

  string host(pHostname);
  if (pServer->allowConnection(connection, host)) {

    // Connection allowed, call onConnection to let it finish stuff off:
    
    pServer->onConnection(connection, host);
  }
  else {
    Tcl_Close(pServer->m_pInterpreter->getInterpreter(),
	      connection);
  }
}

/////////////////////////////////////////////////////////////////////////////

/*!
  Called by a server instance when its exiting.  The server instance is
  removed from the book-keeping list.
  \param pInstance - Pointer to a CTCLTcpServerInstance to remove.
*/
void
CTCLServer::instanceExit(CTCLTcpServerInstance* pInstance)
{
  std::list<CTCLTcpServerInstance*>::iterator p = m_serverInstances.begin();
  while (p != m_serverInstances.end()) {
    if (*p == pInstance) {
      delete *p;		  //  Destroy the instance.
      m_serverInstances.erase(p); // Erase it from the list.
      break;
    }
    p++;
  }

}

/*!
  Called to shutdown the server.  All instance servers are destroyed.
  In so doing, they close their connections to the outside world.
*/
void
CTCLServer::shutdown()
{
  // First stop our listener:

  Tcl_Close(m_pInterpreter->getInterpreter(), m_listenChannel);

  // Tell each server instance it's gotten an eof.. That will cause
  // it to call instanceExit for itself which will delete the instance
  // and remove its pointer from the list.
  //
  while (!m_serverInstances.empty()) {
    m_serverInstances.front()->onEndFile(); // EOF handling calls instanceExit.
  }
}



/*!
   This is called on a connection. In order to support creating subclasses of
   CTCLTcpServerInstance, we will actually create the server with the virtual
   member createInstance, so this class can be subclassed, and that member overriden.
   (strategy pattern). The pointer to the resulting object is isnerted in
   the m_seerverInstnaces list.
   \param connection - Tcl_Channel the client and server use to communicate.
   \param hostname   - Name of the client's host.

*/
void
CTCLServer::onConnection(Tcl_Channel connection,
			 string     host)
{
  CTCLTcpServerInstance*  pInstance = createInstance(connection, host);
  m_serverInstances.push_back(pInstance);
  pInstance->start();

}

/////////////////////////////////////////////////////////////////////////

/*!
  Determines if the connection is allowed.  The default behavior is to
  require that the hostname be either the string "localhost" or 
  "127.0.0.1"  both of which indicate a local connection.

  This can be overridden to implement a security policy for connections.

  \param connection - Data connection (provided to allow some authentication
                      handshaking if desired.
  \param hostname   - Name of the client's host.

  \return bool
  \retval true  - Connection is allowed.
  \retval false - Connection should not be allowed.

*/
bool
CTCLServer::allowConnection(Tcl_Channel connection,
			    string      hostname)
{
  if (  (hostname == string("localhost"))   ||
	(hostname == string("127.0.0.1"))) {
    return true;
  }
  else {
    return false;
  }
	 
}
/*!
   Create the default server instance object type
   (CTCLTcpServerInstance) and return it. This can be overridden
   in subclasses if a different type is required.
   \param connection - Tcl_Channel on which data communication will take place.
   \param hostname   - Name of the host to which the connection will be made.

   \return CTCLTcpServerInstance*
*/
CTCLTcpServerInstance*
CTCLServer::createInstance(Tcl_Channel connection,
			   string      hostname)
{
  return new CTCLTcpServerInstance(m_pInterpreter,
				   connection,
				   this);
}

