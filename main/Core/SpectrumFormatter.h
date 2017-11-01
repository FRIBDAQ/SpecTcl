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

#ifndef __OSTREAM_DAQH
#include <ostream>
#endif

#ifndef __ISTREAM_DAQH
#include <istream>
#endif



typedef CDictionary<CParameter>                   ParameterDictionary;
typedef ParameterDictionary::DictionaryIterator   ParameterDictinoaryIterator;

class CSpectrum;
                               
class CSpectrumFormatter      
{                       
			
   std::string m_Description; //Describes the formatter (e.g. "NSCL ASCII format").
   std::string m_FileType; //File type typically associated with a spectrum (e.g. .spa).        

protected:

public:

   // Constructors and other cannonical operations:

  CSpectrumFormatter (const std::string& rDescription,
		      const std::string& rFileType)    : 
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

  std::string getDescription() const
  { 
    return m_Description;
  }
  std::string getFileType() const
  { 
    return m_FileType;
  }
                       
// Mutators:

protected:

  void setDescription (const std::string am_Description)
  { 
    m_Description = am_Description;
  }
  void setFileType (const std::string am_FileType)
  { 
    m_FileType = am_FileType;
  }
       
public:

  // Pure virtual interface definitions.
  //
 virtual   std::pair<std::string, CSpectrum*> Read (std::istream& rStream, 
			    ParameterDictionary& rDict)   = 0  ;
 virtual   void Write (std::ostream& rStream, CSpectrum& rSpectrum,
		       ParameterDictionary& rDict)   = 0  ;
 

};

#endif
