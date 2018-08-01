   
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


#ifndef CCALIBRATEDPARAMETERMANAGER_H  //Required for current class
#define CCALIBRATEDPARAMETERMANAGER_H

//
// Include files:
//


#include <EventProcessor.h>
#include <histotypes.h>        //Required for include files 
#include <string>        //Required for include files  
#include <map>

// Forward class definitions.

class CCalibratedParameter;

// Class interface:

class CCalibratedParameterManager : public CEventProcessor     
{
	// Data types exported to the world.
public:
	typedef std::pair<std::string, CCalibratedParameter*> MapEntry;
	typedef std::map<std::string, CCalibratedParameter*> CalibratedParameterDictionary;
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
    static void AddParameter (std::string rName, CCalibratedParameter* pParam)   ; 
    static CCalibratedParameter* DeleteParameter (std::string rName)   ; 
    static CalibratedParameterIterator begin ()   ; 
    static CalibratedParameterIterator end ()   ; 
    static size_t size ()   ; 
    static CalibratedParameterIterator FindParameter (std::string rName)   ; 
    
    // Private utilities:
private:
    static std::string ReportEvaluationException(CalibratedParameterIterator i);
    template <class T> static void
    OutputEvaluationException(std::string prefix,
                              T&      rExcept); //!< Output writable exceptions.

};

#endif
