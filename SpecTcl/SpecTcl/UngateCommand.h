// Class: CUngateCommand                     //ANSI C++
//
// Removes the gating condition from a spectrum.
// The gate on a spectrum is replaced with a gate which
// is always true.
//   The form of the command is:
//        ungate spec1 [spec2 ...]
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved UnGateCommand.h
//

#ifndef __UNGATECOMMAND_H  //Required for current class
#define __UNGATECOMMAND_H

                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif

class CTCLInterpreter;
class CTCLResult;
class CTCLCommandPackage;
                               
class CUngateCommand  : public CTCLPackagedCommand        
{                       
public:			
  // Constructor:

  CUngateCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPack) : 
    CTCLPackagedCommand("ungate", pInterp, rPack)
  {     
  } 
   ~ CUngateCommand ( ) { }  //Destructor
  
   //Copy constructor - Illegal
private:
  CUngateCommand (const CUngateCommand& aCUngateCommand );
public:

   // Operator= Assignment Operator Illegal
private:
  CUngateCommand& operator= (const CUngateCommand& aCUngateCommand);
public:
 
   //Operator== Equality Operator - nonsense, but allowed:

  int operator== (const CUngateCommand& aCUngateCommand) const {
    return CTCLPackagedCommand::operator==(aCUngateCommand);
  }
	
  // Class operations.
public:

 virtual   int operator() (CTCLInterpreter& rInterpreter, CTCLResult& rResult,
			   int nArgs, char* pArgs[])    ;
 
protected:
  static string Usage();

};

#endif
