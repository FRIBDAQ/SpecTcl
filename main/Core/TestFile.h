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

//  CTestFile.h:
//
//    This file defines the CTestFile class.  Usage notes:  The
//    list of distributions is an array of pointers this implies that the 
//    distributions must be alive for the lifetime of the object.
//    There are a few ways to do this and clean up nicely:
//
//    1. Distributions declared explicitly in a scope which is outer 
//       to the scope in which the CTestFile is declared.
//    2. Derive from TestFile and include the distributions in the
//       derivation.. inserting them into the vector.  Note you'll have to
//       be careful in copy constructors and assignments to copy the 
//       distributions and re-insert in the lhs.
//    3. Dynamically allocate distributions and maintain them as 
//       reference counted pointers in an object from a class derived
//       from CTestFile... e.g. when the last CTestFile goes away, so does
//       the dynamically allocated distribution.
//    4. Be very careful about when to delete stuff.
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

#ifndef TESTFILE_H  // Required for current class
#define TESTFILE_H

#include "File.h"
#include "Distribution.h"
#include <histotypes.h>
#include <vector>
#include <string>

typedef std::vector<CDistribution*> CDistributionList;
typedef CDistributionList::iterator CDistributionIterator;

class CTestFile : public CFile {
  // Attributes:
 private:
  CDistributionList m_vDistributions;

 public:
  // Constructors:
  CTestFile() {}
  ~CTestFile() ;


  CTestFile(const CTestFile& aCTestFile) {
    DoAssign(aCTestFile);
  }
  
  // Operator= Assignment Operator
  // Update to access 1:M part class attributes
  // Update to access 1:1 associated class attributes
  // Update to access 1:M associated class attributes
  CTestFile& operator=(const CTestFile& aCTestFile) {
    if(this != &aCTestFile) {
      CFile::operator=(aCTestFile);
    }
    DoAssign(aCTestFile);
    return *this;
  }

  // Operator== Equality Operator
  int operator==(const CTestFile& aCTestFile);
  int operator!=(const CTestFile& acTestFile) {
    return !(operator==(acTestFile));
  }

  // Operations:
 public:
  virtual Int_t Read(Address_t pBuffer, UInt_t nSize);
  virtual Int_t Write(const Address_t pBuffer, UInt_t nBytes);
  virtual void Open(const STD(string)& rsFilename, UInt_t nAccess, bool unused=true);
  virtual void Close();

  // operations which are specific to test files:
 public:
  UInt_t GetDistributionCount() const;
  void AddDistribution(CDistribution& rDistribution);
  void ClearDistribution(UInt_t nDistribution)  ;
  void ClearDistributions();
  CDistributionIterator begin();
  CDistributionIterator end();

  // utility functions:
  void DoAssign(const CTestFile& rhs);
  UInt_t FormatNSCLHeader(Address_t pBuffer);
  UInt_t NSCLHeaderSize() const { // # bytes in a header.
    return 32;
  }
  virtual UInt_t FormatEvent(Address_t pBuffer);
  virtual UInt_t EventSize() const;
  void SetEventCount(Address_t pBuffer, UInt_t nEvents);
  void SetBufferSize(Address_t pBuffer, UInt_t nBytes);
};

#endif
