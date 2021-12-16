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

/** @file: CHttpdSserver.cpp
 *  @brief: Implement the code to start/stop the Tcl Httpd server with SpecTcl extensions
 */

#include "CHttpdServer.h"
#include "TCLInterpreter.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>


bool CHttpdServer::m_isRunning(false);        // Singleton true when it's running.
int  CHttpdServer::m_nPort(0);                //When is runing the port.
int  CHttpdServer::m_nActualPort(0);          // The port the server is actually running on.

/**
 * constructor
 *    Just save the interpreter.
 * @param pInterp - pointer to the interpreter on which this will be run.
 */
CHttpdServer::CHttpdServer(CTCLInterpreter* pInterp) :
    m_pInterp(pInterp)
{}

/**
 * start
 *    If m_isRunning is false:
 *    -   Start the server.
 *    -   set m_isRunning true.
 *    -   save the port number requested (note this need not be the one the
 *        server probes to).
 *    If m_isRunning is true and port is different from m_nPort
 *    -  Warn the user that the server port is probably much different than they think.
 *
 *    @todo Can we report the port number from the server Config variables?
 *
 * @param port - starting point at which the server probes for free ports.
 * @return int - the actual port the server is running on.
 *
 * 
 */
int
CHttpdServer::start(int port)
{   
    if (!m_isRunning) {
        std::string prefix(INSTALLED_IN);
        std::string cmd ("lappend auto_path ");
        cmd += prefix + "/TclLibs";
        auto resultStr = m_pInterp->GlobalEval(cmd);
        resultStr = m_pInterp->GlobalEval("package require SpecTclHttpdServer");
        std::stringstream startcmd;
        startcmd << "startSpecTclHttpdServer [::SpecTcl::findFreePort " << port <<" ]";
        //resultStr = m_pInterp->GlobalEval("startSpecTclHttpdServer [::SpecTcl::findFreePort 8080]");
        resultStr = m_pInterp->GlobalEval(startcmd.str());
        std::cout << " SpecTcl REST Server running on port: " << resultStr << std::endl;
        
        m_isRunning = true;
        m_nPort = port;
        m_nActualPort = atoi(resultStr.c_str());
        
    
    } else if (port != m_nPort) {
        std::cerr << "****WARNING - you are trying to double start the SpecTcl REST server\n";
        std::cerr << "              the server alread is started with a probed port of "
                  << m_nPort << std::endl;
        std::cerr << "              you are trying with port " << port << std::endl;
        std::cerr <<  "              it is likely the server is actually running \n";
        std::cerr << "              on a far different port than you think it is\n";
        
    }
    return m_nActualPort;
}
/**
 * stop
 *    If the server is running stop it.. otherwise, throw an std::logic_error
 *    If successful, m_isRuning -> false.
 */
void
CHttpdServer::stop()
{
    if (m_isRunning) {
        auto resultStr = m_pInterp->GlobalEval("Httpd_ServerShutdown");
        m_isRunning = false;
    } else {
        throw std::logic_error("Attempted shutdown of tclhttpd server that's not running");
    }
}
/**
 * getActualPort
 *   @return int - the actual port the server is listening on.
 *   @throw std::logic_error - if the server isn't running.
 */
int
CHttpdServer::getActualPort()
{
    if (m_isRunning) {
        return m_nActualPort;
    } else {
        throw std::logic_error("Tried to get the httdp port number but server isn't running");
    }
}
/**
 * isRunning
 *   @return bool - true if the server is runnning false if not.
 */
bool
CHttpdServer::isRunning() 
{
    return m_isRunning;
}
