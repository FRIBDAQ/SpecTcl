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

/** @file:  CAnalyzeCommand.h
 *  @brief: Defines the 'analyze' command that analyzes data.
 */
#ifndef CANALYZECOMMAND_H
#define CANALYZECOMMAND_H
#include <TCLObjectProcessor.h>

class CDataGetter;
class CDataDistributor;

/**
 *  Provides a command to run analysis from a
 *  data getter to a data distributor.
 *  @note While this is a TCL command it's setup as a singleton
 *         so that commands that set the data getter and data distributor
 *         can find it.
 *
 *         Prior to the first 'getInstance()' call gpInterpreter
 *         must have been set.
 *
 */
class CAnalyzeCommand : public CTCLObjectProcessor
{
private:
    static CAnalyzeCommand*  m_pInstance;
    static CDataGetter*      m_pDataSource;
    static CDataDistributor* m_pDataSink;

    // Singletons have private constructors:
    
    CAnalyzeCommand(CTCLInterpreter& interp);   // Fixed command 'analyze'.
    
public:
    static CAnalyzeCommand* getInstance();
    static void   setDataGetter(CDataGetter* pGetter);
    static void   setDistributor(CDataDistributor* pDistrib);
    
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

};


#endif