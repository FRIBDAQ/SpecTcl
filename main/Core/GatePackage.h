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


/*
  Change Log:
  $Log$
  Revision 5.2  2005/06/03 15:19:26  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.2  2005/05/23 15:47:29  thoagland
  Added Support to allow users to filter "gate -list" by a pattern

  Revision 5.1.2.1  2004/12/15 17:24:05  ron-fox
  - Port to gcc/g++ 3.x
  - Recast swrite/sread in terms of tcl[io]stream rather than
    the kludgy thing I had done of decoding the channel fd.
    This is both necessary due to g++ 3.x's runtime and
    nicer too!.

  Revision 5.1  2004/11/29 16:56:10  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.2  2003/04/15 19:25:20  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs. Modifications support the input, listing and application information when gate coordinates are floating point.

*/


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

#ifndef GATEPACKAGE_H  //Required for current class
#define GATEPACKAGE_H

                               //Required for base classes
#include "TCLCommandPackage.h"
#include "GateCommand.h"
#include "ApplyCommand.h"
#include "UngateCommand.h"
#include <histotypes.h>        //Required for include files
#include <Histogrammer.h>        //Required for include files
#include <GateContainer.h>        //Required for include files
#include <Gate.h>        //Required for include files
#include <SpectrumPackage.h>        //Required for include files
#include <vector>        //Required for include files
#include <string>        //Required for include files
                               //Required for 1:1 association classes
                                                             
/*!

 Encapsulates a package of commands which
 manipulates gates.  This class also provides
 services which bridge between the Tcl
 scripting language and the underlying 
 representation of gates etc.
*/
class CGatePackage  : public CTCLCommandPackage        
{                       
			
  CHistogrammer*  m_pHistogrammer;   //!< Pointer to SpecTcl Histogrammer        
  CGateCommand*   m_pGateCommand;    // 1:1 association object data member
  CApplyCommand*  m_pApplyCommand;   // 1:1 association object data member
  CUngateCommand* m_pUngateCommand;  // 1:1 association object data member
  static UInt_t   m_nNextId;	     //!< Next Gate Id.

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

   Bool_t AddGate (CTCLResult& rResult,
                   const std::string& rGateName, 
		   const CGate* pGate);
   CTCLString ListGates(const char* pattern);
   CTCLString ListGatesById(const char* pattern);
   Bool_t DeleteGates (CTCLResult& rResult, 
		       const std::vector<std::string>& rGateNames);
   Bool_t DeleteGates (CTCLResult& rResult, 
                       const std::vector<UInt_t>& rIds)    ;
   Bool_t ApplyGate (CTCLString& rResult, const std::string& rGateName, 
		     const std::string& rSpectrumName);
   Bool_t ListAppliedGate (CTCLString& rApplication, 
                           const std::string& rName)    ;
   Bool_t  Ungate(CTCLString& rResult, const std::string & rName);

   std::string GateToString (CGateContainer* pGate);
   static  UInt_t AssignId();
protected:
   static  void SortGateListById (std::vector<CGateContainer*>& rpGates);
private:

};

#endif
