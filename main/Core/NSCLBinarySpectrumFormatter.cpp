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
// Class: CNSCLBinarySpectrumFormatter
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
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

#include <config.h>

#include <histotypes.h>
#include "NSCLBinarySpectrumFormatter.h"
#include "nsclbin.h"
#include "nsclbinerror.h"
#include <Exception.h>
#include <Spectrum.h>
#include <SpectrumPackage.h>
#include <SpectrumFormatError.h>
#include <StreamIOError.h>
#include <SpectrumFactory.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <ctype.h>
#include <Histogrammer.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


extern CEventSink* gpEventSink;	// Current event sink which must really be a
				// histogrammer.
SpectrumType_t* equals(SpectrumType_t&, const int&);
int* equals(int&, const SpectrumType_t&);
DataType_t* equals(DataType_t&, const int&);
int* equals(int&, const DataType_t&);

// Functions for class CNSCLBinarySpectrumFormatter

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//     CNSCLBinarySpectrumFormatter& operator=
//     (const CNSCLBinarySpectrumFormatter& aCNSCLBinarySpectrumFormatter)
//
// Operation Type:
//    assignment.
//
CNSCLBinarySpectrumFormatter&
CNSCLBinarySpectrumFormatter::operator=
        (const CNSCLBinarySpectrumFormatter& aCNSCLBinarySpectrumFormatter) 
{
  if(this != &aCNSCLBinarySpectrumFormatter) {
    CSpectrumFormatter::operator=(aCNSCLBinarySpectrumFormatter);
  }
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:
//     int operator== 
//        (const CNSCLBinarySpectrumFormatter& aCNSCLBinarySpectrumFormatter) 
//        const;
//  Operation Type:
//      Equality comparison.
//
int
CNSCLBinarySpectrumFormatter::operator==
     (const CNSCLBinarySpectrumFormatter& aCNSCLBinarySpectrumFormatter) const
{
  return CSpectrumFormatter::operator==(aCNSCLBinarySpectrumFormatter);
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Read(istream& rStream, ParameterDictionary& rDict)
//
//  @param rStream - the stream from which to read the spectrum.
//  @param rDict   - The parameter dictionary (not used in this format.)
//  @return std::pair<std::string, CSpectrum*>  - Pair consisting of the spectrum name
//                   and a dynamically allocated anonymous spectrum that was read
//                   from the file. 
//   
std::pair<std::string, CSpectrum*>
CNSCLBinarySpectrumFormatter::Read(istream& rStream,  ParameterDictionary& rDict)  
{
  // Reads a spectrum from file.
  // 
  // Formal Parameters:
  //    istream& rStream:
  //       Refers to the stream from which the
  //       read is done.
  // Returns:
  //    Pointer to the spectrum read or
  //    throws an exception if the read fails.

  string         Name;		// Spectrum name.
  string         Date;		// Date spectrum was written.
  string         Time;
  string NullParam = "";
  SpectrumType_t eSpecType;	// Type of the spectrum
  DataType_t     eDataType;	// Data type of spectrum.
  vector <UInt_t> vDimensions;
  vector <UInt_t> vResolutions;
  vector <long> Channels;
  vector<string> vParameters;
  nsclbin SpecObject(&rStream);
  try {
    readhead(rStream, Name, Time, Date, vDimensions, eSpecType,eDataType, SpecObject);
    readdata(SpecObject,Channels);
  }
  catch (nsclfileerror *err) {   
    // catching file errors and rethrowing as CSpectrum errors

    switch ((*err).geterror()) {
    case nsclfileerror::EmptyStream:
      throw CStreamIOError(CStreamIOError::EndFile,
			   "Reading File",rStream);
      break;
    case nsclfileerror::HeaderReadError:
      throw CSpectrumFormatError(CSpectrumFormatError::InvalidHeader,
				 "Reading File");
      break;    
    case nsclfileerror::ReadErrorN:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Reading spectrum data",rStream);
      break;
    case nsclfileerror::ReadErrorS:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Reading spectrum data",rStream);
      break;
    case nsclfileerror::BufferWriteError:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Writing nsclbinary header buffer",rStream);
      break;
    case nsclfileerror::DataWriteError:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Writing nsclbinary spectrum data",rStream);
      break;
    case nsclfileerror::BitmaskWriteError:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Writing nsclbinary spectrum bitmask",rStream);
      break;
    default:
      throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
				 "Reading header");
    }
  }
  catch (nscloperror *err) {     // catching 'operational' errors

    switch ((*err).geterror()) {
    case nscloperror::InvalidFormat:
      throw CSpectrumFormatError(CSpectrumFormatError::IncompatibleFormat,
				 "Reading File");
      break;
    case nscloperror::InvalidHeader:
      throw CSpectrumFormatError(CSpectrumFormatError::InvalidHeader,
				 "Reading File");
      break;
    case nscloperror::WriteWOSetup:
      throw CException("Write without setup");
      break;
    case nscloperror::BadDataIndex:
      throw CSpectrumFormatError(CSpectrumFormatError::OverflowChannels,
				 "Reading File");
      break;
    case nscloperror::BadIndexGet:
      throw CSpectrumFormatError(CSpectrumFormatError::InvalidChannels,
				 "Reading File");
      break;
    case nscloperror::OutOfMemory:
      throw CException("Ran out of memory");
      break;
    default:
      throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
				 "Reading header");
    }
  }
  getresolutions(vDimensions,vResolutions);
  CSpectrumFactory Factory;
  Factory.ExceptionMode(kfFALSE);
  vector<UInt_t>  vChannels;
  vector<Float_t> lows;
  vector<Float_t> highs;

  for(int dims = 0;dims<vDimensions.size();dims++) {
    NullParam+=" ";
    while (parameterexist(NullParam))
      NullParam+=" ";
    vParameters.push_back(NullParam);
    vChannels.push_back(vDimensions[dims]);
    lows.push_back(0.0);
    highs.push_back((Float_t)(vDimensions[dims] - 1));
  }

  // Create temporary dummy vectors  
  vector<Float_t> vTransform;

  ///// 
  // We're going to create an anonymous spectrum so that Root doesn't get pissed
  // that we're replacing an existng TH1 object
  // 
  AssurePossibleSpectrum(eSpecType,eDataType);
  CSpectrum* pSpectrum;
  pSpectrum = Factory.CreateSpectrum("", eSpecType, eDataType, vParameters, 
				     vChannels);
  pSpectrum->Clear();
  //fill in the spectrum
  //testdata(SpecObject);
  //testdata(pSpectrum);
  insertdata(pSpectrum, Channels, vDimensions, vResolutions);
  return std::pair<std::string, CSpectrum*>(Name, pSpectrum);
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Write(ostream& rStream, CSpectrum& rSpectrum)
//  Operation Type: 
//     I/O
void CNSCLBinarySpectrumFormatter::Write(ostream& rStream, 
					 CSpectrum& rSpectrum,
					 ParameterDictionary& rDict)  
{
  // Writes a spectrum to a C++ stream in 
  // NSCL Binary format.
  //
  // Formal Parameters:
  //    ostream& rStream:
  //       Refers to the stream 
  //    CSpectrum& rSpectrum:
  //       Refers to the spectrum to write.
  //
    
  nsclbin Binaryobj(&rStream);
  char name[120];
  strcpy(name, (rSpectrum.getName()).c_str());
  
  try {
    Binaryobj.setname(name);
    Binaryobj.setdimension(rSpectrum.Dimensionality());
    setdimensions(Binaryobj, rSpectrum);
    DataType_t dType = rSpectrum.StorageType();
    int format;
    equals(format, dType);
    Binaryobj.setformat(format);
    if (Binaryobj.testinit()) {  // testing whether the object has enough data to setup
      Binaryobj.setup();     //  setting up the object to accept channel data
    }
    setdata(Binaryobj, rSpectrum);
    Binaryobj.writeout(rStream);
  }
  catch (nsclfileerror *err) {   
    // catching file errors and rethrowing as CSpectrum errors

    switch ((*err).geterror()) {
    case nsclfileerror::EmptyStream:
      throw CStreamIOError(CStreamIOError::EndFile,
			   "Reading File",rStream);
      break;
    case nsclfileerror::HeaderReadError:
      throw CSpectrumFormatError(CSpectrumFormatError::InvalidHeader,
				 "Reading File");
      break;    
    case nsclfileerror::ReadErrorN:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Reading spectrum data",rStream);
      break;
    case nsclfileerror::ReadErrorS:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Reading spectrum data",rStream);
      break;
    case nsclfileerror::BufferWriteError:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Writing nsclbinary header buffer",rStream);
      break;
    case nsclfileerror::DataWriteError:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Writing nsclbinary spectrum data",rStream);
      break;
    case nsclfileerror::BitmaskWriteError:
      throw CStreamIOError(CStreamIOError::FailSet,
			   "Writing nsclbinary spectrum bitmask",rStream);
      break;
    default:
      throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
				 "Reading header");
    }
  }
  catch (nscloperror *err) {     // catching 'operational' errors

    switch ((*err).geterror()) {
    case nscloperror::InvalidFormat:
      throw CSpectrumFormatError(CSpectrumFormatError::IncompatibleFormat,
				 "Reading File");
      break;
    case nscloperror::InvalidHeader:
      throw CSpectrumFormatError(CSpectrumFormatError::InvalidHeader,
				 "Reading File");
      break;
    case nscloperror::WriteWOSetup:
      throw CException("Write without setup");
      break;
    case nscloperror::BadDataIndex:
      throw CSpectrumFormatError(CSpectrumFormatError::OverflowChannels,
				 "Reading File");
      break;
    case nscloperror::BadIndexGet:
      throw CSpectrumFormatError(CSpectrumFormatError::InvalidChannels,
				 "Reading File");
      break;
    case nscloperror::OutOfMemory:
      throw CException("Ran out of memory");
      break;
    default:
      throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
				 "Reading header");
    }
  }
}


//Reads a spectrum header from a nsclbin object and puts it into
// parameters to be passed back.
void CNSCLBinarySpectrumFormatter::
readhead(istream& rStream, string& Name, string& Time, string& Date, 
	 vector<UInt_t>& vDimensions, SpectrumType_t& eSpecType, 
	 DataType_t& eDataType, nsclbin& SpecObject) {
  char namec[121],datec[10],timec[10];
  int format, dimension;
  SpecObject.getdata(rStream);
  SpecObject.getname(namec);        //  getting header values
  SpecObject.gettime(timec);  
  SpecObject.getdate(datec);
  equals(eSpecType, SpecObject.getdimension());
  equals(eDataType,SpecObject.getformat());
  vDimensions.push_back(SpecObject.getxlength());
  if (SpecObject.getylength() >0) vDimensions.push_back(SpecObject.getylength());
  Name=namec;
  Time=timec;
  Date=datec;
}
// void readdata(nsclbin& SpecObject, vector<long>& vChannels)
// 
//  Reads spectrum data from nsclbin object and push_backs it into the vChannels vector.
//  
void CNSCLBinarySpectrumFormatter::
readdata(nsclbin& SpecObject, vector<long>& vChannels) {
  long elements = SpecObject.getxlength()*SpecObject.getylength();
  if (elements == 0) elements = SpecObject.getxlength();
  while (vChannels.size()<elements)
    vChannels.push_back(SpecObject.get(vChannels.size()));
}
  
//
// equals(...)
//  
//  The following overloaded functions convert from SpectrumType_t to integer,
//  integer to SpectrumType_t, DataType_t to integer, and integer to Datatype_t.
//  the ..._t types are compatible with SpecTcl, and the integer values are 
//  compatible with the nsclbin object.
//



SpectrumType_t* equals(SpectrumType_t& lhs, const int& rhs) {
  switch (rhs) {
  case 1:
    lhs = ke1D;
    break;
  case 2:
    lhs = ke2D;
    break;
  default:
    //***unknown dimension type
    throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
			       "Decoding spectrum type");
  }
  return &lhs;
}

int* equals(int& lhs, const SpectrumType_t& rhs) {
  lhs=(int)rhs+1;
  if ((lhs!=1) &&(lhs!=2))     
    throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
			       "Decoding spectrum type");
  return &lhs;
}

DataType_t* equals(DataType_t& lhs, const int& rhs) {
  switch (rhs) {
  case 1:
    lhs = keByte;
    break;
  case 2:
    lhs= keWord;
    break;
  case 4:
    lhs = keLong;
    break;
  default:
    //**unknown format type
    throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
						 "Decoding spectrum type");
  }
  return &lhs;
}

int* equals(int& lhs, const DataType_t& rhs) {
  switch (rhs) {
  case keByte:
    lhs = 1;
    break;
  case keWord:
    lhs= 2;
    break;
  case keLong:
    lhs = 4;
    break;
  default:
    //**unknown format type
    throw CSpectrumFormatError(CSpectrumFormatError::FileFormatCorrupt,
			       "Decoding spectrum type");
  }
  return &lhs; 
}
//
// bool parameterexist(string& parameter)
//
//  Returns a boolean value specifying whether a parameter with the name passed
//  already exists in SpecTcl.  
//  1 parameter exists
//  0 parameter does not exist
//
bool CNSCLBinarySpectrumFormatter::
parameterexist(string& parameter) {
  CHistogrammer*     pSorter = (CHistogrammer*)gpEventSink;
  return (pSorter->FindParameter(parameter )!=kpNULL);
}
//
//insertdata(CSpectrum *spectrum, vector<long>& Channels, 
//           const vector<UInt_t>& vDimensions, const vector<UInt_t>& vResolutions) {
//  Inserts data from the Channels vector into the CSpectrum.  It uses getindices to 
//  convert from the one dimensional 'i' index to indices of the spectrum's dimensionality, 
//  deposits these indeces into the channelarray and then calls the set function with the
//  channelarray and the data value from the Channels vector.
//
void CNSCLBinarySpectrumFormatter::
insertdata(CSpectrum *spectrum, vector<long>& Channels, const vector<UInt_t>& vDimensions, const vector<UInt_t>& vResolutions) {
  UInt_t* channelarray = new UInt_t[vDimensions.size()];
  for(UInt_t i = 0; i<Channels.size();i++) {
    getindices(i, 
	       vDimensions, 
	       channelarray);
    if (goodchannel(channelarray, vDimensions, vResolutions))
      spectrum->set(channelarray,Channels[i]);
  }
  delete []channelarray;
}
//
//void getresolutions(const vector<UInt_t>& vDimensions, vector<UInt_t>& vResolutions)
//
//  Computes resolutions from dimension sizes.  A resolution is equal to the log base two
//  of the corresponding dimension size. 
//
void CNSCLBinarySpectrumFormatter::
getresolutions(const vector<UInt_t>& vDimensions, vector<UInt_t>& vResolutions) {
  for (int dims = 0; dims<vDimensions.size();dims++) {
#ifdef HAVE_LOGB
    vResolutions.push_back((UInt_t)logb((float)vDimensions[dims]));
#else
    vResolutions.push_back((UInt_t)(log10((float)vDimensions[dims]) /
				    log10(2.0)));
#endif
  }
}

//void getindices(UInt_t index, const vector<UInt_t>& vDimensions, UInt_t *indexarray)
//
//From a single index produces a corresponding sequence of indices based on
// the number and size of the dimensions.
// UInt_t index                                the single index value
//
// const vector<<UInt_t>& vDimensions          vector of dimensions.  individual
//                                             elements hold dimension sizes and
//                                             vDimensions.size holds spectrum
//                                             dimensionality.
//
//UInt_t *indexarray                           array of indices returned
//
void CNSCLBinarySpectrumFormatter::
getindices(UInt_t index, const vector<UInt_t>& vDimensions, UInt_t *indexarray) {
  UInt_t data_area=1;
  for(int i = vDimensions.size()-1;i>=0;i--) {
    data_area*=vDimensions[i];
  }
  for(int dims = vDimensions.size()-1; dims>=0;dims--) {
    data_area/=vDimensions[dims];
    indexarray[dims] = (UInt_t)(index/data_area);
    index %= data_area;
  }
} 
//void setdimensions(nsclbin& Obj, const CSpectrum& rSpectrum)
//  
//  Reads dimension sizes from the CSpectrum and inserts them into the 
//  nsclbin object.  This function will have to be updated when the capabilty to
//  represent dimensionality greater than two is added to SpecTcl.  The nsclbin class
//  will also have to be updated to provide for this capability.
//
void CNSCLBinarySpectrumFormatter::
setdimensions(nsclbin& Obj, const CSpectrum& rSpectrum) {
  Obj.setxlength(rSpectrum.Dimension(0));
  Obj.setylength(rSpectrum.Dimension(1));
}

// void setdata(nsclbin& Obj, const CSpectrum& rSpectrum)
//
// Inserts data values into the nsclbin object.  While doing the insert
// it also computes the channel count and the number of non-zero elements, 
// which are then also inserted into the nsclbin object.
//
void CNSCLBinarySpectrumFormatter::
setdata(nsclbin& Obj, const CSpectrum& rSpectrum) {
  int els = 1,elements = 0,tot = 0;
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
    if(Value != 0) {
      tot+=Value;
      elements++;
    }
    WriteVal(Obj, nCoordinates, pIdx, Value);
  } while(NextIndex(nCoordinates, pLows, pHis, pIdx));
  Obj.setcount(tot);
  Obj.setelements(elements);
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
CNSCLBinarySpectrumFormatter::NextIndex(UInt_t nCoords, 
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

//Writes one value into a specified location in the nsclbin object.
void CNSCLBinarySpectrumFormatter::
WriteVal(nsclbin& Obj, UInt_t  nCoords, UInt_t* pCoords, ULong_t nValue) {
  int x = pCoords[0], y=0;
  if (nCoords>1) 
    y+=pCoords[1];
  Obj.put(x,y,nValue);
}

//Assures that the combination of eSpecType and eDataType are valid.
//Looks for one dimensional byte spectrum and turns them into 
// one dimensional word spectrum.
void CNSCLBinarySpectrumFormatter::
AssurePossibleSpectrum(SpectrumType_t& eSpecType, DataType_t& eDataType) {
  if ((eDataType ==  keByte)&&(eSpecType == ke1D))
    eDataType = keWord;
}
void CNSCLBinarySpectrumFormatter::
testdata(CSpectrum *pSpectrum) {
  cout <<"name: "<<pSpectrum->getName()<<"\n";
  cout <<"data type: "<<pSpectrum->StorageType()<<"\n";
  cout <<"dimensions: "<<pSpectrum->Dimensionality()<<"\n";
  cout <<"x dimension: "<<pSpectrum->Dimension(0)<<"\n";
  cout <<"y dimension: "<<pSpectrum->Dimension(1)<<"\n";
}
void CNSCLBinarySpectrumFormatter::
testdata(nsclbin& obj) {
  char name[120];
  obj.getname(name);
  cout <<"name: "<<name<<"\n";
  cout <<"data type: "<<obj.getformat()<<"\n";
  cout <<"dimensions: "<<obj.getdimension()<<"\n";
  cout <<"x dimension: "<<obj.getxlength()<<"\n";
  cout <<"y dimension: "<<obj.getylength()<<"\n";
}
// Bool_t goodchannel(goodchannel(UInt_t* channelarray,  
//         const vector<UInt_t>& vDimensions, const vector<UInt_t>& vResolutions)
//
//Returns a boolean value signifying whether the indices held in channelarray
//  are valid channel indices for the spectrum.  Since SpecTcl defines dimension sizes
//  using the resolutions (a resolution is the log base two of the dimension size)
//  all valid SpecTcl spectra must have dimension sizes which are powers of two.
//  So instead of making channel indices bound by the maximum of the dimension size
//  they must be bound by 2 to the power of the resolution.  This way in the case that 
//  the spectrum being read does not have dimensions that are powers of two, it will 
//  still be read as valid SpecTcl spectra (though missing some channels on the edges).
//
//  1 channel is good
//  0 channel is out of bounds
Bool_t CNSCLBinarySpectrumFormatter::
goodchannel(UInt_t* channelarray,  const vector<UInt_t>& vDimensions, const vector<UInt_t>& vResolutions) {
  Bool_t isgood = 1;
  for(int i = 0; i<vDimensions.size();i++) {
    
    if (channelarray[i]>= vDimensions[i]) {
      isgood=0;
      break;
    }

  }
  return isgood;
}
//UInt_t f2n(UInt_t power)
//
//returns the value of 2 raised to the exponent 'power'
//
UInt_t CNSCLBinarySpectrumFormatter::
f2n(UInt_t power) {
  UInt_t val = 1;
  for(int i = 0; i<power;i++) {
    val*=2;
  }    
  return val;
}
