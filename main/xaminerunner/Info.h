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

/** @file:  Info.h
 *  @brief: Provides getHost, getPort and isLocal in Xamine namespace.
 */
#ifndef INFO_H
#define INFO_H
#include <TCLObjectProcessor.h>
#include <string>
class CTCLInterpreter;
class CTCLObject;


/**
 * getHost command processing class.
 */
class GetHostCommand : public CTCLObjectProcessor {
private:
    std::string m_host;
public:
    GetHostCommand(CTCLInterpreter& interp, const char* host);
    virtual ~GetHostCommand();
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};
/**
 * getPort command processing class.
 */
class GetPortCommand : public CTCLObjectProcessor {
private:
    std::string m_port;                // Might be a service.
public:
    GetPortCommand(CTCLInterpreter& interp, const char* port);
    virtual ~GetPortCommand();
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

};

/**
 * getUser  - command to get the user to look up if a managed port.
 */
class GetUserCommand : public CTCLObjectProcessor
{
 private:
    std::string m_username;
public:
    GetUserCommand(CTCLInterpreter& interp, const char* user);
    virtual ~GetUserCommand();
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};

/**
 * IsLocalCommand
 *   Determine if a host (IP or DNS) is local.
 */
class IsLocalCommand : public CTCLObjectProcessor {
public:
    IsLocalCommand(CTCLInterpreter& interp);
    virtual ~IsLocalCommand();
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
private:
    bool local(std::string host);
    std::string getfqdn(const char* host);
};


#endif