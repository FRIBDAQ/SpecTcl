// Class: CNSCLAsciiSpectrumFormatter                     //ANSI C++
//
// does I/O to spectrum in a simple ASCII format.

//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved NSCLAsciiSpectrumFormatter.h
//

#ifndef __NSCLASCIISPECTRUMFORMATTER_H  //Required for current class
#define __NSCLASCIISPECTRUMFORMATTER_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

                               //Required for base classes
#ifndef __SPECTRUMFORMATTER_H
#include "SpectrumFormatter.h"
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR_
#include <vector>
#define __STL_VECTOR
#endif


                               
class CNSCLAsciiSpectrumFormatter  : public CSpectrumFormatter        
{                       
			
protected:

public:

   // Constructors and other cannonical operations:

  CNSCLAsciiSpectrumFormatter ()  :
    CSpectrumFormatter("NSCL Ascii Spectrum file format", ".spa")
  { 
  } 
  ~ CNSCLAsciiSpectrumFormatter ( )  // Destructor 
    { }  

  //Copy constructor 

  CNSCLAsciiSpectrumFormatter 
    (const CNSCLAsciiSpectrumFormatter& aCNSCLAsciiSpectrumFormatter )   : 
    CSpectrumFormatter (aCNSCLAsciiSpectrumFormatter) 
  { 
  }                                     

   // Operator= Assignment Operator 

  const CNSCLAsciiSpectrumFormatter& operator= 
    (const CNSCLAsciiSpectrumFormatter& aCNSCLAsciiSpectrumFormatter); 
 
   //Operator== Equality Operator 
  
  int operator== 
    (const CNSCLAsciiSpectrumFormatter& aCNSCLAsciiSpectrumFormatter) const;
  int operator!=
    (const CNSCLAsciiSpectrumFormatter& aCNSCLAsciiSpectrumFormatter) const
  {
    return !(*this == aCNSCLAsciiSpectrumFormatter);
  }
	
       
public:

 virtual   CSpectrum* Read (istream& rStream, 
			    ParameterDictionary& rDict)    ;
 virtual   void Write (ostream& rStream, CSpectrum& rSpectrum,
		       ParameterDictionary& rDict)    ;

protected:
  static void     WriteValues(ostream& rStream, CSpectrum& rSpectrum);
  static void     WriteLine(ostream& rStream,
		     UInt_t  nCoordinates,
		     UInt_t* pCoordinates,
		     ULong_t nValue);
  static Bool_t   NextIndex(UInt_t  nCoordinates,
			    UInt_t* pLowLimits,
			    UInt_t* pHiLimits,
			    UInt_t* pIndexes);

  static void     ReadHeader(istream&        rStream,
			     string&         rName,
			     string&         rDate,
			     Float_t&         rRevisionLevel,
			     SpectrumType_t& rSpecType,
			     DataType_t&     rDataType,
			     vector<UInt_t>& rDimensions,
			     vector<string>& rParameters);
  static string   ReadLine(istream&    rStream);
  static Bool_t   CompatibleFormat(Float_t nFormat);
  static void     ReadBody(istream&   rStream,       
			   vector<UInt_t>& rvDimensions, 
			   CSpectrum* pSpectrum);
  static void     ThrowStreamError(istream& rStream,
				   const char*    pDoing,
				   Bool_t IgnoreEof=kfFALSE);
  static void     DecodeParenList(istream& rStream, 
				  vector<string>& rList);
  static void     CheckIndices(vector<UInt_t>& Dimensions, 
			       vector<UInt_t>& Indices);
  static void     CheckDataSize(ULong_t data, DataType_t type);
  static void     ReadDelimited(istream& rStream, string& rString,
				char start, char end);
  static string   ParenListElement(istream& rStream);
};

#endif
