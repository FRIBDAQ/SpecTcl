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

/** @file:  CmdInfo.h
 *  @brief: Commands that provide information about command parameters.
 */
#ifndef CMDINFO_H
#define CMDINFO_H
#include <TCLObjectProcessor.h>
#include <string>

class CTCLInterpreter;
class CTCLObject;

/**
 * @class CmdInfo
 *    This command class is constructed with a command name and a string.
 *    All we really do is provide the value of the string.
 *    The idea is that the main instantiates one of us for each of the command
 *    line option values and the script can then use these to determine
 *    how we starte4d.
 */
class CmdInfo : public CTCLObjectProcessor
{
private:
    std::string m_value;
public:
    CmdInfo(CTCLInterpreter& interp, const char* command, const char* value);
    virtual ~CmdInfo();
    
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};

#endif