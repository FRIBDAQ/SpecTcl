   
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

//! \class: CCalibratedParameterManager           
//! \file:  .h
/*!
  \class CCalibratedParameterManager
  \file  .h

  This EventProcessor manages a set of calibrated parameters.
  In addition to the normal evaluation operation, the class supports
  member functions for manipulating the list of parameters it manage:
  - AddParameter to add a new calibrated parameter.
  - DeleteParameter to remove an existing parameter.
  - FindParameter to locate a parameter.
  - begin, end, size to support iteration.

*/


#ifndef __CCALIBRATEDPARAMETERMANAGER_H  //Required for current class
#define __CCALIBRATEDPARAMETERMANAGER_H

#ifndef ___EVENTPROCESSOR_H
#include <EventProcessor.h>
#endif

//
// Include files:
//

#ifndef __HISTOTYPES_H
#include <histotypes.h>        //Required for include files 
#endif

#ifndef __STL_STRING
#include <string>        //Required for include files  
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif

// Forward class definitions.

class CCalibratedParameter;

// Class interface:

class CCalibratedParameterManager : public CEventProcessor     
{
	// Data types exported to the world.
public:
	typedef pair<string, CCalibratedParameter*> MapEntry;
	typedef map<string, CCalibratedParameter*> CalibratedParameterDictionary;
	typedef CalibratedParameterDictionary::iterator CalibratedParameterIterator;
private:
  
  	static CalibratedParameterDictionary m_CalibratedParameters; 


public:
    //  Constructors and other canonical operations.
 
    CCalibratedParameterManager ();		//!< Constructor.
    virtual  ~ CCalibratedParameterManager ( ); //!< Destructor.
    CCalibratedParameterManager (const CCalibratedParameterManager& rSource ); //!< Copy construction.
    CCalibratedParameterManager& operator= (const CCalibratedParameterManager& rhs); //!< Assignment.
    int operator== (const CCalibratedParameterManager& rhs) const; //!< == comparison.
    int operator!= (const CCalibratedParameterManager& rhs) const; //!< != comparison.

// Class operations:

public:

	virtual   Bool_t operator() (const Address_t pEvent, CEvent& rEvent, 
							     CAnalyzer& rAnalyzer, 
							     CBufferDecoder& rDecoder)   ; 
    static void AddParameter (string rName, CCalibratedParameter* pParam)   ; 
    static CCalibratedParameter* DeleteParameter (string rName)   ; 
    static CalibratedParameterIterator begin ()   ; 
    static CalibratedParameterIterator end ()   ; 
    static size_t size ()   ; 
    static CalibratedParameterIterator FindParameter (string rName)   ; 
    
    // Private utilities:
private:
    static string ReportEvaluationException(CalibratedParameterIterator i);
    template <class T> static void
    OutputEvaluationException(string prefix,
                              T&      rExcept); //!< Output writable exceptions.

};

#endif
