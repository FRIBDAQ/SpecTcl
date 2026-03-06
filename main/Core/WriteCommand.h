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
#include <string>


class CSpectrumFormatter;
                               
class CWriteCommand  : public CTCLPackagedObjectProcessor
{                       
			
protected:

public:

   // Constructors and other cannonical operations:

  CWriteCommand (CTCLInterpreter* pInterp, const char* cmd="swrite") :
    CTCLPackagedObjectProcessor(*pInterp, cmd, true)
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
  int    CountValidSpectra(const char** pSpectrum, int nSpectra);
  static CSpectrumFormatter*  GetFormatter(const char* pFormatter);
  static void                 Usage(CTCLInterpreter& rInterp);

  friend class CReadCommand;
};

/**
 * @struct WriteCommandInfo
 *    There are some spectrum formatters that may need to know
 * more about the write operation being performed.
 * Specifically, some formats may not be ablt to use an std::ostream&
 * might need to use the file path directly.
 * This singleton struct allows that information to be gotten
 */
struct WriteCommandInfo {
  static WriteCommandInfo* m_pInstance;
  std::string              m_filename;         //!< :fd: - if e.g. pipe.
  bool                     m_firstSpectrum;

  static WriteCommandInfo* getInstance();
};
#endif
