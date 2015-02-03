/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/



static const char* Copyright = "(C) Copyright Michigan State University 2009, All rights reserved";
// Class: CNSCLAsciiSpectrumFormatter
// does I/O to spectrum in a simple ASCII format.
//   The format is as follows:
//    line 1:  "name"  (dimensions) \n
//    line 2:  date/time \n
//    line 3:  1.0\n    [note 1.0 is file format version].
//    line 4:  spectrum type (SpecTcl coding)\n
//    line 5:  List of parameters in spectrum.\n
//    line 6:  "-------------------------------------------\n"
//    data lines: (coords)   data
//    end  line   (-1 for all coords) -1 for data.
//
//     Note that missing coordinates have data values of zero.
//     (zero supression).
//     Note: this format allows for several spectra to fit into a file as
//           well as for mixed spectrum other data files.
//
//     
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////



#include <config.h>

#include "NSCLAsciiSpectrumFormatter.h"    				
#include <Spectrum.h>
#include <Spectrum1DL.h>
#include <Spectrum1DW.h>
#include <SpectrumPackage.h>
#include <SpectrumFormatError.h>
#include <StreamIOError.h>
#include <SpectrumFactory.h>

#include <math.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <Sstream.h>
#include <ctype.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

static const char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved NSCLAsciiSpectrumFormatter.cpp \n";

static float FormatLevel = 3.0;
static const char* HeaderEnding= "--------------------------------------------\n";

// The stuff below is a table which gives largest values for 
// various data types.
//

static ULong_t Maxima[] = {
  UCHAR_MAX,			// Largest UChar_t
  USHRT_MAX,			// Largest UShort_t
  ULONG_MAX,			// Largest ULong_t
  ULONG_MAX,			// For now largest UFloat_t
  ULONG_MAX,			// For now largest DFloat_t
  0
};

// Matching predicate for id lookups:

class FindById {
  UInt_t m_nId;
public:
  FindById(UInt_t nId) : m_nId(nId) {}
  int operator()(pair<const string, CNamedItem> i) {
    return (i.second.getNumber() == m_nId);
  }
};

// reseteof   used to reset any false eof condition set by the skip functions
//            in case the last character they process is also the last one
//            in the file.

static inline void reseteof(istream& rStream)
{
  ios::iostate state = rStream.rdstate();
  state &= ios::failbit | ios::badbit; // Remove eof and good...
  if(!state) state = ios::goodbit;     // If no errors, good. 
  rStream.setstate(state);
  
}

// skipwhite used to be part of istream, but isn't anymore.  Perhaps
// ipfx does the job, but the documentation on that is as clear as mud,
// therefore:
//

static inline void skipwhite(istream& rStream)
{
  char c;
  while(!rStream.eof()) {
    rStream.read(&c,1);
    if(!(isspace(c) || (c == '\n'))) {
      if(!rStream.eof()) {
	rStream.putback(c);
	reseteof(rStream);
      }
      return;
    }
  }
}

// Functions for class CNSCLAsciiSpectrumFormatter

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//    CNSCLAsciiSpectrumFormatter& operator= 
//       (const CNSCLAsciiSpectrumFormatter& aCNSCLAsciiSpectrumFormatter) 
//           const;
// Operation Type:
//    assignment.
//
const CNSCLAsciiSpectrumFormatter&
CNSCLAsciiSpectrumFormatter::operator=
    (const CNSCLAsciiSpectrumFormatter& aCNSCLAsciiSpectrumFormatter)
{
  if(this != &aCNSCLAsciiSpectrumFormatter) {
    CSpectrumFormatter::operator=(aCNSCLAsciiSpectrumFormatter);
  }
  return *this;
}

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   int operator== 
//      (const CNSCLAsciiSpectrumFormatter& aCNSCLAsciiSpectrumFormatter) 
//        const
// Operation Type:
//    Equality Comparison.
//
int
CNSCLAsciiSpectrumFormatter::operator==
     (const CNSCLAsciiSpectrumFormatter& aCNSCLAsciiSpectrumFormatter) const
{
  return CSpectrumFormatter::operator==(aCNSCLAsciiSpectrumFormatter);
}

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Read(istream& rStream)
//  Operation Type: 
//     I/O
CSpectrum* 
CNSCLAsciiSpectrumFormatter::Read(istream& rStream,
				  ParameterDictionary& rDict)  
{
  // Reads in a spectrum in simple ASCII
  // format.
  //    Note that as SpecTcl add spectrum types,
  // this code may need to be modified.
  //
  // Formal Parameters:
  //    istream& rStream:
  //       C++ Stream from which to read the data.
  //    ParameterDictionary& rDict:
  //       Refers to a parameter dictionary in which to lookup the
  //       parameter names.
  // Returns:
  //    CSpectrum* Pointer to a spectrum read in.
  //    Throws an exception on failures both in I/O
  //    and in decode.
  //

  // Read the file header.
  string         Name;		// Spectrum name.
  string         Date ;		// Date spectrum was written.
  Float_t        nRevision;	// Data format revision level.
  SpectrumType_t eSpecType;	// Type of the spectrum
  DataType_t     eDataType;	// Data type of spectrum.
  vector<UInt_t> vDimensions;	// # channels in each dimension.
  //  vector<UInt_t> vResolutions;	// Spectrum channel resolutions.

  vector<string> vParameters;	// Vector of parameter names.
  vector<string> vyParameters;  // Parameters on y axis for g2d delux.
  vector<Float_t> vLows;
  vector<Float_t> vHighs;

  ReadHeader(rStream, Name, Date, nRevision, eSpecType, eDataType, 
	     vDimensions, vParameters, vyParameters, vLows, vHighs);
  if(!CompatibleFormat(nRevision)) {
    throw CSpectrumFormatError(CSpectrumFormatError::IncompatibleFormat,
			       "Reading NSCLAscii spectrum header");
  }
  // Summary spectra are weird... Although 2-d, they only have a single
  // low/high specification, and that's determined by their
  // second dimension.  Here we do that special case translation:
  // Note that in version 3 of the format, only the single dimension is shown.


  vector<Float_t> createLows;
  vector<Float_t> createHighs;
  vector<UInt_t>  createDims;


  // This switch handles special cases:

  switch (eSpecType) {

  case keSummary:
    {
      
      UInt_t nChannels;
      if (nRevision == 3) {
	nChannels = vDimensions[0];
      }
      else {
	// There was a time that I had not incremented the
	// rev level to 2, but wrote only 1 dim:
	
	if (vDimensions.size() == 1) {
	  nChannels = vDimensions[0];
	} else {
	  nChannels = vDimensions[1];
	}
      }
      Float_t fLow      = vLows[1];
      Float_t fHigh     = vHighs[1];
      
      createDims.push_back(nChannels);
      createLows.push_back(fLow);
      createHighs.push_back(fHigh);
      
    }
    break;
  case keGSummary:
    createDims.push_back(vDimensions[1]);
    createLows.push_back(vLows[1]);
    createHighs.push_back(vHighs[1]);
    break;
  default: 
    {
      
      createDims = vDimensions;
      createLows = vLows;
      createHighs= vHighs;
    }
  }
  //
  // Create an appropriate spectrum:
  //
  CSpectrum* pSpectrum(0);
  CSpectrumFactory Factory;
  Factory.ExceptionMode(kfFALSE);

  // Gamma summary spectra are a special case:

  if (eSpecType == keGSummary) {
    // parameter names have to be marshalled into a vector of vectors.
    // each parameter strip ends in an empty parameter name ("").
    // This includes the last strip.

    vector<vector<string> > gSummaryParams;
    vector<string>          column;
    for (int i=0; i < vParameters.size(); i++) {
      if (vParameters[i] == "") {
	// End of strip:

	gSummaryParams.push_back(column);
	column.clear();
      }
      else {
	column.push_back(vParameters[i]);
      }
    }
    // Now we can create the spectrum:

    pSpectrum = Factory.CreateSpectrum(Name, eSpecType, eDataType,
				       gSummaryParams, 
				       createDims, &createLows, &createHighs);
				       

  }
  else if (vyParameters.size() == 0) {
    pSpectrum = Factory.CreateSpectrum(Name, eSpecType, eDataType,
				       vParameters, 
				       createDims,
				       &createLows, 
				       &createHighs);
  } 
  else {
    pSpectrum = Factory.CreateSpectrum(Name, eSpecType, eDataType,
				       vParameters, vyParameters,
				       createDims[0],  createDims[1],
				       &createLows, &createHighs);
  }
  // Fill in the spectrum:
  //
  pSpectrum->Clear();

  // Once more a wierdness of summary spectra.. at this time
  // vDimensions won't be right for them:

  if (eSpecType == keSummary) {
    vDimensions.clear();
    vDimensions.push_back(vParameters.size());  // X axis dim is parameter count.
    vDimensions.push_back(createDims[0]);       // and y axis is the 1 creation dim.
  }

  try {


    ReadBody(rStream, vDimensions, pSpectrum);
  }
  catch(...) {			// If an exception is thrown delete the our
    delete pSpectrum;		// allocated memory before retossing the
    throw;			// exception to the next levels.
  }
  return pSpectrum;		// Return the final spectrum to the caller.
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Write(ostream& rStream, CSpectrum& rSpectrum)
//  Operation Type: 
//     I/O
void 
CNSCLAsciiSpectrumFormatter::Write(ostream& rStream, CSpectrum& rSpectrum,
				   ParameterDictionary& rDict)  
{
  // Writes a spectrum to file in a simple
  // ASCII format.  Note that this member
  // may need modification if the set of
  // spectra supported by SpecTcl are changed.
  // 
  //   Formal Parameters:
  //       ostream& rStream:
  //         C++ stream to which to write the spectrum.
  //       CSpectrum& rSpectrum:
  //         Refers to the spectrum to write.
  //       ParameterDictionary& rDict:
  //         Refers to the parameter dictionary which has parameter number
  //         name correspondences.
  // 
  //
  CSpectrum::SpectrumDefinition spectrumDef = rSpectrum.GetDefinition();
  string Quote("\"");
  string Spname = Quote + spectrumDef.sName + Quote;
  
  rStream << Spname.c_str() << " ";  // Write out quoted spectrum name
  
  // Write out the dimensions parethesized, spaces between dimensions

  char Delimeter = '(';
  for(UInt_t i = 0; i < spectrumDef.nChannels.size(); i++) {
    rStream << Delimeter << spectrumDef.nChannels[i];
    Delimeter = ' ';
  }
  rStream << ")\n";
 
  // Get the date and time and make a line of it with the format...

  time_t SinceEpoch;
  SinceEpoch    = time(&SinceEpoch);
  tm*    pDate  = localtime(&SinceEpoch);
  char*  pTimes = asctime(pDate);
  rStream << pTimes << " " << FormatLevel << endl;

  // List the spectrum type:
  //
  SpectrumType_t sType = spectrumDef.eType;
  DataType_t     dType = spectrumDef.eDataType;

  rStream << sType  << ' ' << dType << 
    endl;

  // List the parameters of the spectrum.
  // 

  // Gamma 2d deluxe parameters are written as two lists (x then y) parameters
  // All others are written as a single list:

  vector<UInt_t> Parameters = spectrumDef.vParameters;
  if (sType == keG2DD) {
    Delimeter = '(';
    for (UInt_t i = 0; i < Parameters.size(); i++) {
      FindById p(Parameters[i]);
      ParameterDictionaryIterator pD = rDict.FindMatch(p);
      string name;
      if (pD != rDict.end()) {
	name =  pD->second.getName();
      }
      else {
	name = "*UNKNOWN*";
      }
      rStream << Delimeter << Quote << name << Quote;
      Delimeter = ' ';		// Internal delimeter.
    }
    rStream << ") ";		// Close off the first one:
    Delimeter = '(';
    for (UInt_t i = 0; i < spectrumDef.vyParameters.size(); i++) {
      UInt_t   pnum = spectrumDef.vyParameters[i];
      FindById p(pnum);
      ParameterDictionaryIterator pD = rDict.FindMatch(p);
      string name;
      if (pD != rDict.end()) {
	name =  pD->second.getName();
      }
      else {
	name = "*UNKNOWN*";
      }
      rStream << Delimeter << Quote << name << Quote;
      Delimeter   = ' ';
    }
    rStream << ")\n";
  }
  else {
    Delimeter = '(';
    for(UInt_t i = 0; i < Parameters.size(); i++) {
      UInt_t pnum = Parameters[i];
      FindById p(pnum);
      ParameterDictionaryIterator pD = rDict.FindMatch(p);
      if(pD != rDict.end()) {
	rStream << Delimeter << Quote << (*pD).second.getName() << Quote;
      }
      else {
	rStream << Delimeter << (pnum == UINT_MAX ? Quote+Quote : "*UNKNOWN*");
      }
      Delimeter = ' ';
    }
    rStream << ")\n";

  }
  // 
  // List of axis ranges.  This is gotten from 
  // calling AxisToMappedParameter for each axis for channel 0 and size-1.
  //
				// First dimension is easy:
  
  Float_t xlow, xhigh;
  UInt_t  nChannels = rSpectrum.Dimension(0); // Everyone has this..
  xlow = spectrumDef.fLows[0];
  xhigh= spectrumDef.fHighs[0];

  rStream << '(' << xlow << " " << xhigh << ") ";
  if(rSpectrum.Dimensionality() == 2) {
    UInt_t nymap = 1;		// By default second axis is second map..
    if(sType == keG2D) {
      nymap = Parameters.size(); // All x maps are first in gamma 2ds.
    } 
    if(sType == keG2DD) {
      nymap = spectrumDef.vyParameters[0];
    }
    xlow = spectrumDef.fLows[nymap];
    xhigh= spectrumDef.fHighs[nymap];
    rStream << '(' << xlow << " " << xhigh << ")";
  }
  rStream << "\n";

  //
  //  This concludes the header:
  // 
  rStream << HeaderEnding;
  WriteValues(rStream, rSpectrum);
  
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//   WriteValues(ostream& rStream, CSpectrum& rSpectrum)
// Operation Type:
//   Protected, static utility.
// 
void
CNSCLAsciiSpectrumFormatter::WriteValues
        (ostream& rStream, CSpectrum& rSpectrum)
{
  // Writes the spectrum channel values (all the stuff following the 
  // header).  This requires that we get the low and hi limits for all
  // of the dimensions.  At present, the low limit is always 0, the
  // hi limit is given by the Dimension() member of CSpectrum.
  // We iterate over all values and put out non zero channels.
  //
  //  Formal Parameters:
  //     ostream& rStream: 
  //        Write spectrum here.
  //     CSpectrum& rSpectrum:
  //        The Spectrum to write.
  //

  UInt_t nCoordinates = rSpectrum.Dimensionality();
  UInt_t* pLows       = new UInt_t[nCoordinates]; // Low limits.
  UInt_t* pHis        = new UInt_t[nCoordinates]; // hi limits.
  UInt_t* pIdx        = new UInt_t[nCoordinates]; // current indices.

  memset(pIdx,  0, nCoordinates*sizeof(UInt_t));
  memset(pLows, 0, nCoordinates*sizeof(UInt_t));

  for(UInt_t i = 0; i < nCoordinates; i++) {
    pHis[i] = rSpectrum.Dimension(i);
  }

  do {
    ULong_t Value = rSpectrum[pIdx];
    if(Value != 0) 
      WriteLine(rStream, nCoordinates, pIdx, Value);
  } while(NextIndex(nCoordinates, pLows, pHis, pIdx));

  char Delimeter = '(';
  for(UInt_t i = 0; i < nCoordinates; i++) {
    rStream << Delimeter << -1;
    Delimeter = ' ';
  }
  rStream << ") -1\n";
  delete []pLows;
  delete []pHis;
  delete []pIdx;
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void WriteLine(ostream& rStream, UInt_t nCoords, UInt_t* pCoords, 
//                  ULong_t  nValue)
// Operation type:
//   Protected utility.
//
void
CNSCLAsciiSpectrumFormatter::WriteLine(ostream& rStream, 
				       UInt_t  nCoords, UInt_t* pCoords,
				       ULong_t nValue)
{
  // Writes a single line to the output file.
  // The line contains a parenthesized list of coordinates, following the
  // close paren, the value of the data at that coordinate set is written.
  // 
  // Formal Parameters:
  //    ostream& rStream:
  //       Refers to the output stream on which the data is being written.
  //    UInt_t nCoords, UInt_t* pCoords:
  //       Set of coordinates describing the point's location.
  //    ULong_t nValue:
  //       Value of the point.
  //
  char Delimeter='(';
  for(UInt_t i = 0; i < nCoords; i++) {
    rStream << Delimeter << *pCoords;
    pCoords++;
    Delimeter  = ' ';
  }
  rStream << ") " << nValue << endl;
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//    Bool_t NextIndex(UInt_t nCoords, UInt_t* pLows, UInt_t* pHis,
//                     UInt_t* pIdx)
// Operation Type:
//    Protected utility.
//
Bool_t
CNSCLAsciiSpectrumFormatter::NextIndex(UInt_t nCoords, 
				       UInt_t* pLows, UInt_t* pHis,
				       UInt_t* pIdx)
{
  // Determines the next sequential index in an arbitrarily dimensioned
  // array.. if there is no such dimension, this function returns
  // kfFALSE.
  //
  // Formal Parameters:
  //    UInt_t nCoords,
  //    UInt_t *pLows, UInt_t* pHis:
  //      Number of coordinates, low limits and hi limits of the arrray.
  //    UInt_t* pIdx:
  //      Current set of indices.
  // 

  UInt_t i = 0;
  while(i < nCoords) {
    pIdx[i]++;
    if(pIdx[i] >= pHis[i]) {	// Need to carry into the next coordinate.
      pIdx[i] = pLows[i];
      i++;
    }
    else {			// Don't need to carry so done.
      return kfTRUE;
    }
  }                            // Carries out of last dimension indicate end. 
  return kfFALSE;
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//    string ReadLine(istream& rStream)
// Operation type:
//    Utility
//
string
CNSCLAsciiSpectrumFormatter::ReadLine(istream& rStream)
{
  // Reads a line from file into a string which is returned.
  // the read also terminates if the stream reaches an eof condition.
  // The newline character is not placed in the output string.
  // eof is not an error.  A leading eof results in an empty string.

  string Result;
  while(!rStream.eof()) {
    char c;
    rStream.read(&c, 1);
    if(c == '\n') {
      return Result;
    }
    Result += c;
  }
  // Control lands here on an end of file:

  return Result;
}
///////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void ReadHeader(istream& rStream, string& rName, string& rDate,
//                     Float_t& rRevision, SpectrumType_t& rSpecType,
//                     DataType_t& rType, 
//                     vector<UInt_t>& rDimensions,
//                     vector<string>& rParameters)
//  Operation Type:
//     Utility
//
/*!
  \par Description:
   This function reads in a header for an ASCII formatted file, or
   throws an exception if it is unabled to do so.
   Two types of exceptions might be thrown, CSpectrumFormatError if
   the header format doesn't make sense or alternatively,
   CStreamIOError if there is an error reading the stream or unexpected
   eof condition.
  
   \par Formal Parameters:
   \param <TT>rStream (istream& [modified]):</TT>
          Stream from which the data is taken.
   \param <TT>rName: (string& [out]):</TT>
          Name of the spectrum from the header.
   \param <TT>rDate (string& [out]):</TT>
          Date at which the spectrum was generated.  The format
          of this date string is somewhat operating system dependent.
          however it is suitable for human consumption.
    \param <TT>rRevisionLevel (Float_t [out]) </TT>
          Revision level of the file format.  
    \param <TT>rSpecType (SpectrumType_t& [out])</TT>
          Type of the spectrum (e.g. 1,2,s or b).
    \param <TT>rDataType (DataType_t&  [out])</TT>
          Type of data being written (e.g. word).
    \param <TT>rDimensions (vector<UInt_t>& [out])</TT>
          The set of spectrum dimensions read from the header
          for each axis of the histogram.
    \param <TT>rParameters (vector<string>& [out])</TT>
          The set of parameter names read from the file header.
    \param <tt>ryParameters (vector<string>& [out] </tt>
          Set of y axis parameter names for gamma 2d delux spectra.
    \param <TT>rLows (vector<Float_t>& [out])</TT>
          vector of spectrum low limits.  This will be filled with
	  0.0 for level 1 formatted files, otherwise read from the 
	  Line 6 of the header.
    \param <TT>rHighs (vector<Float_t>& [out]</TT>
          Vector of spectrum high limits.  This will be filled in 
          with dimension-1 for level 1 format otherwise read 
	  from line 6 of the header.
    \para Throws:
      \throw CStreamIOError
      \throw  CSpectrumFormaterror
*/
void
CNSCLAsciiSpectrumFormatter::ReadHeader(istream&  rStream,
					string&         rName,
					string&         rDate,
					Float_t&         rRevisionLevel,
					SpectrumType_t& rSpecType,
					DataType_t&     rDataType,
					vector<UInt_t>& rDimensions,
					vector<string>& rParameters,
					vector<string>& ryParameters,
					vector<Float_t>& rLows,
					vector<Float_t>& rHighs)
{

  // The header consists of 6 lines and a separator line:

  string idanddims;
  string date;
  string revlevel;
  string type;
  string parameters;
  string separator;

  // Read these lines in.  An end of file is premature and cause for
  // a CStreamIOError exception.
  //
  idanddims = ReadLine(rStream);
  ThrowStreamError(rStream, "Reading the name and dimensions header line");

  date = ReadLine(rStream);
  ThrowStreamError(rStream, "Reading the data and time header line");

  revlevel = ReadLine(rStream);
  ThrowStreamError(rStream, "Reading format revision level header line");

  type = ReadLine(rStream);
  ThrowStreamError(rStream, "Reading spectrum and data type header line");

  parameters = ReadLine(rStream);
  ThrowStreamError(rStream, "Reading parameter list header line");



  // Decode the name and dimensions line.  This line contains
  // a quoted name of the spectrum followed by a parenthesized list of
  // spectrum dimensions separated by whitespace.
  // this is decoded from a string input stream.  End file conditions
  // on that stream are indicative of a format error in the file header
  // and result in a CSpectrumFormatError 
  //
  istringstream strString(idanddims.c_str());
  ReadDelimited(strString, rName, '"', '"');
  ThrowStreamError(strString,
		   " Getting name from id and dimensions header line");

  vector<string> Dims;
  DecodeParenList(strString, Dims);
  for(UInt_t i = 0; i < Dims.size(); i++) {
    UInt_t idim;
    if(sscanf(Dims[i].c_str(), "%u", &idim) != 1) {
      throw CSpectrumFormatError(CSpectrumFormatError::InvalidHeader,
				 "Decoding a spectrum dimension");
    }
    rDimensions.push_back(idim);
  }
  //
  // At this time, the date string is passed uninterpreted to the
  // caller:
  //
  rDate = date;
  //
  //  Decode the revlevel into a float:
  //
  istringstream strRev(revlevel.c_str());
  strRev >> rRevisionLevel;
  ThrowStreamError(strRev,
		   "Decoding revision level from Spectrum header", kfTRUE);

  // Next decode the spectrum and data types.
  //
  istringstream strTypes(type.c_str());
  skipwhite(strTypes);
  strTypes >> rSpecType;
  ThrowStreamError(strRev,
		   "Decoding spectrum type in header", kfTRUE);
  if(rSpecType == keUnknown) {	// Invalid spectrum type.
    throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
			       "Decoding spectrum type");
  }
  skipwhite(strTypes);
  strTypes >> rDataType;
  ThrowStreamError(strTypes,
		   "Decoding spectrum data type in header", kfTRUE);
  if(rDataType == keUnknown_dt) {
    throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
			       "Decoding spectrum data type");
  }
  // Decode the parameter names:

  istringstream ParameterStream(parameters.c_str());

  DecodeParenList(ParameterStream, rParameters); //  This is all or x only params:
  // Each parameter name is surrounded by quotes to allow for spaces.  These
  // must be removed.

  for(UInt_t  i = 0; i < rParameters.size(); i++) {
    rParameters[i] = rParameters[i].substr(1, 
					   rParameters[i].size()-2);
  } 						
  // If a gamma 2d deluxe spectrum there are two parameter lists:

  if (rSpecType == keG2DD) {
    DecodeParenList(ParameterStream, ryParameters);
    for (UInt_t i =0; i < ryParameters.size(); i++) {
      ryParameters[i] = ryParameters[i].substr(1, 
					       ryParameters[i].size()-2);
    }
  }



  // if this is format 2.0 or greater, we have a new line to decode
  // that has limit information for each axis:

  if(rRevisionLevel >= 2.0) {

    string LimitString = ReadLine(rStream);
    ThrowStreamError(rStream, "Reading limit line (rev. 2.0)");

    istringstream strLimits(LimitString.c_str());
    vector<string> Limits;
    DecodeListOfParenLists(strLimits, Limits);

    if(!LimitCountOk(rSpecType, Limits.size(), rDimensions.size())) {
      throw 
	CSpectrumFormatError(
		     CSpectrumFormatError::InvalidHeader,
		    "Axis limit count differs from  dimensionality");
    }


    for(int i = 0; i < Limits.size(); i++ ) { // process limits.
      vector<string> LimitPair;
      istringstream strPair(Limits[i].c_str());
      DecodeParenList(strPair, LimitPair);
      if(LimitPair.size() != 2) {
	throw
	  CSpectrumFormatError(
			   CSpectrumFormatError::InvalidHeader,
			  "Axis limit pair is not a pair");
      }
      Float_t fValue;
      if(sscanf(LimitPair[0].c_str(), "%f", &fValue) != 1) {
	throw
	  CSpectrumFormatError(
		       CSpectrumFormatError::InvalidHeader,
		       "Decoding an axis limit value.");
      }
      rLows.push_back(fValue);
      if(sscanf(LimitPair[1].c_str(), "%f", &fValue) != 1) {
	  CSpectrumFormatError(
		       CSpectrumFormatError::InvalidHeader,
		       "Decoding an axis limit value.");
      }
      rHighs.push_back(fValue);

    }

    
    
  } else {			// Older stuff, calculate limits:
    for(int i = 0; i < rDimensions.size(); i++) {
      rLows.push_back(0.0);
      rHighs.push_back(((Float_t)rDimensions[i]) - 1.0);
    }
  }

  separator = ReadLine(rStream);
  ThrowStreamError(rStream, "Reading separator line");

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   static void     ThrowStreamError(istream& rStream,
//				      char*    pDoing)
// Operation Type:
//    Utility.
//
void
CNSCLAsciiSpectrumFormatter::ThrowStreamError(istream& rStream,
					      const char* pDoing,
					      Bool_t IgnoreEof)
{
  if(!rStream.good()) {		// Couldn't pull out revlevel:
    CStreamIOError::IoStreamConditions Reason;
    if(rStream.eof()) Reason = CStreamIOError::EndFile;
    if(rStream.bad()) Reason = CStreamIOError::BadSet;
    if(rStream.fail()) Reason= CStreamIOError::FailSet;
    if(IgnoreEof && (Reason == CStreamIOError::EndFile)) return;
    throw CStreamIOError(Reason,
			 pDoing,
			 rStream);
  }
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    Bool_t   CompatibleFormat(Float_t nFormat)
// Operation type:
//    Utility.
//
Bool_t
CNSCLAsciiSpectrumFormatter::CompatibleFormat(Float_t nFormat)
{
  return (nFormat <= FormatLevel); // for now we're compatible with all older revs.
}
////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void     DecodeParenList(istream& rStream, 
//			       vector<string>& rList)
//  Operation Type:
//     Utility
//
void 
CNSCLAsciiSpectrumFormatter::DecodeParenList(istream& rStream,
					     vector<string>& rList)
{
  //  Decodes a parenthesized list from file. A parenthesized list is
  //  one whose items are separated by whitespace and which is enclosed
  //  by ( ).  The list must be closed prior to the end of the input stream.
  // 
  // Formal Parameters:
  //    istream& rStream:
  //       The input stream from which the list is decoded.
  //    vector<string>& rList   [out]:
  //       A vector which, on successful decode, will contain the set of
  //       items extracted from the list.
  // Throws:
  //      CSpectrumFormatError - Could not find the paren in the input stream.
  //      CStreamIOError       - End of file prior to ) or alternatively
  //                             a decode failed.
  //
  char   paren;
  string item;
  skipwhite(rStream);		// Skip leading whitespace.
  ThrowStreamError(rStream,
		   "Skipping whitespace prior to list leading (");
  rStream >> paren;		// Must be the leading paren or not a list:
  ThrowStreamError(rStream, "Extracting list leading (");
  if(paren != '(') {
    throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
			       "Could not find leading paren in list");
  }
  // Now we should just be able to extract list elements unless we have
  // a ).  The strategy is to skip white space, pull out a character and
  // see if it's a ).  If it is, then we're done and successful.
  // if it isn't the paren is put back and we pull the entire item out and
  // stuff it on the end of rList. After each attempt, we check the
  // eof/bad etc. status.
  //
  while(1) {
    skipwhite(rStream);
    ThrowStreamError(rStream, 
		     "Skipping whitespace prior to paren list element");
    rStream >> paren;
    ThrowStreamError(rStream, 
		     "Extracting candidate ) from paren list");
    if(paren == ')') break;
    rStream.putback(paren);
    item = ParenListElement(rStream);
    rList.push_back(item);
  }
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//        void     ReadBody(istream&   rStream,
//			    vector<UInt_t>& rvDimensions, 
//			    CSpectrum* pSpectrum)
// Operation Type:
//   Utility:
//
void
CNSCLAsciiSpectrumFormatter::ReadBody(istream&   rfStream, 
				      vector<UInt_t>& rvDimensions, 
				      CSpectrum* pSpectrum)
{
  // This function reads the body of a spectrum from file.  The 
  // body consists of channel description followed by the channel value
  // one channel per line.  Note that zero valued channels are suppressed.
  // The channel description is a parenthesized list of channel indices.
  // The channel value is just the number of counts in that channel.
  //
  // Formal Parameters:
  //    istream& rStream:      [modify]
  //       Stream open on the spectrum file.
  //    vector<UInt_t>& rvDimensions [read]
  //       The dimensionality of the spectrum.  It will be an error
  //       if the indices are too large or if they disagree with 
  //       the number of indices in the spectrum.
  //    CSpectrum* pSpectrum:  [modify]
  //       Pointer to the spectrum to read in.
  //
  CSpectrum& rSpectrum(*pSpectrum);    // Make the notation a bit easier.
  UInt_t*     IndexArray = new UInt_t[rvDimensions.size()];
  string Line; 

  // The main loop of this function reads the parethesized lists of
  // spectrum coordinates, converts them into integers. If any are -1
  // the loop exits, and we're done. The spectrum channel is also read
  // and then, if legal, placed in the spectrum.

  try {
    while(!rfStream.eof()) {
      Line = ReadLine(rfStream);
      istringstream rStream(Line.c_str());
      // Pull index texts from parenlist:
      
      vector<string> IndexList;
      DecodeParenList(rStream, IndexList);
      if(IndexList.size() != rvDimensions.size()) { // Mismatch in sizes means
	throw CSpectrumFormatError(             // corrupt.
				   CSpectrumFormatError::FileFormatCorrupt,
				   "Index list size != channel coord size");
      }
      // recode paren list as integer indices.
      
      vector<UInt_t> Indices;
      for(UInt_t i = 0; i < IndexList.size(); i++) {
	UInt_t n;
	if(IndexList[i] == string("-1")) {
	  delete [] IndexArray;
	  return; // End of the data.
	}
	if(sscanf(IndexList[i].c_str(), "%u", &n) != 1) {	// File format corrupt
	  throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
				     "Decoding index string");
	}
	Indices.push_back(n);
      }
      CheckIndices(rvDimensions, Indices);
      
      // Get channel value and check for size:
      
      ULong_t Channel;
      skipwhite(rStream);
      rStream >> Channel;
      ThrowStreamError(rStream,
		       "Pulling channel value out of stream", kfTRUE);
      CheckDataSize(Channel, rSpectrum.StorageType());
      
      // Build the channel list for the put and do the put.
      
      for(UInt_t i = 0; i < rvDimensions.size(); i++) {
	IndexArray[i] = Indices[i];
      }
      rSpectrum.set(IndexArray, Channel);
    }
  }
  catch (...) {
    return;
  }
    // If control passes here we hit the eof prior to the end of the data.

  delete []IndexArray;
  ThrowStreamError(rfStream,
		   "Prior to channel end sentinell");
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//    void CheckIndices(vector<UInt_t>& Dimensions, 
//			vector<UInt_t>& Indices)
// Operation Type:
//    Utility function.
//
void
CNSCLAsciiSpectrumFormatter::CheckIndices(vector<UInt_t>& Dimensions, 
					  vector<UInt_t>& Indices)
{
  // Check to ensure that the indices are in the range of the dimensions.
  // If one isn't that's an InvalidChannels SpectrumFormatError.  It's the
  // caller's responsibility to be sure that 
  //   Dimensions.size() == Indices.size()
  //
  for(UInt_t i = 0; i < Dimensions.size(); i++) {
    if(Indices[i] >= Dimensions[i]) {
      throw CSpectrumFormatError(CSpectrumFormatError::InvalidChannels,
				 "Range checking index list");
    }
  }
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   void     CheckDataSize(ULong_t data, DataType_t type)
// Operation Type:
//   Utility:
//
void
CNSCLAsciiSpectrumFormatter::CheckDataSize(ULong_t data, DataType_t type)
{
  // Checks the data size to ensure it does not overflow a particular
  // data type.  If there's an overflow, then the SpectrumFormatError::
  // OverflowChannels is signalled
  // 
  
  if(data > Maxima[type]) {
    throw CSpectrumFormatError(CSpectrumFormatError::OverflowChannels,
			       "Spectrum channel is out of data type range");
  }
}
/////////////////////////////////////////////////////////////////////
//
// Function:
//   void ReadDelimited(istream& rStream, string& rString,
//                      char start, char end)
// Operation Type:
//    Static utility.

void 
CNSCLAsciiSpectrumFormatter::ReadDelimited(istream& rStream, string& rOutput, 
			  char start, char end) 
{
  // Reads a string delimited by the specified characters.
  // Formal Parameters:
  //
  //   istream rStream:
  //     Input stream the read is done on.
  //   string& rOutput:
  //     Output string.
  //   char start,end:
  //     Starting and ending delimeter characters.

  char c;
  while(!rStream.eof()) {	// Search start delimeter.
    rStream.read(&c,1);
    if(c == start) break;
  }
  if(rStream.eof()) return;	// Nothing if at eof.
  
  // Accept the string until end delimeter:

  while(!rStream.eof()) {
    rStream.read(&c,1);
    if(c==end) {
      reseteof(rStream);
      return;
    }
    rOutput += c;
  }
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   string ParenListElement(istream& rStream)
// Operation Type:
//
string
CNSCLAsciiSpectrumFormatter::ParenListElement(istream& rStream)
{
  // Reads an element from a parenthesized list.  The end condition is
  // encountering any of:
  //  1. whitespace (put back).
  //  2. close paren (put back).
  //  3. Eof 
  //  On any exit, eof is reset.

  string item;
  while(!rStream.eof()) {
    char c;
    rStream.read(&c, 1);
    if(c == ')' || isspace(c)) { // End of element.
      rStream.putback(c);
      break;
    }
    item += c;
  }
  reseteof(rStream);
  return item;

}
/*!
    \para Functionality:
       Decodes a list of parenthesized lists.  e.g. a line that 
       looks like:
       \verbatim
          ( stuff ) ( stuff ) ...
       \endverbatim
       Each parenthesized list is stuffed into the output array,
       parentheses and all.  
       Note that we don't support nested 
       lists.

       \para Parameters:
       \param rStream
       \param rList

       \para Throws:
       \throw CStreamError
       \throw CSpectrumFormatError

*/
void
CNSCLAsciiSpectrumFormatter::DecodeListOfParenLists(
				      istream& rStream,
				      vector<string>& rList)
{
  char paren = '(';
  skipwhite(rStream);		// Skip leading whitspace...
  while(! rStream.eof()) {
    char   c;
    string List;		// One item.
    rStream.read(&c,1);		// Should be a paren...
    if (rStream.eof()) break;	// In some C++ get eof after read.
    ThrowStreamError(rStream,"Reading what should be a paren");
    if(c != paren) {	   
      throw
	CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
			     "Looking for opening (");
    }
    List += c;
    while(1) {
      rStream.read(&c,1);
      ThrowStreamError(rStream,"Reading a list body");
      List += c;
      if(c == ')') break;

    }
    rList.push_back(List);
    List = "";
    skipwhite(rStream);		// Skip to next item or end.
  }
}
//
// Local function to ensure that spectrum limit counts are ok
// For all but summary spectra this just requires that
// the number of limits is the same as the number of dimensions..
// for summmary spectra the number of limits is 2 but the number
// of dimensions will be either 1 or 2 depending on the file format
// version number.
//
bool
CNSCLAsciiSpectrumFormatter::LimitCountOk(SpectrumType_t type,
					  size_t         limits,
					  size_t         dimensions)
{
  if (limits == dimensions) return true;

  if (type == keSummary) {
    return limits == 2;
  }
  return false;
}
