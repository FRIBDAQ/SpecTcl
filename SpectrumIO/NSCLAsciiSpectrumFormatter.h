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

/*
  Change log:
  $Log$
  Revision 5.2  2005/06/03 15:19:29  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:26  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:13  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.2  2003/04/19 00:11:50  ron-fox
  Move to format version 2.0:  Supply mapping information about the range covered by each axis of a spectrum.  Note that reads are backwards compatible with version 1.0 and will default the range to 0 - nchans-1.

*/
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

#ifndef __OSTREAM_DAQH
#include <Ostream.h>
#endif
#ifndef __ISTREAM_DAQH
#include <Istream.h>
#endif


/*!
 Class: CNSCLAsciiSpectrumFormatter
 does I/O to spectrum in a simple ASCII format.
   The format is as follows:
    line 1:  "name"  (dimensions) \n
    line 2:  date/time \n
    line 3:  2.0\n    [note 2.0 is file format version].
    line 4:  spectrum type (SpecTcl coding)\n
    line 5:  List of parameters in spectrum.\n
    line 6:  List of axis ranges (new in format 2.0).
    line 7:  "-------------------------------------------\n"
    data lines: (coords)   data
    end  line   (-1 for all coords) -1 for data.

     \note that missing coordinates have data values of zero.
     (zero supression).
     \note this format allows for several spectra to fit into a file as
           well as for mixed spectrum other data files.
     \note The axis range format is :  (low high) once for each dimension.
     \note  This class is able to read in format level 1.0 as well (same
            format as above, but line 6 is missing.  In this case, the
            spectrum axis is assumed to run from 0 - size-1

*/                                
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

 virtual   CSpectrum* Read (STD(istream)& rStream, 
			    ParameterDictionary& rDict)    ;
 virtual   void Write (STD(ostream)& rStream, CSpectrum& rSpectrum,
		       ParameterDictionary& rDict)    ;

protected:
  static void     WriteValues(STD(ostream)& rStream, CSpectrum& rSpectrum);
  static void     WriteLine(STD(ostream)& rStream,
		     UInt_t  nCoordinates,
		     UInt_t* pCoordinates,
		     ULong_t nValue);
  static Bool_t   NextIndex(UInt_t  nCoordinates,
			    UInt_t* pLowLimits,
			    UInt_t* pHiLimits,
			    UInt_t* pIndexes);

  static void     ReadHeader(STD(istream)&        rStream,
			     STD(string)&         rName,
			     STD(string)&         rDate,
			     Float_t&         rRevisionLevel,
			     SpectrumType_t& rSpecType,
			     DataType_t&     rDataType,
			     STD(vector)<UInt_t>& rDimensions,
			     STD(vector)<STD(string)>& rParameters,
			     STD(vector)<Float_t>& rLows,
			     STD(vector)<Float_t>& rHighs);
  static STD(string)   ReadLine(STD(istream)&    rStream);
  static Bool_t   CompatibleFormat(Float_t nFormat);
  static void     ReadBody(STD(istream)&   rStream,       
			   STD(vector)<UInt_t>& rvDimensions, 
			   CSpectrum* pSpectrum);
  static void     ThrowStreamError(STD(istream)& rStream,
				   const char*    pDoing,
				   Bool_t IgnoreEof=kfFALSE);
  static void     DecodeParenList(STD(istream)& rStream, 
				  STD(vector)<STD(string)>& rList);
  static void     DecodeListOfParenLists(STD(istream)& rStream,
					 STD(vector)<STD(string)>& rList);
  static void     CheckIndices(STD(vector)<UInt_t>& Dimensions, 
			       STD(vector)<UInt_t>& Indices);
  static void     CheckDataSize(ULong_t data, DataType_t type);
  static void     ReadDelimited(STD(istream)& rStream, STD(string)& STDString,
				char start, char end);
  static STD(string)   ParenListElement(STD(istream)& rStream);
};

#endif
