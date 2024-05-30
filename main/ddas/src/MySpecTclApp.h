/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef MYSPECTCLAPP_H
#define MYSPECTCLAPP_H

// Required for the base class:
#include "TclGrammerApp.h"

//________________________________________________________________________
// The user creates this subclass and fills in the appropriate overrides for
// any additions they want to make. The class is a self contained example
// which registers an event processor
//
class CMySpecTclApp : public CTclGrammerApp {
public:
    /**
     * @brief Default constructor.
     * 
     * @details 
     * Default constructor alternative to compiler-provided default ctor.
     */
    CMySpecTclApp();
    /**
     * @brief Destructor.
     * 
     * @details
     * Delete any pointer data members that used new in ctors. The destructor 
     * should be virtual if and only if class contains at least one virtual 
     * function. Objects should be destroyed in the reverse order of the 
     * construction order.
     */
    ~CMySpecTclApp();
    
private:
    /** @brief Copy constructor. */
    CMySpecTclApp(const CMySpecTclApp& aCMySpecTclApp);

    // Operators:
    CMySpecTclApp& operator=(const CMySpecTclApp& aCMySpecTclApp);
    int operator==(const CMySpecTclApp& aCMySpecTclApp) const;

public:
    virtual void BindTCLVariables(CTCLInterpreter& rInterp);
    virtual void SourceLimitScripts(CTCLInterpreter& rInterpreter);
    virtual void SetLimits();
    virtual void CreateHistogrammer();
    virtual void SelectDisplayer(
	UInt_t nDisplaySize, CHistogrammer& rHistogrammer
	);
    virtual void SetupTestDataSource();
    virtual void CreateAnalyzer(CEventSink* pSink);
    virtual void SelectDecoder(CAnalyzer& rAnalyzer);
    virtual void CreateAnalysisPipeline(CAnalyzer& rAnalyzer);
    virtual void AddCommands(CTCLInterpreter& rInterp);
    virtual void SetupRunControl();
    virtual void SourceFunctionalScripts(CTCLInterpreter& rInterp);
    virtual int operator()();
};

#endif
