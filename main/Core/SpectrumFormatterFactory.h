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


// Class: CSpectrumFormatterFactory                     //ANSI C++
//
// Encapsulates spectrum formatter selection behind a keyword
// selection system.  Spectrum formatters are registered with
// the factory and associated with a keyword.  Formatters can
// also be removed when no longer used.  In addition,
// Formatters can be selected by keword and iterated as
// well.
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved SpectrumFormatterFactory.h
//

#ifndef SPECTRUMFORMATTERFACTORY_H  //Required for current class
#define SPECTRUMFORMATTERFACTORY_H

#include "SpectrumFormatter.h"        //Required for include files
#include <DictionaryException.h>        //Required for include files
#include <histotypes.h>
#include <string>        //Required for include files
#include <map>        //Required for include files


                               


typedef std::map<std::string, CSpectrumFormatter*>            FormatterMap;
typedef std::map<std::string, CSpectrumFormatter*>::iterator  FormatterIterator;

class CSpectrumFormatterFactory      
{                       
			
  static FormatterMap   m_Formatters;

public:

   // Constructors and other cannonical operations:

  CSpectrumFormatterFactory ();


  ~ CSpectrumFormatterFactory ( )  // Destructor 
  { }  

  
   //Copy constructor 

  CSpectrumFormatterFactory(const CSpectrumFormatterFactory& 
			    aCSpectrumFormatterFactory ) 
  { 
  }                                     

   // Operator= Assignment Operator 

  CSpectrumFormatterFactory& 
  operator=(const CSpectrumFormatterFactory& aCSpectrumFormatterFactory) {
    return *this; 
  }
 
   //Operator== Equality Operator 

  int 
  operator== (const CSpectrumFormatterFactory& aCSpectrumFormatterFactory) 
    const
  {
    return 1;
  }
	
// Selectors:

public:
  FormatterMap GetFormatters() const 
  {
    return m_Formatters;
  }
// Mutators:

protected:
  void SetFormatters(const FormatterMap& rFormatters) 
  {
    m_Formatters = rFormatters;
  }
public:

  static  void AddFormatter (const std::string& rKeyword, 
			     CSpectrumFormatter* pFormatter)    ;
  static  CSpectrumFormatter* DeleteFormatter (const std::string& rKeyword)    ;
  static  CSpectrumFormatter* MatchFormatter (const std::string& rKeyword)    ;
  static  FormatterIterator FormatterBegin ()    ;
  static  FormatterIterator FormatterEnd ()    ;
 

};
class CSpectrumStandardFormatters
{
public:
  struct RegistrationEntry {
    std::string               Keyword;
    CSpectrumFormatter*  Formatter;
  };
private:
  static Bool_t  m_fFirstTime;
  static RegistrationEntry RegistrationTable[]; 
public:
  CSpectrumStandardFormatters();

};


#endif

