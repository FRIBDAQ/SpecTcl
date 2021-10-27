/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  Info.cpp
 *  @brief:  Provide implementations of the classes in Info.h
 */

#include "Info.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <ErrnoException.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <tcl.h>
#include <limits.h>

///////////////////////////////////////////////////////////////////////////////
// GetHostCommand implementation.

/**
 *  GetHostCommand constructor
 *    @param interp - references the interpreter on which the command
 *                    is registered.
 *    @param host   - The --host parameter value.
 */
GetHostCommand::GetHostCommand(CTCLInterpreter& interp, const char* host) :
  CTCLObjectProcessor(interp, "Xamine::getHost", TCLPLUS::kfTRUE),
    m_host(host)
{}

/**
 * GetHostCommand destructor
 */
GetHostCommand::~GetHostCommand()
{}

/**
 * operator()
 *    - ensure there are no further command line parameters.
 *    - return m_host.
 */
int
GetHostCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    try {
        requireExactly(objv, 1, "No additional command parameters accepted by getInfo");
        interp.setResult(m_host);
    } catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unanticipated exception type");
        return TCL_ERROR;
    }
    return TCL_OK;
}
///////////////////////////////////////////////////////////////////////////////
// GetPortCommand

/**
 * GetPortCommand constructor
 *   @param interp - interpreter on which the command is being registered.
 *   @param port   - stringified port (could be a service).
 *   @param pCommand - command to register.
 */ 
GetPortCommand::GetPortCommand(CTCLInterpreter& interp, const char* port, const char* pCommand) :
  CTCLObjectProcessor(interp, pCommand, TCLPLUS::kfTRUE),
    m_port(port)
{}

/**
 * GetPortCommand - destructor.
 */
GetPortCommand::~GetPortCommand()
{}

/**
 * operator()
 *  - ensure there are no command line parameters.
 *  - Return the m_port value.
 */
int
GetPortCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    try {
        requireExactly(objv, 1, "Incorrect number of command parameters");
        interp.setResult(m_port);
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unanticipated exeption type");
        return TCL_ERROR;
    }
    return TCL_OK;
}
//////////////////////////////////////////////////////////////////////////////
// GetUserCommand

/**
 * GetUserCommand -- constructor
 *  @param interp -interpreter we're registering on
 *  @param user   - name of the user to return.
 */
GetUserCommand::GetUserCommand(CTCLInterpreter& interp, const char* user) :
  CTCLObjectProcessor(interp, "Xamine::getUser", TCLPLUS::kfTRUE),
  m_username(user)
  {}
/**
 * GetUserCommand - destructor
 */
GetUserCommand::~GetUserCommand()
{}

/**
 * operator()
 *    Returns the username we were constructed with as the result
 * @param interp - interpreter running the command.
 * @param objv  - Command words - there canbe only one.
 * @return int   - TCL_OK on normal or TCL_ERROR on failure.
 */
int
GetUserCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  
  try {
      requireExactly(objv, 1, "Incorrect number of command parameters");
      interp.setResult(m_username);
  }
  catch (std::string msg) {
      interp.setResult(msg);
      return TCL_ERROR;
  }
  catch (...) {
      interp.setResult("Unanticipated exeption type");
      return TCL_ERROR;
  }
  return TCL_OK;

}

///////////////////////////////////////////////////////////////////////////////
// IsLocalCommand

/**
 * IsLocalCommand constructor
 *   @param interp - interpreter on which the command was registered.
 */
IsLocalCommand::IsLocalCommand(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, "Xamine::isLocal", TCLPLUS::kfTRUE)
{}

                    
/**
 * IsLocalCommand destructor.
 */
IsLocalCommand::~IsLocalCommand()
{
    
}
/**
 * operator()
 *    - Ensure we have the right number of command parameters.
 *    - extract the host name as a std::string object.
 *    - Determine if it's local.
 */
int IsLocalCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    try {
        requireExactly(objv, 2, "Incorrect number of command parameters");
        std::string host = objv[1];
        interp.setResult(Tcl_NewIntObj(local(host) ? 1 : 0));
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (CException & e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unanticipated exception type");
        return TCL_ERROR;
    }
    
    return TCL_OK;
}

/**
 * @param host - some arbitrary host name.
 * @return bool - true if the host is the same as the local host.
 */
bool
IsLocalCommand::local(std::string host)
{
     // Check for explicit localhost:

  if (host == std::string("localhost")) return true;


  // Create the fqdn of the local host:
  // TODO: Error handling from gethostname and getaddrinfo
  //
  char hostname[HOST_NAME_MAX+1];
  gethostname(hostname, sizeof(hostname));

  if (host == std::string(hostname)) {
    return true;
  }
  std::string fqhostname = getfqdn(hostname);


  // If the host has no periods append the domain name from
  // fqhostname.

  if (host.find(".") == std::string::npos) {

    // locate the start of the domain name in
    // fqhostname..and append the domain to the host:

    size_t domainStartsAt = fqhostname.find(".");
    if (domainStartsAt != std::string::npos) {
      host += fqhostname.substr(domainStartsAt);
    }
  }

  return host == fqhostname;

}
/**
 * given a host name return the fully qualified version of it.
 *
 * @param host - the host to do this for.
 * @return std::string fully qualified host name.
 */
std::string
IsLocalCommand::getfqdn(const char* host)
{
    struct addrinfo  hints = {AI_CANONNAME | AI_V4MAPPED | AI_ADDRCONFIG,
                            AF_UNSPEC, 0, 0,
                            0, NULL, NULL, NULL};

  struct addrinfo* hostInfo;
  if (getaddrinfo(host, NULL, &hints, &hostInfo) < 0) {
    throw CErrnoException("getaddrinfo failed");
  }

  std::string fqhostname(hostInfo->ai_canonname);
  freeaddrinfo(hostInfo);

  return fqhostname;

}
