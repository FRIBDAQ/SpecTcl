/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

// Class: CTclGrammerApp            //ANSI C++
// File: TclGrammerApp
/*
  Performs SpecTcl initialization.  The user extends this class to add their 
  own initialization.
*/
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef __TCLGRAMMERAPP_H  //Required for current class
#define __TCLGRAMMERAPP_H

#ifndef __TCLAPPLICATION_H
#include <TCLApplication.h>
#endif

#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __TCLVARIABLE_H
#include <TCLVariable.h>
#endif

#ifndef __ANALYZER_H
#include <Analyzer.h>
#endif

#include <tcl.h>

class CTCLInterpreter;
class CAnalyzer;
class CTCLHistogrammer;
class CHistogrammer;
class CEventSink;
class CEventSinkPipeline;
class CTKRunControl;
class CXamineEventHandler;
class CRunControlPackage;
class CXamineEventHandler;
class CRunControlPackage;
class CParameterPackage;
class CSpectrumPackage;
class CDataSourcePackage;
class CGatePackage;
class CEventProcessor;
class CNSCLBufferDecoder;
class CMultiTestSource;
class CDisplayInterface;
class CGatingDisplayObserver;

class CTclGrammerApp : public CTCLApplication {
  // Private Member data:
  UInt_t m_nDisplaySize;
  UInt_t m_nParams;
  UInt_t m_nListSize;
  std::string m_displayType;
  CAnalyzer* m_pAnalyzer;
  CTCLHistogrammer* m_pHistogrammer;
  CBufferDecoder* m_pDecoder; // Formerly declared as only an NSCL Buffer Decoder.
  CTKRunControl* m_pRunControl;
  CXamineEventHandler* m_pXamineEvents;
  CRunControlPackage* m_pRunControlPackage;
  CParameterPackage* m_pParameterPackage;
  CSpectrumPackage* m_pSpectrumPackage;
  CDataSourcePackage* m_pDataSourcePackage;
  CGatePackage* m_pGatePackage;
  CTCLVariable m_RCFile;
  CTCLVariable m_TclDisplaySize;
  CTCLVariable m_TclParameterCount;
  CTCLVariable m_TclEventListSize;
  CTCLVariable m_TclDisplayType;
  CMultiTestSource* m_pMultiTestSource;
  CDisplayInterface* m_pDisplayInterface;
  CGatingDisplayObserver* m_pGatingObserver;
  
  int m_nUpdateRate;

 public:
  //Default constructor alternative to compiler provided default constructor
  //Ensure correct initial values
  //Initialization std::list has members in the order declared
  //Association object data member pointers initialized to null association object
  CTclGrammerApp();
  virtual ~CTclGrammerApp();

  // Copy of this singleton is illegal so many canonical functions are
  // private and unimplemented to prevent their use.
 private:
  //Copy Constructor.
  CTclGrammerApp (const CTclGrammerApp& aCTclGrammerApp );

  //Operator= Assignment Operator 
  CTclGrammerApp& operator= (const CTclGrammerApp& aCTclGrammerApp);

  //Operator== Equality Operator 
  int operator== (const CTclGrammerApp& aCTclGrammerApp) const;

  // Selectors:
 public:
  //Get accessor function for non-static attribute data member
  UInt_t getDisplaySize() const {
    return m_nDisplaySize;
  }
  //Get accessor function for non-static attribute data member
  UInt_t getParams() const {
    return m_nParams;
  }
  //Get accessor function for non-static attribute data member
  UInt_t getListSize() const {
    return m_nListSize;
  }
  //Get accessor function for non-static attribute data member
  CAnalyzer* getAnalyzer() const {
    return m_pAnalyzer;
  }
  //Get accessor function for non-static attribute data member
  CTCLHistogrammer* getHistogrammer() const {
    return m_pHistogrammer;
  }
  //Get accessor function for non-static attribute data member
  CTKRunControl* getRunControl() const {
    return m_pRunControl;
  }
  //Get accessor function for non-static attribute data member
  CXamineEventHandler* getXamineEvents() const {
    return m_pXamineEvents;
  }
  //Get accessor function for non-static attribute data member
  CRunControlPackage* getRunControlPackage() const {
    return m_pRunControlPackage;
  }
  //Get accessor function for non-static attribute data member
  CParameterPackage* getParameterPackage() const {
    return m_pParameterPackage;
  }
  //Get accessor function for non-static attribute data member
  CSpectrumPackage* getSpectrumPackage() const {
    return m_pSpectrumPackage;
  }
  //Get accessor function for non-static attribute data member
  CDataSourcePackage* getDataSourcePackage() const {
    return m_pDataSourcePackage;
  }
  //Get accessor function for non-static attribute data member
  CGatePackage* getGatePackage() const {
    return m_pGatePackage;
  }
  //Get accessor function for non-static attribute data member
  CTCLVariable getRCFile() const {
    return m_RCFile;
  }
  //Get accessor function for non-static attribute data member
  CTCLVariable getTclDisplaySize() const {
    return m_TclDisplaySize;
  }
  //Get accessor function for non-static attribute data member
  CTCLVariable getTclParameterCount() const {
    return m_TclParameterCount;
  }
  //Get accessor function for non-static attribute data member
  CTCLVariable getTclEventListSize() const {
    return m_TclEventListSize;
  }

  CMultiTestSource* getTestDataSource() {
    return m_pMultiTestSource;
  }

  // Attribute mutators:
 protected:
  //Set accessor function for non-static attribute data member
  //Set accessor function for non-static attribute data member
  void setDisplaySize (const UInt_t am_nDisplaySize) {
    m_nDisplaySize = am_nDisplaySize;
  }
  //Set accessor function for non-static attribute data member
  void setParams (const UInt_t am_nParams) {
    m_nParams = am_nParams;
  }
  //Set accessor function for non-static attribute data member
  void setListSize (const UInt_t am_nListSize) {
    m_nListSize = am_nListSize;
  }
  //Set accessor function for non-static attribute data member
  void setAnalyzer (CAnalyzer* am_pAnalyzer) {
    m_pAnalyzer = am_pAnalyzer;
  }
  //Set accessor function for non-static attribute data member
  void setHistogrammer (CTCLHistogrammer* am_pHistogrammer) {
    m_pHistogrammer = am_pHistogrammer;
  }
  //Set accessor function for non-static attribute data member
  void setRunControl (CTKRunControl* am_pRunControl) {
    m_pRunControl = am_pRunControl;
  }
  //Set accessor function for non-static attribute data member
  void setXamineEvents (CXamineEventHandler* am_pXamineEvents) {
    m_pXamineEvents = am_pXamineEvents;
  }
  //Set accessor function for non-static attribute data member
  void setRunControlPackage (CRunControlPackage* am_pRunControlPackage) {
    m_pRunControlPackage = am_pRunControlPackage;
  }
  //Set accessor function for non-static attribute data member
  void setParameterPackage (CParameterPackage* am_pParameterPackage) {
    m_pParameterPackage = am_pParameterPackage;
  }
  //Set accessor function for non-static attribute data member
  void setSpectrumPackage (CSpectrumPackage* am_pSpectrumPackage) {
    m_pSpectrumPackage = am_pSpectrumPackage;
  }
  //Set accessor function for non-static attribute data member
  void setDataSourcePackage (CDataSourcePackage* am_pDataSourcePackage) {
    m_pDataSourcePackage = am_pDataSourcePackage;
  }
  //Set accessor function for non-static attribute data member
  void setGatePackage (CGatePackage* am_pGatePackage) {
    m_pGatePackage = am_pGatePackage;
  }
  //Set accessor function for non-static attribute data member
  void setRCFile (const CTCLVariable am_RCFile) {
    m_RCFile = am_RCFile;
  }
  //Set accessor function for non-static attribute data member
  void setTclDisplaySize (const CTCLVariable am_TclDisplaySize) {
    m_TclDisplaySize = am_TclDisplaySize;
  }
  //Set accessor function for non-static attribute data member
  void setTclParameterCount (const CTCLVariable am_TclParameterCount) {
    m_TclParameterCount = am_TclParameterCount;
  }
  //Set accessor function for non-static attribute data member
  void setTclEventListSize (const CTCLVariable am_TclEventListSize) {
    m_TclEventListSize = am_TclEventListSize;
  }


  // Class operations:
 public:
  void RegisterEventProcessor(CEventProcessor& rEventProcessor,
			      const char* name = 0); // Add event processor to pipeline tail
  virtual void BindTCLVariables(CTCLInterpreter& rInterp); // Bind any CTCLVariables to interpreter.
  virtual void SourceLimitScripts(CTCLInterpreter& rInterpreter); // Source variable definition scripts.
  virtual void SetLimits(); // Finalize limit variables.
  virtual void CreateHistogrammer(); // Link Histogramer sink to SpecTcl.
  virtual void CreateDisplays(); // Create the displays that can be used.
  virtual void SelectDisplayer(); // Link Histogrammer to Displayer
  virtual void SetUpDisplay(); // Add observers to the displayer
  virtual void SetupTestDataSource(); // Set up test data source.
  virtual void CreateAnalyzer(CEventSink* pSink); // Create the event analyzer.
  virtual void SelectDecoder(CAnalyzer& rAnalyzer); // Select Buffer Decoder.
  virtual void CreateAnalysisPipeline(CAnalyzer& rAnalyzer) = 0; // Create the event pipeline.
  virtual void AddCommands(CTCLInterpreter& rInterp); // 
  void SetupRunControl(); // 
  virtual void SourceFunctionalScripts(CTCLInterpreter& rInterp); // Do functional Tcl initialization.
  virtual int operator()(); // SpecTcl entry point.

  // Utilities:
 protected:  
  static void UpdateUInt(CTCLVariable& rVar, UInt_t& rValue);
  static void UpdateString(CTCLVariable& rVar, std::string& rString);
  static std::string SourceOptionalFile(CTCLInterpreter& rInterp, std::string filename);
private:
  static void TimedUpdates(ClientData d);
  void protectVariable(CTCLInterpreter* pInterp, const char* pVarName);
};

#endif
