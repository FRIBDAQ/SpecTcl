//  DataSourcePackage.h:
//
//    This file defines the CDataSourcePackage class.
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
//  Change Log:
//     July 14, 1999 Ron Fox
//        Removed support for online source and replaced it with support for
//        PIPE data source.
//
/////////////////////////////////////////////////////////////

#ifndef __DATASOURCEPACKAGE_H  //Required for current class
#define __DATASOURCEPACKAGE_H
                               //Required for base classes
#ifndef __TCLCOMMANDPACKAGE_H
#include "TCLCommandPackage.h"
#endif                               

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif



/* Forward Class reference: */

class CTapeCommand;
class CAttachCommand;
class CTCLInterpreter;
class CTCLResult;


/* Class definition: */
                                                               
class CDataSourcePackage  : public CTCLCommandPackage        
{
public:
  // Data type definitions.

  enum SourceType_t {		// Types of data sources:
    kTestSource,
    kFileSource,
    kTapeSource,
    kPipeSource
  };
private:

  // Attributes:

  SourceType_t    m_eSourceType;  // Type of data source connected
  CTapeCommand*   m_pTape;
  CAttachCommand* m_pAttach;

public:
			//Default constructor

  CDataSourcePackage (CTCLInterpreter* pInterp);
  virtual ~CDataSourcePackage ( ) {}            //Destructor

			//Copy constructor [illegal]
private:
  CDataSourcePackage (const CDataSourcePackage& aCDataSourcePackage );
public:

			//Operator= Assignment Operator [illegal]
private:
  CDataSourcePackage& operator= 
                         (const CDataSourcePackage& aCDataSourcePackage);
public:

			//Operator== Equality Operator senseless but legal.

  int operator== (const CDataSourcePackage& aCDataSourcePackage)
  { 
    return (
	    (CTCLCommandPackage::operator== (aCDataSourcePackage)) &&
	    (m_eSourceType == aCDataSourcePackage.m_eSourceType) 
	    );
  }                             
  // Selectors:

public:
  SourceType_t getSourceType() const
  {
    return m_eSourceType;
  }
                       
                       //Get accessor function for 1:1 association
  CTapeCommand* getTapeCommand() const
  {
    return m_pTape;
  }

                       //Get accessor function for 1:1 association
  CAttachCommand* getAttachCommand() const
  {
    return m_pAttach;  
  }
  // Mutators:

protected:
  void setSourceType (SourceType_t am_eSourceType)
  { 
    m_eSourceType = am_eSourceType;
  }

                       //Set accessor function for 1:1 association
  void setTapeCommand (CTapeCommand* pTape)
  { 
    m_pTape= pTape;
  }

                       //Set accessor function for 1:1 association
  void setAttachCommand (CAttachCommand* pAttach)
  {
    m_pAttach = pAttach;
  }
  //
  //   Operations:
  //
public:
  int AttachFileSource (CTCLResult& rResult)  ;
  int AttachTapeSource (CTCLResult& rResult, const char* pDevice)  ;
  int AttachPipeSource (CTCLResult& rResult)  ;
  int OpenSource (CTCLResult& rResult, const char*  pConnectionString, 
		  UInt_t nBufferSize)  ;
  int CloseSource (CTCLResult& rResult)  ;
  int OpenNextTapeFile (CTCLResult& rResult)  ;
  int RewindTape (CTCLResult& rResult)  ;

  // Simple pseudo selectors.
  //
public:
  Bool_t isTape ()  
  { return m_eSourceType == kTapeSource; }
  
  Bool_t isFile () 
  { return m_eSourceType == kFileSource; }
  
  Bool_t isPipe () 
  { return m_eSourceType == kPipeSource; }

  Bool_t IsTest ()  
  { return m_eSourceType == kTestSource; }
  
  Bool_t isOpen ();
  

};

#endif


