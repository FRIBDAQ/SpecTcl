#ifndef __CS800SPECTCLAPP_H
#define __CS800SPECTCLAPP_H

#ifndef __TCLGRAMMERAPP_H
#include "TclGrammerApp.h"
#endif
 
class CS800SpecTclApp  : public CTclGrammerApp        
{
public:
  CS800SpecTclApp ();    // Default constructor alternative to compiler
                       // provided default constructor
 ~ CS800SpecTclApp ();  //Destructor - Delete any pointer data members that used new in constructors
               //Destructor should be virtual if and only if class contains at least one virtual function
              //Objects destroyed in the reverse order of the construction order
private:
             //Copy Constructor 
  CS800SpecTclApp (const CS800SpecTclApp& aCS800SpecTclApp );

             //Operator= Assignment Operator 
  CS800SpecTclApp& operator= (const CS800SpecTclApp& aCS800SpecTclApp);
 
            //Operator== Equality Operator 
  int operator== (const CS800SpecTclApp& aCS800SpecTclApp) const;
public:

  // Class operations:

public:

  virtual   void BindTCLVariables (CTCLInterpreter& rInterp)   ; // 
  virtual   void SourceLimitScripts (CTCLInterpreter& rInterpreter)   ; // 
  virtual   void SetLimits ()   ; // 
  virtual   void CreateHistogrammer ()   ; // 
  virtual   void SelectDisplayer (UInt_t nDisplaySize, 
				  CHistogrammer& rHistogrammer)   ; // 
  virtual   void SetupTestDataSource ()   ; // 
  virtual   void CreateAnalyzer (CEventSink* pSink)   ; // 
  virtual   void SelectDecoder (CAnalyzer& rAnalyzer)   ; // 
  virtual   void CreateAnalysisPipeline (CAnalyzer& rAnalyzer)   ; // 
  virtual   void AddCommands (CTCLInterpreter& rInterp)   ; // 
  virtual   void SetupRunControl ()   ; // 
  virtual   void SourceFunctionalScripts (CTCLInterpreter& rInterp)   ; // 
  virtual   int operator() ()   ; // 

};

#endif
