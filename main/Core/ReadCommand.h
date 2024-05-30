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

		     END OF TERMS AND CONDITIONS '
*/
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

#ifndef READCOMMAND_H  //Required for current class
#define READCOMMAND_H

#include "TCLPackagedObjectProcessor.h"
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>

// Forward class references.

class CTCLInterpeter;
class CTCLCommandPackage;                               

//

class CReadCommand  : public CTCLPackagedObjectProcessor
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

  CReadCommand (CTCLInterpreter* pInterp)  :
    CTCLPackagedObjectProcessor(*pInterp, "sread", true)
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

  virtual   TCLPLUS::Int_t operator() (CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv)    ;
 
  // Utilities.

protected:
  static void     Usage(CTCLInterpreter& rResult);
  static Switch_t NextSwitch(const char* pText);


};

#endif
