// Class: CReadCommand                     //ANSI C++
//
// Implements the sread command.  sread reads spectra 
// from a file.
//   CSpectrumFormatters and
//   the CSpectrumFormatterFactory
//  allows the format of the file to be
//  easily extended by the user.
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
//
// 
// Modified:
//    Chase Bolen (added stream map functions CheckIn, CheckOut, etc.) 4/14/2000 
//
// (c) Copyright NSCL 1999, All rights reserved ReadCommand.h
//

#ifndef __READCOMMAND_H  //Required for current class
#define __READCOMMAND_H

                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif

#include <fstream.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>

// Forward class references.

class CTCLInterpeter;
class CTCLCommandPackage;                               

//

class CReadCommand  : public CTCLPackagedCommand        
{                       
  // Data types.
public:
  typedef enum _Switch_t {
    keFormat,
    keSnapshot, keNoSnapshot,
    keReplace,  keNoReplace,
    keBind,     keNoBind,
    keNotSwitch
  } Switch_t;

public:

   // Constructors and other cannonical operations:

  CReadCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPack)  :
    CTCLPackagedCommand("sread", pInterp, rPack)
  { 
  } 
  ~ CReadCommand ( )  // Destructor 
  { }  


  
   //Copy constructor (illegal)

private:
  CReadCommand (const CReadCommand& aCReadCommand );
public:

   // Operator= Assignment Operator (illegal)

private:
  CReadCommand& operator= (const CReadCommand& aCReadCommand);
public:

   //Operator== Equality Operator 

  int operator== (const CReadCommand& aCReadCommand) const;
	
  // Operations:
       
public:

 virtual   Int_t operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			     int nArgs, char* pArgs[])    ;
 
  // Utilities.

protected:
  static void     Usage(CTCLResult& rResult);
  static Switch_t NextSwitch(const char* pText);
 private:
  struct StreamEntry {
    fstream *pstreamloc;
    off_t pos;
    dev_t device;
    ino_t inode;
  };
  void CheckIn(fstream *,int);
  fstream *CheckOut(int);
  bool InMap(int);
  void GetEntry(StreamEntry&, int);

};

#endif
