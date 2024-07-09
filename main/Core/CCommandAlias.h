/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2024

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Aaron Chester
             Simon Giraud
             Giordano Cerizza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#ifndef CCOMMANDALIAS_H
#define CCOMMANDALIAS_H

#define protected public
#include <TCLObjectProcessor.h>
#include <TCLPackagedCommand.h>
#include <TCLPackagedObjectProcessor.h>
#include <TCLPackagedCommand.h>
#undef protected


class CTCLCommandPackage;

/**
 * CCommandAlias
 *    Provides a command alias for a CTCLOBjectProcessor derived command.
 * This is really just a jacket wrapper for some other CTCLObjectProcessor:
 */
class CCommandAlias : public CTCLObjectProcessor {
private:
    CTCLObjectProcessor* m_pActual;
public:
    CCommandAlias(CTCLInterpreter* pInterp, const char* alias, CTCLObjectProcessor* pActual);
    virtual ~CCommandAlias();

    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};


/**
 * CPackagedCommandAlias
 *    Provides a command alias for a packaged command.  Again, this is just a wrapper.
 */
class CPackagedCommandAlias : public CTCLPackagedCommand {
private:
    CTCLPackagedCommand* m_pActual;
public:
    CPackagedCommandAlias(
        CTCLInterpreter& pInterp, const char* alias, 
        CTCLPackagedCommand* pActual, CTCLCommandPackage& package
    );
    virtual ~CPackagedCommandAlias();

    virtual   int operator() (
        CTCLInterpreter& rInterpreter, CTCLResult& rResult, int nArguments, char* pArguments[]
    );


};

/**
 *  CPackagedObjectProcessorAlias
 *     Provides a command alias for a CTCLPackagedObjectProcessor.
 */
class CPackagedObjectProcessorAlias : public CTCLPackagedObjectProcessor {
private:
    CTCLPackagedObjectProcessor* m_pActual;
public: 
    CPackagedObjectProcessorAlias(
        CTCLInterpreter&  interp, std::string alias, CTCLPackagedObjectProcessor* pActual
    );
    virtual ~CPackagedObjectProcessorAlias();

    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};
#endif