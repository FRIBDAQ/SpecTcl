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

#ifndef __NSCLBINARYSPECTRUMFORMATTER_H  //Required for current class
#define __NSCLBINARYSPECTRUMFORMATTER_H

                               //Required for base classes
#ifndef __SPECTRUMFORMATTER_H
#include "SpectrumFormatter.h"
#endif
                               
#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif
#ifndef __NSCLBINOBJ
#include <nsclbin.h>
#include <nsclbinerror.h>
#define __NSCLBINOBJ
#endif
#include <vector>
class CNSCLBinarySpectrumFormatter  : public CSpectrumFormatter        
{                       
			
protected:

public:

   // Constructors and other cannonical operations:

  CNSCLBinarySpectrumFormatter ()  :
    CSpectrumFormatter(string("NSCL Binary file format (formerly SMAUG)"),
		       string(".spc"))
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

 virtual   CSpectrum* Read (istream& rStream,
			    ParameterDictionary& rDict);
 virtual   void Write (ostream& rStream, CSpectrum& rSpectrum,
		       ParameterDictionary& rDict);
 private:
 void readhead(istream&, string&, string&, string&, vector<UInt_t>&, SpectrumType_t&, DataType_t&, nsclbin&);
 void readdata(nsclbin&, vector<long>&);
 void getresolutions(const vector<UInt_t>&,vector<UInt_t>&);
 void getindices(UInt_t, const vector<UInt_t>&, UInt_t*);
 void insertdata(CSpectrum*, vector<long>&, const vector<UInt_t>&, const vector<UInt_t>&);
 bool parameterexist(string&);
 void setdimensions(nsclbin&, const CSpectrum&);
 void setdata(nsclbin&, const CSpectrum&);
 Bool_t NextIndex(UInt_t, UInt_t*, UInt_t*,UInt_t*);
 void WriteVal(nsclbin&, UInt_t, UInt_t*, ULong_t);
 void AssurePossibleSpectrum(SpectrumType_t&,DataType_t&);
 void testdata(nsclbin&);
 void testdata(CSpectrum*);
 Bool_t goodchannel(UInt_t*,  const vector<UInt_t>&, const vector<UInt_t>&);
 void padstream(ostream&);
 UInt_t f2n(UInt_t);
 //data members:


};
#endif
