
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

#ifndef __SPECTRUMFORMATTERFACTORY_H  //Required for current class
#define __SPECTRUMFORMATTERFACTORY_H

#ifndef __SPECTRUMFORMATTER_H
#include "SpectrumFormatter.h"        //Required for include files
#endif

#ifndef __DICTIONARYEXCEPTION_H
#include <DictionaryException.h>        //Required for include files
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>        //Required for include files
#define __STL_STRING
#endif

#ifndef __STL_MAP
#include <map>        //Required for include files
#define __STL_MAP
#endif


                               


typedef map<string, CSpectrumFormatter*>            FormatterMap;
typedef map<string, CSpectrumFormatter*>::iterator  FormatterIterator;

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

  static  void AddFormatter (const string& rKeyword, 
			     CSpectrumFormatter* pFormatter)    ;
  static  CSpectrumFormatter* DeleteFormatter (const string& rKeyword)    ;
  static  CSpectrumFormatter* MatchFormatter (const string& rKeyword)    ;
  static  FormatterIterator FormatterBegin ()    ;
  static  FormatterIterator FormatterEnd ()    ;
 

};
class CSpectrumStandardFormatters
{
public:
  struct RegistrationEntry {
    string               Keyword;
    CSpectrumFormatter*  Formatter;
  };
private:
  static Bool_t  m_fFirstTime;
  static RegistrationEntry RegistrationTable[]; 
public:
  CSpectrumStandardFormatters();

};


#endif

