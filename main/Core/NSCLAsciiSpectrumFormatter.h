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
  Revision 5.4  2007/05/11 20:51:57  ron-fox
  Make NSCLAsciiSpectrumFormatter correctly deal with rev 2, 3 and
  'malforme3d 2' produced by version 3.2-pre2 for summary spectra.

  Revision 5.3  2007/02/23 20:38:18  ron-fox
  BZ291 enhancement... add gamma deluxe spectrum type (independent x/y
  parameter lists).

  Revision 5.2  2005/06/03 15:19:29  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:26  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:13  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.2  2003/04/19 00:11:50  ron-fox
  Move to format version 2.0:  Supply mapping information about the range covered by each axis of a spectrum.  Note that reads are backwards compatible with version 1.0 and will default the range to 0 - nchans-1.

*/
#ifndef NSCLASCIISPECTRUMFORMATTER_H  //Required for current class
#define NSCLASCIISPECTRUMFORMATTER_H

#include <histotypes.h>
#include "SpectrumFormatter.h"
#include <string>
#include <vector>
#include <ostream>
#include <istream>


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

 virtual   std::pair<std::string, CSpectrum*> Read (std::istream& rStream, 
			    ParameterDictionary& rDict)    ;
 virtual   void Write (std::ostream& rStream, CSpectrum& rSpectrum,
		       ParameterDictionary& rDict)    ;

protected:
  static void     WriteValues(std::ostream& rStream, CSpectrum& rSpectrum);
  static void     WriteLine(std::ostream& rStream,
		     UInt_t  nCoordinates,
		     UInt_t* pCoordinates,
		     ULong_t nValue);
  static Bool_t   NextIndex(UInt_t  nCoordinates,
			    UInt_t* pLowLimits,
			    UInt_t* pHiLimits,
			    UInt_t* pIndexes);

  static void     ReadHeader(std::istream&        rStream,
			     std::string&         rName,
			     std::string&         rDate,
			     Float_t&         rRevisionLevel,
			     SpectrumType_t& rSpecType,
			     DataType_t&     rDataType,
			     std::vector<UInt_t>& rDimensions,
			     std::vector<std::string>& rParameters,
			     std::vector<std::string>& ryParameters,
			     std::vector<Float_t>& rLows,
			     std::vector<Float_t>& rHighs);
  static std::string   ReadLine(std::istream&    rStream);
  static Bool_t   CompatibleFormat(Float_t nFormat);
  static void     ReadBody(std::istream&   rStream,       
			   std::vector<UInt_t>& rvDimensions, 
			   CSpectrum* pSpectrum);
  static void     ThrowStreamError(std::istream& rStream,
				   const char*    pDoing,
				   Bool_t IgnoreEof=kfFALSE);
  static void     DecodeParenList(std::istream& rStream, 
				  std::vector<std::string>& rList);
  static void     DecodeListOfParenLists(std::istream& rStream,
					 std::vector<std::string>& rList);
  static void     CheckIndices(std::vector<UInt_t>& Dimensions, 
			       std::vector<UInt_t>& Indices);
  static void     CheckDataSize(ULong_t data, DataType_t type);
  static void     ReadDelimited(std::istream& rStream, std::string& STDString,
				char start, char end);
  static std::string   ParenListElement(std::istream& rStream);
  static bool     LimitCountOk(SpectrumType_t type, size_t limits, size_t dimensions);
};

#endif
