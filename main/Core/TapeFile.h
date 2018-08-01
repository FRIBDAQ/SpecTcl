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


//  CTapeFile.h:
//
//    This file defines the CTapeFile class.
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

#ifndef TAPEFILE_H  //Required for current class
#define TAPEFILE_H
                               //Required for base classes
#include "File.h"
#include "Refptr.h"
#include <mtaccess.h>
#include <string>

                               
class CTapeFile  : public CFile        
{
  std::string m_sDevice;  //  Name of the tape device (e.g. /dev/rmt0h
  UInt_t m_nBlocksize;    //  Bytes in block on tape (set by open),
                          //  used to set size on Open.
  CRefcountedPtr<volume> m_pVcb;// Shared volume context between all CTapeFile
				// objects for the same tape file.
  std::string m_sFilename;	// File name when one is open.
public:
			//Constructor with arguments

  CTapeFile (const std::string& am_sDevice);
  virtual ~ CTapeFile ( );          //Destructor
	
			//Copy constructor

  CTapeFile (const CTapeFile& aCTapeFile );


			//Operator= Assignment Operator

  CTapeFile& operator= (const CTapeFile& aCTapeFile);


			//Operator== Equality Operator

  Int_t operator== (const CTapeFile& aCTapeFile);
  Int_t operator!= (const CTapeFile& aCTapeFile) {
    return !(operator==(aCTapeFile));
  }

  // Read selectors:
public:
                       //Get accessor function for attribute
  UInt_t getBlocksize() const
  {
    return m_nBlocksize;
  }
  std::string getDevice() const
  {
    return m_sDevice;
  }
  volume* getVCB()
  {
    return m_pVcb.operator->();
  }
  std::string getFilename() const
  {
    AssertOpen();
    return m_sFilename;
  }

  // Use this with care: Just befofe a creating open this
  // sets the blocksize for the new file.
  //   e.g. 
  //      tape.setBlocksize(8192);
  //      tape.Open("NEWFILE", kacCreate | kackWrite);
  //   creates a file with an 8192byte block.
  //  

  void setBlocksize (UInt_t am_nBlocksize)
  { 
    m_nBlocksize = am_nBlocksize;
  }
  
  // Mutating (Write) selectors:
protected:

  void setDevice (const std::string& am_sDevice)
  { 
    m_sDevice = am_sDevice;
  }
//
//  void setVCB(volume* pVCB) 
//  {
//    m_pVcb = CRefcountedPtr<volume>(*pVCB) 
//				// Point to new Volume control block.
//  }
  void setFilename(const std::string& sName)
  {
    m_sFilename = sName;
  }
  //
  // operations:
  //
public:                   
  virtual   int Read (Address_t pBuffer, UInt_t nBytes)  ;
  virtual   Int_t Write (const Address_t pBuffer, UInt_t  nBytes)  ;
  virtual   void Open (const std::string&  rsFilename, UInt_t nAccess)  ;
  virtual   void Close ()  ;
  static void      Initialize(const std::string& device, // Initialize tape 
			      const std::string& label); // volume.

  //  Utility functions:

protected:
  void DoAssign(const CTapeFile& rhs);
  void TapeCreate(const std::string& name);
  void TapeOpen(const std::string& name);
};


#endif
