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

//  CPipeFile.h:
//
//    This file defines the CPipeFile class.
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

#ifndef __PIPEFILE_H  //Required for current class
#define __PIPEFILE_H
                               //Required for base classes
#ifndef __FILE_H
#include "File.h"
#endif

#ifndef __HISTOTYPES_H                               
#include <histotypes.h>   //Required for include files,
#endif

#ifndef __CRTL_UNISTD_H
#include <unistd.h>
#define __CRTL_UNISTD_H
#endif
                                                               
class CPipeFile  : public CFile        
{
  pid_t m_nPid;  // PId of child process, or 0.
  
public:
			//Default constructor

  CPipeFile () :  
    m_nPid(0)
  {}
  virtual  ~ CPipeFile ( );
  
	
			//Copy constructor

  CPipeFile (const CPipeFile& aCPipeFile );


			//Operator= Assignment Operator

  CPipeFile& operator= (const CPipeFile& aCPipeFile);

			//Operator== Equality Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  int operator== (const CPipeFile& aCPipeFile);
  //
  // Selectors:
  //
public:

                       //Get accessor function for attribute
  pid_t getPid() const
  {
    return m_nPid;
  }

  // Protected mutators:
  //
protected:
                       //Set accessor function for attribute
  void setPid (pid_t am_nPid)
  { 
    m_nPid = am_nPid;
  }

public:
  // Class operations:
  //                   
  virtual   void   Open (const STD(string)& rsConnection, UInt_t nAccess,
			 bool fullBlocks = true)  ;

  virtual   void   Close ()  ;
  virtual   Bool_t IsReadable (UInt_t nMs) const  ;

  // Protected utilities.
  //
protected:
  void          DoAssign(const CPipeFile& rRhs);
  static char** MakeArgv(const STD(string)& rConnection);
  //
  // Private utilities:
  //
private:
  static UInt_t CountItems(const char* pString, const char* pDelims);

};

#endif
