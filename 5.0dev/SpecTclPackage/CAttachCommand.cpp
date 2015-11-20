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
#include "CAttachCommand.h"
#include "CSpecTclInterpreter.h"
#include "TCLObject.h"
#include "CDataSource.h"
#include "CDataSourceFactory.h"
#include "CAnalysisPipeline.h"

#include <string>
#include <tcl.h>
#include <uriparser/Uri.h>

/**
 * @file CAttachCommand.cpp
 * @brief Implements the spectcl::attach command.
 */


// per class data:

CAttachCommand* CAttachCommand::m_pInstance(0);

/**
 * constructor
 *
 *   Constructs the command processor..  It's the responsibility of
 *  the first call to instance() to ensure that the ::spectcl:: namespace has
 *  been created if need be.
 */
CAttachCommand::CAttachCommand() :
  CTCLObjectProcessor(*(CSpecTclInterpreter::instance()), "::spectcl::attach"),
  m_pDataSource(0)
{}

/**
 * Destructor: kill off the data source. This is implemented to support idempotency
 *             in testing.
 */
CAttachCommand::~CAttachCommand() 
{
  delete m_pDataSource;
  m_pDataSource = 0;
}
/**
 * instance
 *
 *  -  If the instance pointer is null create the new instance
 *  -  Return the instance.
 *
 * @return CAttachCommand*
 *
 */
CAttachCommand*
CAttachCommand::instance()
{
  if (!m_pInstance) {
    createInstance();
  }
  return m_pInstance;
}

/**
 * operator()
 *
 *   Process the command.
 *   - There must be a parameter.
 *   - It must be a valid URI
 *   - There must be a DataSource Factory creator associated with the 
 *     protocol of the URI.
 *   - If all that holds, create the data source.
 *   - Set the data  source in the analysis pipeline data source.
 *
 * @param interp - reference to the intrpreter that is running the command.
 * @param objv   - enacpsulated Tcl_Obj's that make up the command words.
 *
 * @return int
 * @retval TCL_OK - the data source is attached. 
 * @retval TCL_ERROR - The data source attach failed and the result is the
 *                     human readable error message.
 */
int
CAttachCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  try {
    CDataSourceFactory fact;

    requireExactly(objv, 2, "spectcl::attach Insufficent parameters, missing data source URI");
    std::string uri = std::string(objv[1]);
    std::string protocol = getURIProtocol(uri);

    // the pipeline must have a decoder if we are going to attach:

    CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
    if (!pPipe->getDecoder()) {
      throw std::string("attach - no decoder associated with pipeline");
    }

    // Create the new data source:

    CDataSource* pDataSource = fact.create(protocol.c_str());
    if (!pDataSource) {
      std::string msg;
      msg = "attach - I don't know how to create data sources for ";
      msg += uri;
      msg += ": URIs";
      throw msg;
    }
    // Note the analysis pipeline will take care of closing the data source.

    m_pDataSource = pDataSource;

    // Attach the new one:

    std::string target = getURITail(uri);
    pDataSource->onAttach(interp, target.c_str());

    // Let the analysis pipeline know we have a new data source:

    pPipe->setDataSource(pDataSource);
    
  }
  catch(std::string& msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  catch (std::exception& e) {
    interp.setResult(e.what());
    return TCL_ERROR;
  }

  interp.setResult("");

  return TCL_OK;
}

/*-------------------------------------------------------------------------------
 *
 * Private utilties.
 */

/**
 * createInstance
 *   - If necessar create the namespace.
 *   - Construct the instance saving a pointer to it in m_pInstance.
 */
void
CAttachCommand::createInstance()
{
  // If necessary create the namespace:

  CTCLInterpreter* pInterp = CSpecTclInterpreter::instance();
  Tcl_Interp*      pRawInterp = pInterp->getInterpreter();

  Tcl_Namespace* pGlobal   = Tcl_GetGlobalNamespace(pRawInterp);

  if(!Tcl_FindNamespace(pRawInterp, "spectcl", pGlobal, 0)) {
    Tcl_CreateNamespace(pRawInterp, "spectcl", 0, 0);
  }


  // Now we can create the instance:

  m_pInstance = new CAttachCommand();

}

/**
 * getURIProtocol
 *
 * Parse the protocol (scheme) from a URI.
 * 
 * @param uri - The URI string (we validate it here).
 *
 * @return std::string the 'scheme' part of the URI.
 *
 * @throw std::string - if the uri fails to parse.
 */
std::string
CAttachCommand::getURIProtocol(std::string uri)
{
  UriParserStateA state;
  UriUriA        parsed;

  state.uri = &parsed;
  if(uriParseUriA(&state, uri.c_str()) != URI_SUCCESS) {
    uriFreeUriMembersA(&parsed);
    throw std::string("spectcl::attach - Invalid URI for data source");
  }
  const char* p    = parsed.scheme.first;
  const char* pEnd = parsed.scheme.afterLast;
  if ((p == NULL) || (p == pEnd)) {
    uriFreeUriMembersA(&parsed);
    throw std::string("spectcl::attach - URI requires a protocol (scheme) e.g. file:");
  }

  std::string result(p, pEnd);
  uriFreeUriMembersA(&parsed);
  return result;

}
/**
 * getURIHierarchy
 *
 *   Parse the tail of the URI. for our purposes, that's the part after
 *   the protocol:
 *
 * @param uri - The uri to be parsed.
 * 
 * @return std::string.
 *
 * @note - wasteful to parse the URI in both this method and geURIProtocol, but attach
 *         is not frequent so it's well amortized.
 * @throw std::string - if the URI fails to parse or has not hierarchical part
 */
std::string
CAttachCommand::getURITail(std::string uri)
{
  // Figure out the protocol:

  std::string proto = getURIProtocol(uri);
  proto += ":";			// Put the : back in.

  // Strip the protocol off the front:

  return uri.substr(proto.size());
}
