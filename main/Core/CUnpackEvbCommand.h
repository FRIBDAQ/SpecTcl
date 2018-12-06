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

/** @file:  CUnpackEvbCommand.h
 *  @brief: Command to create event processors that unpack event data.
 *  @note:  These event processors are registered with the pipeline manager
 *          for inclusion into other processing pipelines.
 */

/**
 * The base command is evbunpack.  This is an ensemble with the following
 * subcommands:
 *
 * \literal
 *    evbunpack create name mhz basename - Creates/registers a new event built data unpacker.
 *    evbunpack addprocessor name sid pipe-name
 *                               - Specifies pipe-name is used to process
 *                                  data from source id sid for the event processor
 *                                  name.
 *    evbunpack list ?pattern?   - lists the event processors we've registered
 * \endliteral
 */
#ifndef CUNPACKEVBCOMMAND_H
#define CUNPACKEVBCOMMAND_H
#include <TCLObjectProcessor.h>
#include <map>
#include <string>

class CEventBuilderEventProcessor;

class CUnpackEvbCommand : public CTCLObjectProcessor
{
private:
    typedef std::map<std::string, CEventBuilderEventProcessor*> ProcessorMap;
    ProcessorMap m_Processors;
    
public:
    CUnpackEvbCommand(CTCLInterpreter& interp);
    virtual ~CUnpackEvbCommand();
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
private:
    void create(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void addprocessor(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void Usage(CTCLInterpreter& interp, std::vector<CTCLObject>& objv, std::string msg);

};

#endif