// Class: CA1900SpecTcl            //ANSI C++
// File: a1900SpecTcl.h
/*
The user creates this subclass and fills in the appropriate overrides for any 
additions they want to make.  The class is a self contained example which 
registers two event processors.  One which unpacks a simple fixed length
event and another which produces a pseudo parameter from the sum of 
the first two parameters in an event.
*/
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 
// Modified by Daniel Bazin July 2001

#ifndef __A1900SPECTCL_H  //Required for current class
#define __A1900SPECTCL_H

//
// Include files:
//

                               //Required for base classes
#ifndef __TCLGRAMMERAPP_H     //CTclGrammerApp
#include "TclGrammerApp.h"
#endif
 
class CA1900SpecTcl  : public CTclGrammerApp        
{
public:
             //Default constructor alternative to compiler provided default constructor
  CA1900SpecTcl ();
 ~ CA1900SpecTcl ( );  //Destructor - Delete any pointer data members that used new in constructors
               //Destructor should be virtual if and only if class contains at least one virtual function
              //Objects destroyed in the reverse order of the construction order
private:
             //Copy Constructor 
  CA1900SpecTcl (const CA1900SpecTcl& aCA1900SpecTcl );

             //Operator= Assignment Operator 
  CA1900SpecTcl& operator= (const CA1900SpecTcl& aCA1900SpecTcl);
 
            //Operator== Equality Operator 
  int operator== (const CA1900SpecTcl& aCA1900SpecTcl) const;
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
