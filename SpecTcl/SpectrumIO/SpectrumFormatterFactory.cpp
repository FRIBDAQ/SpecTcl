// Class: CSpectrumFormatterFactory
// Encapsulates spectrum formatter selection behind a keyword
// selection system.  Spectrum formatters are registered with
// the factory and associated with a keyword.  Formatters can
// also be removed when no longer used.  In addition,
// Formatters can be selected by keword and iterated as
// well.
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
#include "SpectrumFormatterFactory.h"    				

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved SpectrumFormatterFactory.cpp \n";


// Class static data members.

FormatterMap CSpectrumFormatterFactory::m_Formatters;
//
//  The trick below ensures that the first instantiation of a formatter
//  factory will register standard formatters.
//

static CSpectrumStandardFormatters __RegisterFormatters;

// Functions for class CSpectrumFormatterFactory

CSpectrumFormatterFactory::CSpectrumFormatterFactory()
{
}

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     AddFormatter(const string& rKeyword, CSpectrumFormatter* pFormatter)
//  Operation Type: 
//     Mutator.
void 
CSpectrumFormatterFactory::AddFormatter(const string& rKeyword, 
					CSpectrumFormatter* pFormatter)  
{
  // Adds a formatter to the format list:
  //
  //  Formal Parameters:
  //      const string& rKeyword:
  //              Keyword associated with this formatter.
  //      CSpectrumFormatter* pFormatter:
  //              Pointer to a formatter to add to the list.
  //              The scope of the formatter must be
  //              such that it will remain active as long
  //              as it is in the list.
  //  throws:
  //      CDictionary exception if the formatter
  //      cannot be added.

  if(m_Formatters.find(rKeyword) != m_Formatters.end()) {
    
    // Need to throw a duplicate keyword exception...

    throw CDictionaryException(CDictionaryException::knDuplicateKey,
     "Attempting to insert spectrum file formatter into formatter dictionary.",
			       rKeyword.c_str());
  }
  m_Formatters[rKeyword] = pFormatter;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     DeleteFormatter(const string& rKeyword)
//  Operation Type: 
//     Mutator.
CSpectrumFormatter* 
CSpectrumFormatterFactory::DeleteFormatter(const string& rKeyword)  
{
  // Removes a formatter from the list of
  // formatters.  A pointer to the formatter
  // is passed back or Null if the formatter
  // does not exist.
  //
  // Formal Parameters:
  //    const string& rKeyword:
  //        Keyword associated with the formatter.
  //
  FormatterIterator It = m_Formatters.find(rKeyword);
  if(It == m_Formatters.end()) {
    return (CSpectrumFormatter*)kpNULL;
  }
  else {
    m_Formatters.erase(It);
    return ((*It).second);
  }

}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     MatchFormatter(const string& rKeyword.)
//  Operation Type: 
//     Selector.
CSpectrumFormatter* 
CSpectrumFormatterFactory::MatchFormatter(const string& rKeyword)  
{
  // Returns the formatter which matches the
  // selected keyword.
  // 
  // Formal Parameters:
  //    const string& rKeyword:
  //       Keyword to match.
  //
  // Returns:
  //    Pointer to that formatter or a 
  //    NULL if there isn't a match.
  //

  FormatterIterator It = m_Formatters.find(rKeyword);
  if(It == m_Formatters.end()) {
    return (CSpectrumFormatter*)kpNULL;
  }
  else {
    return ((*It).second);
  }
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     FormattersBegin()
//  Operation Type: 
//     Selector.
FormatterIterator
CSpectrumFormatterFactory::FormatterBegin()  
{
  // Returns an iterator which allows the Formatters map
  // to be traversed.
  //
  //
  return m_Formatters.begin();
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     FormattersEnd()
//  Operation Type: 
//     selector
FormatterIterator 
CSpectrumFormatterFactory::FormatterEnd()  
{
  // Returns an iterator representing the 
  // end of an iteration through the Formatters
  // map.
  return m_Formatters.end();
}

