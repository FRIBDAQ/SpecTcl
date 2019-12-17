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

/** @file:  CHttpdServer.h
 *  @brief: Manage the TclHttpdServer startup.
 */
#ifndef CHTTPDSERVER_H
#define CHTTPDSERVER_H

class CTCLInterpreter;

/**
 * @class CHttpdServer
 *    This class is responsible for starting the TCL Httpd Server with
 *    SpecTcl customizations.  The server will only be started once.
 *    The start method can take an optional port however, if the
 *    httpd server is started and the startup port requested was different,
 *    a warning message will be issued.
 */
class CHttpdServer {
private:
    CTCLInterpreter*    m_pInterp;
    static bool         m_isRunning;       //Supports singleton startup.
    static int          m_nPort;
    static int          m_nActualPort;
public:
    CHttpdServer(CTCLInterpreter* pInterp);
    
    int start(int port=8080);
    void stop();
    static int getActualPort();
    static bool isRunning();
};


#endif