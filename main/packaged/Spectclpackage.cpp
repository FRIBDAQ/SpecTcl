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

/** @file:  Spectclpackage.cpp
 *  @brief: Contains package initialization code for SpecTcl as a loadable
 *          package.
 */
#include <tcl.h>
#include <SpecTcl.h>

#include <Globals.h>

// Commands we're going to import

#include <ParameterPackage.h>
#include <SpectrumPackage.h>
#include <GatePackage.h>
#include <FilterCommand.h>
#include <CSpectrumStatsCommand.h>
#include <CTreeParameterCommand.h>
#include <CTreeVariableCommand.h>
#include <CTreeParameter.h>
#include <CTreeVariable.h>
#include <CFoldCommand.h>
#include <CFitCommand.h>
#include <CProjectionCommand.h>
#include <IntegrateCommand.h>
#include <VersionCommand.h>
#include <SContentsCommand.h>
#include <CPipelineCommand.h>
#include <CUnpackEvbCommand.h>
#include <ProductionXamineShMem.h>

#include "CAnalyzeCommand.h"

#include <SpecTclDisplayManager.h>
#include <TCLAnalyzer.h>
#include <TCLHistogrammer.h>
#include <TCLAnalyzer.h>
#include <CRingBufferDecoder.h>
#include <RingFormatHelperFactory.h>
#include <RingFormatHelper.h>



#include <tcl.h>


static const char* kpInstalledBase = INSTALLED_IN; // Preprocessor def.
void* gpApplication(nullptr);


extern "C" {
    int Spectcl_Init(Tcl_Interp* pRawInterp)
    {
        //Tcl_Init(pRawInterp);
        
        
        Tcl_PkgProvide(pRawInterp, "spectcl", "1.0");
        
        CTCLInterpreter* pInterp = new CTCLInterpreter(pRawInterp);
        
        // Create enough of the SpecTcl scaffolding that none of the
        // commands dies.
        
        gpInterpreter = pInterp;
        gpEventSink = new CTCLHistogrammer(pInterp);
        gpEventSinkPipeline = new CEventSinkPipeline;
        gpEventSinkPipeline->AddEventSink(*gpEventSink, "::Histogrammer");
        gpDisplayInterface = new CSpecTclDisplayInterface;
        gpDisplayInterface->setCurrentDisplay("none");
        gpAnalyzer = new CTclAnalyzer(*pInterp, 100, 1);
        
        
        CRingBufferDecoder *pDecoder = new CRingBufferDecoder;
        gpBufferDecoder = pDecoder;
        CRingFormatHelperFactory* pFact = pDecoder->getFormatFactory();
        pDecoder->setFormatHelper(pFact->create(11, 0));    // 11.0 format.
        gpAnalyzer->AttachDecoder(*pDecoder);
        
        
        // Add the SpecTcl Commands
        
        (new CParameterPackage(pInterp, (CTCLHistogrammer*)gpEventSink))->Register();

        (new CSpectrumPackage(
            pInterp, (CHistogrammer*)gpEventSink,gpDisplayInterface)
        )->Register();
        (new CGatePackage(pInterp, (CHistogrammer*)gpEventSink))->Register();

      
        new CTreeParameterCommand;
        new CTreeVariableCommand;
        CTreeVariable::BindVariables(*pInterp);      
        new CFoldCommand(pInterp);
        new CProjectionCommand(*pInterp);
        new CFitCommand(*pInterp);
        new CIntegrateCommand(*pInterp);        
        new CVersionCommand(*pInterp);
        new CSContentsCommand(*pInterp);
        new CSpectrumStatsCommand(*pInterp);      
        new CPipelineCommand(*pInterp);
        new CUnpackEvbCommand(*pInterp);
        CAnalyzeCommand::getInstance();         // installs the command.
        
        return TCL_OK;
    }
}
