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


// Class: CWriteCommand                     //ANSI C++
//
// Implements the swrite command.
//  swrite writes a spectrum to a file.
//     CSpectrumFormatters and the
//     CSpectrumFormatterFactory allow the
//     format of spectra written to be extended
//     easily by the user.
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved WriteCommand.h
//

#ifndef WRITECOMMAND_H  //Required for current class
#define WRITECOMMAND_H

                               //Required for base classes
#include "TCLPackagedObjectProcessor.h"



class CSpectrumFormatter;
                               
class CWriteCommand  : public CTCLPackagedObjectProcessor
{                       
			
protected:

public:

   // Constructors and other cannonical operations:

  CWriteCommand (CTCLInterpreter* pInterp) :
    CTCLPackagedObjectProcessor(*pInterp, "swrite", true)
  { 
  } 
  ~ CWriteCommand ( )  // Destructor 
    { }  

   //Copy constructor (Illegal)
private:
  CWriteCommand (const CWriteCommand& aCWriteCommand );
  CWriteCommand& operator= (const CWriteCommand& aCWriteCommand);
  int operator== (const CWriteCommand& aCWriteCommand) const;


public:
  virtual
  int operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv)  ;
protected:
  int    CountValidSpectra(char** pSpectrum, int nSpectra);
  static CSpectrumFormatter*  GetFormatter(const char* pFormatter);
  static void                 Usage(CTCLInterpreter& rInterp);

  friend class CReadCommand;
};

#endif
