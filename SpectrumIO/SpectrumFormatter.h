// Class: CSpectrumFormatter                     //ANSI C++
//
// Base class of spectrum formatters.   Derivations
// of this class are responsible for reading and writing
// spectra from C++ streams. 
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved SpectrumFormatter.h
//

#ifndef __SPECTRUMFORMATTER_H  //Required for current class
#define __SPECTRUMFORMATTER_H

#ifndef __PARAMETER_H
#include <Parameter.h>
#endif

#ifndef __DICTIONARY_H
#include <Dictionary.h>
#endif


#ifndef __STL_STRING
#include <string>        //Required for include files
#define __STL_STRING
#endif

typedef CDictionary<CParameter>                   ParameterDictionary;
typedef ParameterDictionary::DictionaryIterator   ParameterDictinoaryIterator;

class CSpectrum;
                               
class CSpectrumFormatter      
{                       
			
   string m_Description; //Describes the formatter (e.g. "NSCL ASCII format").
   string m_FileType; //File type typically associated with a spectrum (e.g. .spa).        

protected:

public:

   // Constructors and other cannonical operations:

  CSpectrumFormatter (const string& rDescription,
		      const string& rFileType)    : 
    m_Description(rDescription),   
    m_FileType(rFileType) 
  { 
  } 
  virtual  ~ CSpectrumFormatter ( )
  { 
  }  


  CSpectrumFormatter (const CSpectrumFormatter& aCSpectrumFormatter ) 
  { 
    
    m_Description = aCSpectrumFormatter.m_Description;
    m_FileType = aCSpectrumFormatter.m_FileType;
    
  }                                     

   // Operator= Assignment Operator 

  const 
  CSpectrumFormatter& operator=(const CSpectrumFormatter& aCSpectrumFormatter);
 
   //Operator== Equality Operator 

  int operator== (const CSpectrumFormatter& aCSpectrumFormatter) const;
  int operator!= (const CSpectrumFormatter& aCSpectrumFormatter) const 
  {
    return !(operator==(aCSpectrumFormatter));
  }
// Selectors:

public:

  string getDescription() const
  { 
    return m_Description;
  }
  string getFileType() const
  { 
    return m_FileType;
  }
                       
// Mutators:

protected:

  void setDescription (const string am_Description)
  { 
    m_Description = am_Description;
  }
  void setFileType (const string am_FileType)
  { 
    m_FileType = am_FileType;
  }
       
public:

  // Pure virtual interface definitions.
  //
 virtual   CSpectrum* Read (istream& rStream, 
			    ParameterDictionary& rDict)   = 0  ;
 virtual   void Write (ostream& rStream, CSpectrum& rSpectrum,
		       ParameterDictionary& rDict)   = 0  ;
 

};

#endif
