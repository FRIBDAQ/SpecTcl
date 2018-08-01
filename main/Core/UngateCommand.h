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

#ifndef UNGATECOMMAND_H  //Required for current class
#define UNGATECOMMAND_H

                               //Required for base classes
#include "TCLPackagedCommand.h"

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
  int operator==(const CUngateCommand& rhs) const;
  int operator!=(const CUngateCommand& rhs) const;
public:

	
  // Class operations.
public:

 virtual   int operator() (CTCLInterpreter& rInterpreter, CTCLResult& rResult,
			   int nArgs, char* pArgs[])    ;
 
protected:
  static std::string Usage();

};

#endif
