//  CTCLPackagedCommand.h:
//
//    This file defines the CTCLPackagedCommand class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __TCLPACKAGEDCOMMAND_H  //Required for current class
#define __TCLPACKAGEDCOMMAND_H
                               
                               //Required for 1:1 associated classes
#ifndef __TCLCOMMANDPACKAGE_H
#include "TCLCommandPackage.h"
#endif                                                               

#ifndef __TCLPROCESSOR_H
#include "TCLProcessor.h"
#endif                                                  
             
class CTCLPackagedCommand   : public CTCLProcessor
{
  
  CTCLCommandPackage& m_rMyPackage;
  
public:

			//Constructor with arguments
  CTCLPackagedCommand (const std::string& sCommand, CTCLInterpreter* pInterp,
		       CTCLCommandPackage& rPackage) :
    CTCLProcessor(sCommand, pInterp),
    m_rMyPackage(rPackage)
  { }        
  CTCLPackagedCommand (const char* pCommand, CTCLInterpreter* pInterp,
		       CTCLCommandPackage& rPackage) :
    CTCLProcessor(pCommand, pInterp),
    m_rMyPackage(rPackage)
  { }        
   ~ CTCLPackagedCommand ( ) { }       //Destructor
	
			//Copy constructor [illegal]
private:
  CTCLPackagedCommand (const CTCLPackagedCommand& aCTCLPackagedCommand ) ;
public:
                        // Assignment operator [illegal]
private:
  CTCLPackagedCommand& operator= 
      (const CTCLPackagedCommand& aCTCLPackagedCommand);
public:

			//Operator== Equality Operator
                        // Nonsense maybe, but provided anyway.

  int operator== (const CTCLPackagedCommand& aCTCLPackagedCommand)
  { return ( CTCLProcessor::operator==(aCTCLPackagedCommand) &&
	    (m_rMyPackage == aCTCLPackagedCommand.m_rMyPackage)

	     );
  }                             
  // Selectors:

public:
                       //Get accessor function for 1:1 association
  CTCLCommandPackage& getMyPackage()
  {
    return m_rMyPackage;
  }
  // Mutators
                       
public:
  void setMyPackage (CTCLCommandPackage& am_rMyPackage)
  { 
    m_rMyPackage = am_rMyPackage;
  }


};

#endif
