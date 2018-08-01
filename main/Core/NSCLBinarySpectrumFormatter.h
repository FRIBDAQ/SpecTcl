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
// Class: CNSCLBinarySpectrumFormatter                     //ANSI C++
//
// Does spectrum I/O in NSCL binary format
// (previously known as SMAUG format) to
// a spectrum file.  Note that NSCL binary
// format is predicated on a little endian
// representation of data.  This implementation
// does not assume the system on which we are
// executing is little-endian, however.
// 
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved NSCLBinarySpectrumFormatter.h
//

#ifndef NSCLBINARYSPECTRUMFORMATTER_H  //Required for current class
#define NSCLBINARYSPECTRUMFORMATTER_H

                               //Required for base classes
#include "SpectrumFormatter.h"
#include <string>
#include <nsclbin.h>
#include <nsclbinerror.h>
#include <vector>
#include <istream>
#include <ostream>


class CNSCLBinarySpectrumFormatter  : public CSpectrumFormatter        
{                       
			
protected:

public:

   // Constructors and other cannonical operations:

  CNSCLBinarySpectrumFormatter ()  :
    CSpectrumFormatter(std::string("NSCL Binary file format (formerly SMAUG)"),
		       std::string(".spc"))
  { 
  } 
  ~CNSCLBinarySpectrumFormatter ( )  // Destructor 
  { }  


  CNSCLBinarySpectrumFormatter 
      (const CNSCLBinarySpectrumFormatter& aCNSCLBinarySpectrumFormatter ) : 
	CSpectrumFormatter (aCNSCLBinarySpectrumFormatter) 
  { 
  }                                     

   // Operator= Assignment Operator 

  CNSCLBinarySpectrumFormatter& operator=
     (const CNSCLBinarySpectrumFormatter& aCNSCLBinarySpectrumFormatter);
 
   //Operator== Equality Operator 

  int operator== 
     (const CNSCLBinarySpectrumFormatter& aCNSCLBinarySpectrumFormatter) const;

  int operator!= 
     (const CNSCLBinarySpectrumFormatter& aCNSCLBinarySpectrumFormatter) const
  {
    return !(*this == aCNSCLBinarySpectrumFormatter);
  }

	
// Selectors:

public:

 virtual   std::pair<std::string, CSpectrum*> Read (std::istream& rStream,
			    ParameterDictionary& rDict);
 virtual   void Write (std::ostream& rStream, CSpectrum& rSpectrum,
		       ParameterDictionary& rDict);
 private:
 void readhead(std::istream&, std::string&, std::string&, std::string&, std::vector<UInt_t>&, SpectrumType_t&, DataType_t&, nsclbin&);
 void readdata(nsclbin&, std::vector<long>&);
 void getresolutions(const std::vector<UInt_t>&,std::vector<UInt_t>&);
 void getindices(UInt_t, const std::vector<UInt_t>&, UInt_t*);
 void insertdata(CSpectrum*, std::vector<long>&, const std::vector<UInt_t>&, const std::vector<UInt_t>&);
 bool parameterexist(std::string&);
 void setdimensions(nsclbin&, const CSpectrum&);
 void setdata(nsclbin&, const CSpectrum&);
 Bool_t NextIndex(UInt_t, UInt_t*, UInt_t*,UInt_t*);
 void WriteVal(nsclbin&, UInt_t, UInt_t*, ULong_t);
 void AssurePossibleSpectrum(SpectrumType_t&,DataType_t&);
 void testdata(nsclbin&);
 void testdata(CSpectrum*);
 Bool_t goodchannel(UInt_t*,  const std::vector<UInt_t>&, const std::vector<UInt_t>&);
 void padstream(std::ostream&);
 UInt_t f2n(UInt_t);
 //data members:


};
#endif
