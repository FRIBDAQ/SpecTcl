// Class: CGatePackage                     //ANSI C++
//
// Encapsulates a package of commands which
// manipulates gates.  This class also provides
// services which bridge between the Tcl
// scripting language and the underlying 
// representation of gates etc.
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved GatePackage.h
//

#ifndef __GATEPACKAGE_H  //Required for current class
#define __GATEPACKAGE_H

                               //Required for base classes
#ifndef __TCLCOMMANDPACKAGE_H
#include "TCLCommandPackage.h"
#endif

#ifndef __GATECOMMAND_H
#include "GateCommand.h"
#endif

                               //Required for 1:1 association classes
#ifndef __APPLYCOMMAND_H
#include "ApplyCommand.h"
#endif

                               //Required for 1:1 association classes
#ifndef __UNGATECOMMAND_H
#include "UngateCommand.h"
#endif


#ifndef __HISTOTYPES_H                               
#include <histotypes.h>        //Required for include files
#endif

#ifndef __HISTOGRAMMER_H
#include <Histogrammer.h>        //Required for include files
#endif

#ifndef __GATECONTAINER_H
#include <GateContainer.h>        //Required for include files
#endif

#ifndef __GATE_H
#include <Gate.h>        //Required for include files
#endif

#ifndef __SPECTRUMPACKAGE_H
#include <SpectrumPackage.h>        //Required for include files
#endif

#ifndef __STL_VECTOR
#include <vector>        //Required for include files
#define __STL_VECTOR
#endif

#ifndef __STL_STRING
#include <string>        //Required for include files
#define __STL_STRING
#endif                               
                               //Required for 1:1 association classes
                                                             
class CGatePackage  : public CTCLCommandPackage        
{                       
			
  CHistogrammer*  m_pHistogrammer;   // Pointer to SpecTcl Histogrammer        
  CGateCommand*   m_pGateCommand;    // 1:1 association object data member
  CApplyCommand*  m_pApplyCommand;   // 1:1 association object data member
  CUngateCommand* m_pUngateCommand;  // 1:1 association object data member
  static UInt_t   m_nNextId;	     // Next Gate Id.

public:

  // Class constructor (with arguments only allowed).

  CGatePackage(CTCLInterpreter* pInterp, CHistogrammer* pHistogrammer);
   ~ CGatePackage ( );		// Destructor.

   //Copy constructor alternative to compiler provided default copy constructor
  //    Copy construction is illegal due to pointer containment.
private:
  CGatePackage (const CGatePackage& aCGatePackage );
public:

   // Operator= Assignment Operator alternative to compiler provided 
   // default operator= [illegal]

private:
  CGatePackage& operator= (const CGatePackage& aCGatePackage);
public:
   //Operator== Equality Operator - No compiler default operator== generated
   // This is legal but not worth much.
  //

  int operator== (const CGatePackage& aCGatePackage) {
    return (CTCLCommandPackage::operator==(aCGatePackage)          &&
	    (m_pHistogrammer  == aCGatePackage.m_pHistogrammer)   &&
	    (m_pGateCommand   == aCGatePackage.m_pGateCommand)    &&
	    (m_pUngateCommand == aCGatePackage.m_pUngateCommand)  &&
	    (m_nNextId        == aCGatePackage.m_nNextId));
  }
	
// Selectors:

public:

  CHistogrammer* getHistogrammer()
  { 
    return m_pHistogrammer;
  }
                       
  const CGateCommand* getGateCommand() const
  {
    return m_pGateCommand;
  }
  const CApplyCommand* getApplyCommand() const
  { 
    return m_pApplyCommand;
  }
  const CUngateCommand* getUngateCommand() const
  { 
    return m_pUngateCommand;
  }
  UInt_t getNextId() const {
    return m_nNextId;
  }
                       
// Mutators:

protected:

  void setHistogrammer (CHistogrammer* am_pHistogrammer)
  { 
    m_pHistogrammer = am_pHistogrammer;
  }
       
  void setGateCommand (CGateCommand* p)
  {
    m_pGateCommand = p;
  }
  void setApplyCommand (CApplyCommand* p)
  {
    m_pApplyCommand = p;
  }
  void setUngateCommand (CUngateCommand* p)
  {
    m_pUngateCommand = p;
  }
  void setNextId(UInt_t n) {
    m_nNextId = n;
  }

  // Class operations:

public:

   Bool_t AddGate (CTCLResult& rResult, const std::string& rGateName, 
		   const CGate* pGate)    ;
   CTCLString ListGates ()    ;
   CTCLString ListGatesById ()    ;
   Bool_t DeleteGates (CTCLResult& rResult, 
		       const vector<string>& rGateNames)    ;
   Bool_t DeleteGates (CTCLResult& rResult, const vector<UInt_t>& rIds)    ;
   Bool_t ApplyGate (CTCLString& rResult, const string& rGateName, 
		     const string& rSpectrumName)    ;
   Bool_t ListAppliedGate (CTCLString& rApplication, const string& rName)    ;
   Bool_t  Ungate(CTCLString& rResult, const string & rName);

   std::string GateToString (CGateContainer* pGate)    ;
   static  UInt_t AssignId();
protected:
   static  void SortGateListById (vector<CGateContainer*>& rpGates)   ;
private:

};

#endif
