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

#ifndef __WRITECOMMAND_H  //Required for current class
#define __WRITECOMMAND_H

                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif


#ifndef __STL_STRING
#include <string>
#endif


class CSpectrumFormatter;
                               
class CWriteCommand  : public CTCLPackagedCommand        
{                       
			
protected:

public:

   // Constructors and other cannonical operations:

  CWriteCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPack)  :
    CTCLPackagedCommand("swrite", pInterp, rPack)
  { 
  } 
  ~ CWriteCommand ( )  // Destructor 
    { }  

   //Copy constructor (Illegal)
private:
  CWriteCommand (const CWriteCommand& aCWriteCommand );
public:

   // Operator= Assignment Operator  (illegal)
private:
  CWriteCommand& operator= (const CWriteCommand& aCWriteCommand);
public:
 
   //Operator== Equality Operator 

  int operator== (const CWriteCommand& aCWriteCommand) const;
	
  // operations

public:
  virtual
  int CWriteCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
				int nArgs, char* pArgs[])  ;
protected:
  static Bool_t               IsFileId(const char* pConnectionString);
  static Int_t                GetFileId(const char* pConnectionString);
  static CSpectrumFormatter*  GetFormatter(const char* pFormatter);
  static void                 Usage(CTCLResult& rResult);

  friend class CReadCommand;
};

#endif
